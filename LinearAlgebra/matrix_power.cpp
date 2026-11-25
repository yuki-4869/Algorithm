namespace matrix_power_detail {
inline void require(bool condition) {
    if (!condition) std::abort();
}

inline std::size_t area(std::size_t rows, std::size_t cols) {
    require(rows == 0 || cols <= std::numeric_limits<std::size_t>::max() / rows);
    return rows * cols;
}

template<class Matrix>
Matrix pow(const Matrix& a, std::uint64_t k) {
    require(a.rows() == a.cols());
    if (k == 0) return Matrix::identity(a.rows());
    Matrix power = a;
    for (auto bit = std::bit_floor(k) >> 1; bit != 0; bit >>= 1) {
        power = power * power;
        if (k & bit) power = power * a;
    }
    return power;
}

template<class Matrix>
std::vector<typename Matrix::value_type> pow_apply(
        const Matrix& a, std::uint64_t k, std::vector<typename Matrix::value_type> v) {
    require(a.rows() == a.cols() && v.size() == a.cols());
    if (k == 0) return v;
    if (k & 1) v = a.apply(v);
    if ((k >>= 1) == 0) return v;

    Matrix power = a * a;
    for (;;) {
        if (k & 1) v = power.apply(v);
        if ((k >>= 1) == 0) return v;
        power = power * power;
    }
}

template<class Matrix>
std::pair<Matrix, Matrix> pow_sum(const Matrix& a, std::uint64_t k) {
    require(a.rows() == a.cols());
    if (k == 0) return {Matrix::identity(a.rows()), Matrix(a.rows(), a.cols())};
    Matrix power = a, sum = Matrix::identity(a.rows());
    for (auto bit = std::bit_floor(k) >> 1; bit != 0; bit >>= 1) {
        sum = sum + power * sum;
        power = power * power;
        if (k & bit) {
            sum = sum + power;
            power = power * a;
        }
    }
    return {std::move(power), std::move(sum)};
}

struct MinPlus {
    using value_type = long long;
    static constexpr value_type zero() { return std::numeric_limits<value_type>::max(); }
    static constexpr value_type one() { return 0; }
    static value_type add(value_type a, value_type b) { return std::min(a, b); }
    static value_type mul(value_type a, value_type b) {
        if (a == zero() || b == zero()) return zero();
        const __int128 sum = static_cast<__int128>(a) + b;
        require(std::numeric_limits<value_type>::min() <= sum && sum < zero());
        return static_cast<value_type>(sum);
    }
};

struct Boolean {
    using value_type = std::uint8_t;
    static constexpr value_type zero() { return 0; }
    static constexpr value_type one() { return 1; }
    static value_type add(value_type a, value_type b) { return a || b; }
    static value_type mul(value_type a, value_type b) { return a && b; }
};
}

template<std::uint32_t Mod = 998244353>
class MatrixMod {
    static_assert(2 <= Mod && Mod <= (1u << 30));

public:
    using value_type = std::uint32_t;
    using size_type = std::size_t;

    MatrixMod(size_type rows, size_type cols)
        : rows_(rows), cols_(cols), data_(matrix_power_detail::area(rows, cols)) {}

    size_type rows() const { return rows_; }
    size_type cols() const { return cols_; }
    static constexpr value_type mod() { return Mod; }

    value_type* operator[](size_type row) {
        matrix_power_detail::require(row < rows_);
        return cols_ == 0 ? data_.data() : data_.data() + row * cols_;
    }
    const value_type* operator[](size_type row) const {
        matrix_power_detail::require(row < rows_);
        return cols_ == 0 ? data_.data() : data_.data() + row * cols_;
    }

    void set(size_type row, size_type col, std::int64_t value) {
        matrix_power_detail::require(col < cols_);
        std::int64_t residue = value % static_cast<std::int64_t>(Mod);
        (*this)[row][col] = static_cast<value_type>(residue < 0 ? residue + Mod : residue);
    }

    static MatrixMod identity(size_type n) {
        MatrixMod result(n, n);
        for (size_type i = 0; i < n; ++i) result[i][i] = 1;
        return result;
    }

