// 比較順序を保つ0-indexedの座標圧縮。等価性も比較器から判定する。
// Source: https://github.com/yuki-4869/Algorithm/blob/main/DataStructureBasic/coordinate_compression.cpp
#include <algorithm>
#include <functional>
#include <initializer_list>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace coordinate_zip {

template<class T, class Compare = std::less<T>>
class Zip {
    static_assert(std::is_copy_constructible_v<Compare>, "Zip: Compare must be copy constructible");
    Compare compare_;
    std::vector<T> xs_;
    bool built_ = true;

    void require_built() const {
        if (!built_) throw std::logic_error("Zip: call build() after adding values");
    }
    bool equivalent(const T& a, const T& b) const {
        return !compare_(a,b) && !compare_(b,a);
    }

public:
    using value_type = T;
    using const_reference = typename std::vector<T>::const_reference;
    using const_iterator = typename std::vector<T>::const_iterator;

    Zip() = default;
    explicit Zip(Compare compare) : compare_(std::move(compare)) {}
    explicit Zip(const std::vector<T>& a, Compare compare = Compare{})
        : compare_(std::move(compare)), xs_(a), built_(false) { build(); }
    Zip(std::initializer_list<T> a, Compare compare = Compare{})
        : Zip(std::vector<T>(a), std::move(compare)) {}
    Zip(const Zip&) = default;
    Zip& operator=(const Zip& other) {
        if (this != &other) {
            built_ = false;
            compare_ = other.compare_;
            xs_ = other.xs_;
            built_ = other.built_;
        }
        return *this;
    }
    // 比較器はコピーし、ムーブ元でも引き続き利用できるようにする。
    Zip(Zip&& other) noexcept(std::is_nothrow_copy_constructible_v<Compare>)
        : compare_(other.compare_), xs_(std::move(other.xs_)), built_(other.built_) {
        other.clear();
    }
    Zip& operator=(Zip&& other) noexcept(std::is_nothrow_copy_assignable_v<Compare>) {
        if (this != &other) {
            built_ = false;
            compare_ = other.compare_;
            xs_ = std::move(other.xs_);
            built_ = other.built_;
            other.clear();
        }
        return *this;
    }

    void add(const T& x) {
        // z.add(z[i]) のように、自身の要素を参照している場合にも対応。
        T copy = x;
        built_ = false;
        xs_.push_back(std::move(copy));
    }
    void add(const std::vector<T>& a) {
        if (a.empty()) return;
        if (&a == &xs_) {
            // vectorへの自己範囲挿入を避ける。
            const std::vector<T> copy = a;
            built_ = false;
            xs_.insert(xs_.end(),copy.begin(),copy.end());
        } else {
            built_ = false;
            xs_.insert(xs_.end(),a.begin(),a.end());
        }
    }
    void add(std::initializer_list<T> a) {
        if (a.size()==0) return;
        built_ = false;
        xs_.insert(xs_.end(),a.begin(),a.end());
    }
    void reserve(std::size_t n) { xs_.reserve(n); }
    void build() {
        if (built_) return;
        std::sort(xs_.begin(),xs_.end(),compare_);
        xs_.erase(std::unique(xs_.begin(),xs_.end(),
            [this](const T& a,const T& b){ return equivalent(a,b); }),xs_.end());
        if (xs_.size()>static_cast<std::size_t>(std::numeric_limits<int>::max()))
            throw std::length_error("Zip: too many distinct values for int indices");
        built_ = true;
    }
    void assign(const std::vector<T>& a) {
        std::vector<T> copy = a;
        built_ = false;
        xs_ = std::move(copy);
        build();
    }
    void assign(std::initializer_list<T> a) { assign(std::vector<T>(a)); }
    void clear() { xs_.clear(); built_=true; }
    bool ready() const { return built_; }
    std::size_t pending_size() const { return xs_.size(); }

    // lower/getは未登録値にも挿入位置を返す（旧APIとの互換動作）。
    int lower(const T& x) const {
        require_built();
        return static_cast<int>(std::lower_bound(xs_.begin(),xs_.end(),x,compare_)-xs_.begin());
    }
    int get(const T& x) const { return lower(x); }
    int upper(const T& x) const {
        require_built();
        return static_cast<int>(std::upper_bound(xs_.begin(),xs_.end(),x,compare_)-xs_.begin());
    }
    int index(const T& x) const {
        const int i=lower(x);
        return i<static_cast<int>(xs_.size()) && equivalent(xs_[i],x) ? i : -1;
    }
    bool contains(const T& x) const { return index(x)!=-1; }
    std::pair<int,int> equal_range(const T& x) const { return {lower(x),upper(x)}; }
    int count_range(const T& l,const T& r) const { return std::max(0,lower(r)-lower(l)); }

