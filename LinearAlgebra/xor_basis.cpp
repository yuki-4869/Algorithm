template<class T = unsigned long long>
struct XorBasis {
    static_assert(is_integral_v<T>);
    static_assert(is_unsigned_v<T>);

    static constexpr int B = numeric_limits<T>::digits;

    array<T, B> basis{};
    int basis_rank = 0;

    XorBasis() {
        basis.fill(0);
    }

    bool insert(T x) {
        for (int i = B - 1; i >= 0; i--) {
            if (((x >> i) & T{1}) == 0) continue;

            if (basis[i] == 0) {
                basis[i] = x;
                basis_rank++;

                return true;
            }

            x ^= basis[i];
        }

        return false;
    }

    bool contains(T x) const {
        for (int i = B - 1; i >= 0; i--) {
            if (((x >> i) & T{1}) == 0) continue;

            if (basis[i] == 0) return false;

            x ^= basis[i];
        }

        return true;
    }

    T max_xor(T x = 0) const {
        for (int i = B - 1; i >= 0; i--) {
            if (basis[i] == 0) continue;

            x = max(x, x ^ basis[i]);
        }

        return x;
    }

    T min_xor(T x = 0) const {
        for (int i = B - 1; i >= 0; i--) {
            if (basis[i] == 0) continue;

            x = min(x, x ^ basis[i]);
        }

        return x;
    }

    int rank() const {
        return basis_rank;
    }

    bool independent(T x) const {
        return !contains(x);
    }

    void merge(const XorBasis& other) {
        for (int i = B - 1; i >= 0; i--) {
            if (other.basis[i] != 0) {
                insert(other.basis[i]);
            }
        }
    }

    vector<T> vectors() const {
        vector<T> result;

        for (int i = B - 1; i >= 0; i--) {
            if (basis[i] != 0) {
                result.push_back(basis[i]);
            }
        }

        return result;
    }

    void clear() {
        basis.fill(0);
        basis_rank = 0;
    }
};

/*
細かい注意点
非負整数の部分集合XORが作る線形空間。Tは符号なし整数型。
0は常に作れる。同じ値を複数回insertしてもrankは増えない。
異なるXOR値は2^rank個。unsigned long longなら基底は最大64本。
log MAXはTのビット数に相当する。

使える関数一覧
XorBasis<unsigned long long> xb;               // 空の XOR Basis を作成

xb.insert(x);                                  // x を追加
                                                // x が既存の基底から作れないなら true
                                                // 作れるなら false
                                                // O(log MAX)

xb.contains(x);                                // x を挿入済みの値の XOR で作れるなら true
                                                // O(log MAX)

xb.independent(x);                             // x が現在の基底と線形独立なら true
                                                // O(log MAX)

xb.rank();                                     // 基底のランク O(1)
                                                // 作れる異なる XOR 値の個数は 2^rank

xb.max_xor();                                  // 部分集合 XOR の最大値 O(log MAX)
xb.max_xor(x);                                 // x XOR (部分集合 XOR) の最大値 O(log MAX)

xb.min_xor(x);                                 // x XOR (部分集合 XOR) の最小値 O(log MAX)
                                                // xb.min_xor() は常に 0

xb.merge(other);                               // other の線形空間を統合
                                                // O(log^2 MAX)

xb.vectors();                                  // 現在の基底ベクトルを取得 O(log MAX)

xb.clear();                                    // 空に戻す O(log MAX)

基本の関数
XorBasis<unsigned long long> basis;
basis.insert(3); basis.insert(5);
auto possible = basis.contains(6);           // true
auto maximum = basis.max_xor();              // 6
*/