    MatrixMod mul_classic(const MatrixMod& rhs) const {
        matrix_power_detail::require(cols_ == rhs.rows_);
        MatrixMod result(rows_, rhs.cols_);
        if (rows_ == 0 || cols_ == 0 || rhs.cols_ == 0) return result;

        std::vector<value_type> transposed(cols_ * std::min(rhs.cols_, panel_width));
        multiply_classic(data_.data(), cols_, rhs.data_.data(), rhs.cols_,
                         result.data_.data(), result.cols_, rows_, cols_, rhs.cols_,
                         transposed.data());
        return result;
    }

    MatrixMod operator*(const MatrixMod& rhs) const {
        matrix_power_detail::require(cols_ == rhs.rows_);
        if (rows_ != cols_ || rows_ != rhs.cols_ || rows_ < strassen_start) {
            return mul_classic(rhs);
        }

        const size_type n = rows_;
        size_type divisor = 1;
        while ((n - 1) / divisor >= strassen_leaf) divisor *= 2;
        const size_type padded = (1 + (n - 1) / divisor) * divisor;
        std::vector<value_type> scratch(padded * padded);

        if (padded == n) {
            MatrixMod result(n, n);
            multiply_square(data_.data(), n, rhs.data_.data(), n,
                            result.data_.data(), n, n, scratch.data());
            return result;
        }

        MatrixMod lhs_pad(padded, padded), rhs_pad(padded, padded);
        MatrixMod result_pad(padded, padded), result(n, n);
        for (size_type i = 0; i < n; ++i) {
            std::copy_n((*this)[i], n, lhs_pad[i]);
            std::copy_n(rhs[i], n, rhs_pad[i]);
        }
        multiply_square(lhs_pad.data_.data(), padded, rhs_pad.data_.data(), padded,
                        result_pad.data_.data(), padded, padded, scratch.data());
        for (size_type i = 0; i < n; ++i) std::copy_n(result_pad[i], n, result[i]);
        return result;
    }

    MatrixMod operator+(const MatrixMod& rhs) const {
        matrix_power_detail::require(rows_ == rhs.rows_ && cols_ == rhs.cols_);
        MatrixMod result(rows_, cols_);
        for (size_type i = 0; i < data_.size(); ++i) result.data_[i] = add(data_[i], rhs.data_[i]);
        return result;
    }

    std::vector<value_type> apply(const std::vector<value_type>& v) const {
        matrix_power_detail::require(v.size() == cols_);
        std::vector<value_type> result(rows_);
        if (cols_ != 0) {
            for (size_type i = 0; i < rows_; ++i) result[i] = dot((*this)[i], v.data(), cols_);
        }
        return result;
    }

    MatrixMod pow(std::uint64_t k) const {
        return matrix_power_detail::pow(*this, k);
    }

    std::vector<value_type> pow_apply(std::uint64_t k, std::vector<value_type> v) const {
        return matrix_power_detail::pow_apply(*this, k, std::move(v));
    }

    std::pair<MatrixMod, MatrixMod> pow_sum(std::uint64_t k) const {
        return matrix_power_detail::pow_sum(*this, k);
    }

private:
    static constexpr size_type panel_width = 32;
    static constexpr size_type strassen_leaf = 96;
    static constexpr size_type strassen_start = 288;
    size_type rows_, cols_;
    std::vector<value_type> data_;

    static value_type add(value_type a, value_type b) {
        const value_type sum = a + b;
        return sum < Mod ? sum : sum - Mod;
    }
    static value_type sub(value_type a, value_type b) {
        return a >= b ? a - b : a + Mod - b;
    }

    static std::uint64_t product(value_type a, value_type b) {
        return static_cast<std::uint64_t>(a) * b;
    }

    static value_type dot(const value_type* a, const value_type* b, size_type n) {
        std::uint64_t residue = 0;
        size_type k = 0;
        for (; n - k >= 16; k += 16) {
            std::uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
            for (size_type t = k; t < k + 16; t += 4) {
                s0 += product(a[t], b[t]);
                s1 += product(a[t + 1], b[t + 1]);
                s2 += product(a[t + 2], b[t + 2]);
                s3 += product(a[t + 3], b[t + 3]);
            }
            residue = (residue + s0 + s1 + s2 + s3) % Mod;
        }
        if (k == n) return static_cast<value_type>(residue);
        for (; k < n; ++k) residue += product(a[k], b[k]);
        return static_cast<value_type>(residue % Mod);
    }