    const_reference at(int i) const {
        require_built();
        if (i<0 || static_cast<std::size_t>(i)>=xs_.size())
            throw std::out_of_range("Zip: compressed index out of range");
        return xs_[static_cast<std::size_t>(i)];
    }
    const_reference operator[](int i) const { return at(i); }
    const std::vector<T>& values() const { require_built(); return xs_; }
    const_iterator begin() const { require_built(); return xs_.cbegin(); }
    const_iterator end() const { require_built(); return xs_.cend(); }
    int size() const { require_built(); return static_cast<int>(xs_.size()); }
    bool empty() const { require_built(); return xs_.empty(); }

    std::vector<int> zip(const std::vector<T>& a) const {
        require_built();
        std::vector<int> result;
        result.reserve(a.size());
        for (const auto& x:a) result.push_back(lower(x));
        return result;
    }
    std::vector<int> zip_exact(const std::vector<T>& a) const {
        require_built();
        std::vector<int> result;
        result.reserve(a.size());
        for (const auto& x:a) {
            const int i=index(x);
            if (i==-1) throw std::out_of_range("Zip: value is not registered");
            result.push_back(i);
        }
        return result;
    }
    std::vector<T> unzip(const std::vector<int>& ids) const {
        require_built();
        std::vector<T> result;
        result.reserve(ids.size());
        for (int i:ids) result.push_back(at(i));
        return result;
    }
};

template<class T,class Compare=std::less<T>>
std::vector<int> zip(const std::vector<T>& a,Compare compare=Compare{}) {
    return Zip<T,Compare>(a,std::move(compare)).zip_exact(a);
}

template<class X,class Y,class CompareX=std::less<X>,class CompareY=std::less<Y>>
std::vector<std::pair<int,int>> zip2d(const std::vector<std::pair<X,Y>>& a,
                                    CompareX cx=CompareX{},CompareY cy=CompareY{}) {
    std::vector<X> x;
    std::vector<Y> y;
    x.reserve(a.size()); y.reserve(a.size());
    for (const auto& [p,q]:a) { x.push_back(p); y.push_back(q); }
    const Zip<X,CompareX> zx(x,std::move(cx));
    const Zip<Y,CompareY> zy(y,std::move(cy));
    std::vector<std::pair<int,int>> result;
    result.reserve(a.size());
    for (const auto& [p,q]:a) result.emplace_back(zx.index(p),zy.index(q));
    return result;
}

} // namespace coordinate_zip

// 名前空間の定義後に置く。Zip<long long> のように利用できる。
using coordinate_zip::Zip;