    static void multiply_classic(const value_type* a, size_type a_stride,
                                 const value_type* b, size_type b_stride,
                                 value_type* c, size_type c_stride, size_type rows,
                                 size_type inner, size_type cols, value_type* transposed) {
        for (size_type j0 = 0; j0 < cols; j0 += panel_width) {
            const size_type j_end = std::min(j0 + panel_width, cols);
            for (size_type k0 = 0; k0 < inner; k0 += panel_width) {
                const size_type k_end = std::min(k0 + panel_width, inner);
                for (size_type k = k0; k < k_end; ++k) {
                    for (size_type j = j0; j < j_end; ++j) {
                        transposed[(j - j0) * inner + k] = b[k * b_stride + j];
                    }
                }
            }
            for (size_type i = 0; i < rows; ++i) {
                for (size_type j = j0; j < j_end; ++j) {
                    c[i * c_stride + j] = dot(a + i * a_stride,
                                              transposed + (j - j0) * inner, inner);
                }
            }
        }
    }

    template<bool subtract = false>
    static void combine(const value_type* a, const value_type* b, size_type stride,
                        value_type* dest, size_type n) {
        for (size_type i = 0; i < n; ++i) {
            for (size_type j = 0; j < n; ++j) {
                dest[i * n + j] = subtract ? sub(a[i * stride + j], b[i * stride + j])
                                          : add(a[i * stride + j], b[i * stride + j]);
            }
        }
    }

    template<int sign>
    static void accumulate(value_type* dest, size_type stride, const value_type* src,
                           size_type n) {
        for (size_type i = 0; i < n; ++i) {
            for (size_type j = 0; j < n; ++j) {
                value_type& entry = dest[i * stride + j];
                const value_type value = src[i * n + j];
                if constexpr (sign == 0) entry = value;
                if constexpr (sign == 1) entry = add(entry, value);
                if constexpr (sign == -1) entry = sub(entry, value);
            }
        }
    }

    static void multiply_square(const value_type* a, size_type a_stride,
                                 const value_type* b, size_type b_stride,
                                 value_type* c, size_type c_stride,
                                 size_type n, value_type* scratch) {
        if (n <= strassen_leaf) {
            multiply_classic(a, a_stride, b, b_stride, c, c_stride, n, n, n, scratch);
            return;
        }

        const size_type h = n / 2;
        const size_type block_size = h * h;
        const auto* a11 = a;
        const auto* a12 = a + h;
        const auto* a21 = a + h * a_stride;
        const auto* a22 = a21 + h;
        const auto* b11 = b;
        const auto* b12 = b + h;
        const auto* b21 = b + h * b_stride;
        const auto* b22 = b21 + h;
        auto* c11 = c;
        auto* c12 = c + h;
        auto* c21 = c + h * c_stride;
        auto* c22 = c21 + h;

        auto* lhs = scratch;
        auto* rhs = lhs + block_size;
        auto* p = rhs + block_size;
        auto* child_scratch = p + block_size;

        combine(a11, a22, a_stride, lhs, h);
        combine(b11, b22, b_stride, rhs, h);
        multiply_square(lhs, h, rhs, h, p, h, h, child_scratch);
        accumulate<0>(c11, c_stride, p, h);
        accumulate<0>(c22, c_stride, p, h);

        combine(a21, a22, a_stride, lhs, h);
        multiply_square(lhs, h, b11, b_stride, p, h, h, child_scratch);
        accumulate<0>(c21, c_stride, p, h);
        accumulate<-1>(c22, c_stride, p, h);

        combine<true>(b12, b22, b_stride, rhs, h);
        multiply_square(a11, a_stride, rhs, h, p, h, h, child_scratch);
        accumulate<0>(c12, c_stride, p, h);
        accumulate<1>(c22, c_stride, p, h);

        combine<true>(b21, b11, b_stride, rhs, h);
        multiply_square(a22, a_stride, rhs, h, p, h, h, child_scratch);
        accumulate<1>(c11, c_stride, p, h);
        accumulate<1>(c21, c_stride, p, h);

        combine(a11, a12, a_stride, lhs, h);
        multiply_square(lhs, h, b22, b_stride, p, h, h, child_scratch);
        accumulate<-1>(c11, c_stride, p, h);
        accumulate<1>(c12, c_stride, p, h);

        combine<true>(a21, a11, a_stride, lhs, h);
        combine(b11, b12, b_stride, rhs, h);
        multiply_square(lhs, h, rhs, h, p, h, h, child_scratch);
        accumulate<1>(c22, c_stride, p, h);

        combine<true>(a12, a22, a_stride, lhs, h);
        combine(b21, b22, b_stride, rhs, h);
        multiply_square(lhs, h, rhs, h, p, h, h, child_scratch);
        accumulate<1>(c11, c_stride, p, h);
    }
};

template<class Ops>
class SemiringMatrix {
public:
    using value_type = typename Ops::value_type;
    using size_type = std::size_t;

    SemiringMatrix(size_type rows, size_type cols)
        : rows_(rows), cols_(cols), data_(matrix_power_detail::area(rows, cols), zero()) {}
    size_type rows() const { return rows_; }
    size_type cols() const { return cols_; }
    static value_type zero() { return Ops::zero(); }
    static value_type one() { return Ops::one(); }

    value_type* operator[](size_type row) {
        matrix_power_detail::require(row < rows_);
        return cols_ == 0 ? data_.data() : data_.data() + row * cols_;
    }
    const value_type* operator[](size_type row) const {
        matrix_power_detail::require(row < rows_);
        return cols_ == 0 ? data_.data() : data_.data() + row * cols_;
    }

    static SemiringMatrix identity(size_type n) {
        SemiringMatrix result(n, n);
        for (size_type i = 0; i < n; ++i) result[i][i] = one();
        return result;
    }

    SemiringMatrix operator+(const SemiringMatrix& rhs) const {
        matrix_power_detail::require(rows_ == rhs.rows_ && cols_ == rhs.cols_);
        SemiringMatrix result(rows_, cols_);
        for (size_type i = 0; i < data_.size(); ++i) {
            result.data_[i] = Ops::add(data_[i], rhs.data_[i]);
        }
        return result;
    }

    SemiringMatrix operator*(const SemiringMatrix& rhs) const {
        matrix_power_detail::require(cols_ == rhs.rows_);
        SemiringMatrix result(rows_, rhs.cols_);
        if (rows_ == 0 || cols_ == 0 || rhs.cols_ == 0) return result;
        const auto additive_identity = zero();
        for (size_type i = 0; i < rows_; ++i) {
            const auto* lhs_row = (*this)[i];
            auto* result_row = result[i];
            for (size_type k = 0; k < cols_; ++k) {
                const auto& left = lhs_row[k];
                if (left == additive_identity) continue;
                const auto* rhs_row = rhs[k];
                for (size_type j = 0; j < rhs.cols_; ++j) {
                    result_row[j] = Ops::add(result_row[j], Ops::mul(left, rhs_row[j]));
                }
            }
        }
        return result;
    }

    std::vector<value_type> apply(const std::vector<value_type>& v) const {
        matrix_power_detail::require(v.size() == cols_);
        std::vector<value_type> result(rows_, zero());
        for (size_type i = 0; i < rows_; ++i) {
            const auto* row = (*this)[i];
            for (size_type j = 0; j < cols_; ++j) {
                result[i] = Ops::add(result[i], Ops::mul(row[j], v[j]));
            }
        }
        return result;
    }

    SemiringMatrix pow(std::uint64_t k) const {
        return matrix_power_detail::pow(*this, k);
    }

    std::vector<value_type> pow_apply(std::uint64_t k, std::vector<value_type> v) const {
        return matrix_power_detail::pow_apply(*this, k, std::move(v));
    }

    std::pair<SemiringMatrix, SemiringMatrix> pow_sum(std::uint64_t k) const {
        return matrix_power_detail::pow_sum(*this, k);
    }

private:
    size_type rows_, cols_;
    std::vector<value_type> data_;
};

using MatrixMinPlus = SemiringMatrix<matrix_power_detail::MinPlus>;
using MatrixBool = SemiringMatrix<matrix_power_detail::Boolean>;

template<class Matrix>
class MatrixPower {
public:
    using value_type = typename Matrix::value_type;
    using size_type = typename Matrix::size_type;

    MatrixPower(Matrix a, std::uint64_t max_k) : n_(a.rows()), max_k_(max_k) {
        matrix_power_detail::require(n_ == a.cols());
        const auto bits = std::bit_width(max_k);
        powers_.reserve(bits);
        if (bits == 0) return;
        powers_.push_back(std::move(a));
        for (int i = 1; i < bits; ++i) {
            powers_.push_back(powers_.back() * powers_.back());
        }
    }