/*
座標圧縮 公開API一覧（0-indexed）

T = 値の型、Compare = 比較器（省略時はstd::less<T>）。
N = 登録配列の長さ（構築前は重複を含む）、U = 異なる値の個数。
M = 引数の配列の長さ、N2 = otherの登録配列の長さ。otherはzと同じ型。
比較・値と比較器のコピー・移動をO(1)とした計算量。等価性は!cmp(a,b)&&!cmp(b,a)。
const_referenceは通常const T&（T=boolではbool）、const_iteratorは読み取り専用。

// 構築・コピー・ムーブ（コンストラクタには返り値はない）
Zip<T> z;                                     // 構築済みの空の圧縮表を作る O(1)
Zip<T> z(A);                                  // Aをソート・重複除去して構築 O(M log(M+1))
Zip<T> z{a,b,c};                              // 初期化子リストから構築 O(M log(M+1))
Zip<T,Compare> z(cmp);                        // 比較器を指定し、構築済みの空の表を作る O(1)
Zip<T,Compare> z(A,cmp);                      // 比較器を指定してAから構築 O(M log(M+1))
Zip<T,Compare> z({a,b,c},cmp);                // 比較器付き初期化子リストから構築 O(M log(M+1))
Zip<T> z(other);                              // 独立なコピーを作る O(N2)
Zip<T> z(std::move(other));                   // 登録値を移し、otherを構築済みの空表にする O(1)
z = other;                                    // Zip&: zへの参照。独立なコピーで上書き O(N+N2)
z = std::move(other);                         // Zip&: zへの参照。登録値を移し、otherを空にする O(N+1)

// 値の登録・構築・状態管理
z.add(x);                                     // void: xを追加し、未構築状態にする ならしO(1)
z.add(A);                                     // void: Aの全値を追加し、未構築状態にする ならしO(M)
z.add({a,b,c});                               // void: 初期化子リストの全値を追加 ならしO(M)
z.reserve(n);                                 // void: n個分の容量を確保（構築状態は変わらない） O(N)以下
z.build();                                    // void: ソート・重複除去して構築 O(N log(N+1))、構築済みならO(1)
z.assign(A);                                  // void: 全値をAに置き換え、構築まで行う O(N+M log(M+1))
z.assign({a,b,c});                            // void: 初期化子リストに置き換えて構築 O(N+M log(M+1))
z.clear();                                    // void: 全値を削除し、構築済みの空表にする O(N)
z.ready();                                    // bool: 構築済みならtrue O(1)
z.pending_size();                             // size_t: 現在の登録配列の長さ（未構築なら重複を含む） O(1)

// 値から番号・境界を取得（構築済みで使用）
z.get(x);                                     // int: lower(x)と同じ。未登録値にも挿入位置を返す O(log(U+1))
z.lower(x);                                   // int: 比較順序でxより前の値の種類数 [0,U] O(log(U+1))
z.upper(x);                                   // int: xより前または同値の種類数 [0,U] O(log(U+1))
z.index(x);                                   // int: 登録済みなら圧縮番号、未登録なら-1 O(log(U+1))
z.contains(x);                                // bool: xと比較器で同値の登録値があればtrue O(log(U+1))
z.equal_range(x);                             // pair<int,int>: {lower(x),upper(x)}を返す O(log(U+1))
z.count_range(l,r);                           // int: 比較順で[l,r)に入る登録値の種類数 O(log(U+1))

// 番号から値・圧縮表を取得（構築済みで使用）
z[i];                                       // const_reference: 番号iの元の値 O(1)、範囲外はout_of_range
z.at(i);                                      // const_reference: z[i]と同じ O(1)
z.values();                                   // const vector<T>&: 比較順の重複なし圧縮表を返す O(1)
z.begin();                                    // const_iterator: 最初の値を指す O(1)
z.end();                                      // const_iterator: 最後の値の次を指す O(1)
for (const auto& x : z) { ... }               // 読み取り専用で圧縮表を比較順に走査 O(U)
z.size();                                     // int: 異なる値の個数Uを返す O(1)
z.empty();                                    // bool: 空の圧縮表ならtrue O(1)

// 配列の圧縮・復元（構築済みで使用）
z.zip(A);                                     // vector<int>: 各値のlower位置を返す（未登録値も許可） O(M log(U+1))
z.zip_exact(A);                               // vector<int>: 各登録値の番号。未登録値があればout_of_range O(M log(U+1))
z.unzip(ids);                                 // vector<T>: 番号列を代表値へ復元、範囲外はout_of_range O(M)

// 表の構築も行う一括関数（追加メモリO(M)）
coordinate_zip::zip(A);                       // vector<int>: Aを0-indexedに圧縮 O(M log(M+1))
coordinate_zip::zip(A,cmp);                   // vector<int>: 比較器を指定してAを圧縮 O(M log(M+1))
coordinate_zip::zip2d(P);                     // vector<pair<int,int>>: x,yを独立に圧縮 O(M log(M+1))
coordinate_zip::zip2d(P,cx,cy);               // vector<pair<int,int>>: 各軸の比較器を指定して圧縮 O(M log(M+1))

// 利用上の注意
// add後はbuildが必要。未構築の検索・逆引き・size・列挙・変換はlogic_error（NDEBUGでも検出）。
// 空の配列・リストのaddは何も変えない。再buildすると既存の圧縮番号も変わり得る。
// get/zipは存在判定ではない。厳密な番号取得にはindex/zip_exactを使う。
// Compareはstrict weak orderingを満たし、コピー可能であること。代入には比較器のコピー代入も必要。
// 比較器で同値なら1種類に統合され、元の個別値ではなく代表値に復元される。
// pair/tuple/stringも使用可能。Zip<pair<X,Y>>はpair全体、zip2dは各軸を独立に圧縮する。
// 圧縮表・フラグは非公開。xsの代わりにvalues()、builtの代わりにready()を使う。
// 保持した値の参照・イテレータは追加・構築・置換・clear・reserve等で無効になる場合がある。
// UはINT_MAX以下。超過時のbuildはlength_error。変換結果はO(M)、保持メモリはvectorのcapacityに比例。
// デストラクタは自動実行され、登録値の解放はO(N)。
*/