    Matrix pow(std::uint64_t k) const {
        matrix_power_detail::require(k <= max_k_);
        if (k == 0) return Matrix::identity(n_);
        Matrix result = powers_[std::countr_zero(k)];
        k &= k - 1;
        for (; k != 0; k &= k - 1) result = result * powers_[std::countr_zero(k)];
        return result;
    }

    std::vector<value_type> pow_apply(std::uint64_t k, std::vector<value_type> v) const {
        matrix_power_detail::require(k <= max_k_ && v.size() == n_);
        for (; k != 0; k &= k - 1) v = powers_[std::countr_zero(k)].apply(v);
        return v;
    }

private:
    size_type n_;
    std::uint64_t max_k_;
    std::vector<Matrix> powers_;
};

/*
細かい注意点
添字は0始まり。全型で行列×列ベクトルを計算する。累乗・前計算には正方行列を渡す。
MatrixMod<Mod>は2<=Mod<=2^30、合成数可。A[i][j]とベクトルには0以上Mod未満を渡す。
符号付き整数を行列に入れるときはsetを使う。両演算の添字と要素の範囲は呼び出し側で守る。
MatrixMinPlusの未到達値はzero()。有限値の加算結果はlong longの範囲でzero()未満。
MatrixBoolの値型はuint8_t。行列・ベクトルの値は0または1。
SemiringMatrix<Ops>ではvalue_typeはbool以外。半環のzero、one、add、mulと要素の等値比較を用意する。
pow_sum(k)の和は0<=i<k。k=0では単位行列と零行列を返す。
次元不一致・前計算範囲外・検出したオーバーフローではメッセージを出さず停止する。

使える関数一覧
MatrixMod<Mod> A(h,w);    // 零行列。SemiringMatrix<Ops>、MatrixMinPlus、MatrixBoolでも同様 O(hw+1)
A.rows(), A.cols();       // size_type: 行数、列数 O(1)
A[i];                    // value_type* / const value_type*: i行目 O(1)
A.set(i,j,x);             // void: 符号付き整数xを正規化して代入。MatrixModのみ O(1)
MatrixMod<Mod>::mod();    // value_type: 法 O(1)
M::zero(), M::one();      // value_type: 加法、乗法の単位元。SemiringMatrix系のみ O(1)
M::identity(n);          // M: n次単位行列 O(n^2+1)
A+B;                     // M: 同じ形の行列の和 O(hw+1)
A*B;                     // M: h×dとd×wの積 O(hdw+hw+1)。大きなmod正方行列はStrassen
A.mul_classic(B);         // MatrixMod: 常に三重ループ相当で積を計算 O(hdw+hw+1)
A.apply(v);              // vector<value_type>: A*v O(hw+h+1)
A.pow(k);                // M: A^k O((T(n)+1)log(k+2))
A.pow_apply(k,v);         // vector<value_type>: A^k*v O(n+1+b*(T(n)+1)+popcount(k)*(n^2+1))
A.pow_sum(k);             // pair<M,M>: {A^k, I+A+...+A^(k-1)} O((T(n)+1)log(k+2))
MatrixPower powers(A,k);  // A^(2^b)を前計算。B=bit_width(k)、時間O(n^2+1+B*(T(n)+1))、保持領域O(B*(n^2+1)+1)
powers.pow(k);           // M: A^k。時間O(n^2+1+popcount(k)*(T(n)+1))
powers.pow_apply(k,v);    // vector<value_type>: A^k*v。時間O(n+1+popcount(k)*(n^2+1))
Mは使用する行列型。T(n)はn次正方行列の積1回の時間、b=max(0,bit_width(k)-1)。

基本の関数
MatrixMod<> A(2, 2);
A[0][0] = A[0][1] = A[1][0] = 1;
auto fib = A.pow_apply(10, {0, 1});
auto [power, sum] = A.pow_sum(10);
MatrixPower powers(A, 1000);
auto same = powers.pow_apply(10, {0, 1});
MatrixMinPlus costs(2, 2);
costs[0][1] = -3;
MatrixBool edges(2, 2);
edges[0][1] = 1;
if (fib[0] != 55 || same != fib || power[0][1] != 55 || sum[0][1] != 88 ||
    costs.pow(1)[0][1] != -3 || !edges.pow(1)[0][1]) std::abort();
*/
