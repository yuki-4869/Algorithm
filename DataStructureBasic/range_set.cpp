// 整数集合を互いに離れた最大の半開区間 [l,r) で管理する。
// Source: https://github.com/yuki-4869/Algorithm/blob/main/DataStructureBasic/range_set.cpp
#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template<class T = long long, class Count = T>
class RangeSet {
    static_assert(std::is_integral_v<T> && std::is_signed_v<T> && sizeof(T) <= 8,
                  "RangeSet: T must be a signed integer of at most 64 bits");
    static_assert((std::is_integral_v<Count> && !std::is_same_v<Count, bool> && sizeof(Count) <= 8)
                  || std::is_same_v<Count, __int128_t>,
                  "RangeSet: Count must be an integer of at most 64 bits, or __int128_t");

public:
    using value_type = T;
    using count_type = Count;
    using ptt = std::pair<T, T>;
    using const_iterator = typename std::map<T, T>::const_iterator;
    // covered_by() の従来の「見つからない」返り値だけに使用。内部に番兵は置かない。
    static constexpr T TINF = std::numeric_limits<T>::max() / 2;

private:
    using wide_type = __int128_t;
    std::map<T, T> ranges_;
    Count total_ = 0;
    static wide_type length(T l, T r) {
        return static_cast<wide_type>(r) - static_cast<wide_type>(l);
    }
    static Count checked_count(wide_type value) {
        if (value > static_cast<wide_type>(std::numeric_limits<Count>::max()))
            throw std::overflow_error("RangeSet: covered length exceeds Count");
        return static_cast<Count>(value);
    }

public:
    RangeSet() = default;
    RangeSet(std::initializer_list<ptt> ranges) {
        for (auto [l, r] : ranges) insert(l, r);
    }
    RangeSet(const RangeSet&) = default;
    RangeSet& operator=(const RangeSet&) = default;
    RangeSet(RangeSet&& other) noexcept
        : ranges_(std::move(other.ranges_)), total_(std::exchange(other.total_, 0)) {
        other.ranges_.clear();
    }
    RangeSet& operator=(RangeSet&& other) noexcept {
        if (this != &other) {
            ranges_ = std::move(other.ranges_);
            total_ = std::exchange(other.total_, 0);
            other.ranges_.clear();
        }
        return *this;
    }

    // xを含む保持区間。存在しなければnullopt。
    std::optional<ptt> covering_interval(T x) const {
        auto it = ranges_.upper_bound(x);
        if (it == ranges_.begin()) return std::nullopt;
        --it;
        if (x < it->second) return ptt{it->first, it->second};
        return std::nullopt;
    }
    // [l,r)全体を含む保持区間。l>=rは空区間として{l,l}を返す。
    std::optional<ptt> covering_interval(T l, T r) const {
        if (l >= r) return ptt{l, l};
        auto interval = covering_interval(l);
        if (interval && r <= interval->second) return interval;
        return std::nullopt;
    }
    bool covered(T l, T r) const { return covering_interval(l, r).has_value(); }
    bool covered(T x) const { return covering_interval(x).has_value(); }
    bool contains(T x) const { return covered(x); }
    // 互換API。空区間なら{0,0}、未被覆なら{-TINF,-TINF}。
    ptt covered_by(T l, T r) const {
        if (l >= r) return {0, 0};
        return covering_interval(l, r).value_or(ptt{-TINF, -TINF});
    }
    bool overlaps(T l, T r) const {
        if (l >= r) return false;
        auto it = ranges_.lower_bound(l);
        if (it != ranges_.end() && it->first < r) return true;
        return it != ranges_.begin() && std::prev(it)->second > l;
    }

    // [l,r)を追加。重なり・左右の隣接をすべて統合し、新規追加数を返す。
    Count insert(T l, T r) {
        if (l >= r) return 0;
        auto first = ranges_.lower_bound(l);
        if (first != ranges_.begin() && std::prev(first)->second >= l) --first;
        auto last = first;
        T merged_l = l, merged_r = r;
        wide_type old_length = 0;
        while (last != ranges_.end() && last->first <= merged_r) {
            merged_l = std::min(merged_l, last->first);
            merged_r = std::max(merged_r, last->second);
            old_length += length(last->first, last->second);
            ++last;
        }
        const wide_type added = length(merged_l, merged_r) - old_length;
        const Count next_total = checked_count(static_cast<wide_type>(total_) + added);
        if (first == last) {
            ranges_.emplace_hint(last, merged_l, merged_r);
        } else {
            // 既存ノードを再利用し、mapのconstキーへの代入を避ける。
            auto rest = std::next(first);
            auto node = ranges_.extract(first);
            ranges_.erase(rest, last);
            node.key() = merged_l;
            node.mapped() = merged_r;
            ranges_.insert(last, std::move(node));
        }
        total_ = next_total;
        return static_cast<Count>(added);
    }
    Count insert(T x) {
        if (x == std::numeric_limits<T>::max())
            throw std::out_of_range("RangeSet: x+1 is not representable");
        return insert(x, static_cast<T>(x + 1));
    }

    // [l,r)を削除。左右に残る断片を保持し、実際の削除数を返す。
    Count erase(T l, T r) {
        if (l >= r) return 0;
        auto it = ranges_.lower_bound(l);
        if (it != ranges_.begin() && std::prev(it)->second > l) --it;
        wide_type removed = 0;
        while (it != ranges_.end() && it->first < r) {
            const T a = it->first, b = it->second;
            removed += length(std::max(a, l), std::min(b, r));
            if (a < l) {
                if (r < b) {
                    // 分割に必要なノードを先に確保。元ノードは左側として残す。
                    ranges_.emplace(r, b);
                    it->second = l;
                    break;
                }
                it->second = l;
                ++it;
            } else if (b <= r) {
                it = ranges_.erase(it);
            } else {
                auto next = std::next(it);
                auto node = ranges_.extract(it);
                node.key() = r;
                ranges_.insert(next, std::move(node));
                break;
            }
        }
        total_ = static_cast<Count>(static_cast<wide_type>(total_) - removed);
        return static_cast<Count>(removed);
    }
    Count erase(T x) {
        if (x == std::numeric_limits<T>::max()) return 0;
        return erase(x, static_cast<T>(x + 1));
    }

    // x以上で、集合に含まれない最小の整数。
    T mex(T x = 0) const {
        auto interval = covering_interval(x);
        return interval ? interval->second : x;
    }
    Count count_covered(T l, T r) const {
        if (l >= r) return 0;
        auto it = ranges_.lower_bound(l);
        if (it != ranges_.begin() && std::prev(it)->second > l) --it;
        wide_type count = 0;
        while (it != ranges_.end() && it->first < r) {
            count += length(std::max(l, it->first), std::min(r, it->second));
            ++it;
        }
        return static_cast<Count>(count);
    }

    Count cover_length() const { return total_; }
    std::size_t size() const { return ranges_.size(); }  // 保持する区間数
    bool empty() const { return ranges_.empty(); }
    void clear() { ranges_.clear(); total_ = 0; }
    void swap(RangeSet& other) noexcept {
        ranges_.swap(other.ranges_);
        std::swap(total_, other.total_);
    }
    std::vector<ptt> intervals() const { return {ranges_.begin(), ranges_.end()}; }
    const_iterator begin() const { return ranges_.cbegin(); }
    const_iterator end() const { return ranges_.cend(); }
    void output() const {
        std::cout << "RangeSet: ";
        for (auto [l, r] : ranges_) std::cout << '[' << +l << ',' << +r << ") ";
        std::cout << '\n';
    }
};

/*
RangeSet 公開API一覧

任意の整数座標を扱う。区間は半開区間 [l,r)（lを含み、rを含まない）。
T = 座標の型、Count = 個数の型。省略時はどちらもlong long。
ptt = pair<T,T>、const_iterator = 読み取り専用の区間イテレータ。
K = rsの保持区間数、K2 = otherの保持区間数、M = 初期区間数。
D = 操作で統合・削除・走査する区間数。計算量のK,K2は操作前の値。

// 構築・コピー・ムーブ（コンストラクタには返り値はない）
RangeSet<T, Count> rs;                        // 空集合で初期化 O(1)
RangeSet<T, Count> rs{{a,b},{c,d}};           // M個の区間から構築、重複・隣接を統合 O(M log(M+1))
RangeSet<T, Count> rs(other);                 // 独立なコピーを構築 O(K2)
RangeSet<T, Count> rs(std::move(other));      // 集合を移し、otherを空にする O(1)
rs = other;                                   // RangeSet&: rsへの参照。独立なコピーで上書き O(K+K2)
rs = std::move(other);                        // RangeSet&: rsへの参照。集合を移し、otherを空にする O(K+1)

// 追加・削除
rs.insert(l,r);                               // Count: [l,r)を追加し、新たに被覆した個数を返す O(log(K+1)+D)
rs.insert(x);                                 // Count: 1点xを追加し、新たに被覆した個数（0か1）を返す O(log(K+1))
rs.erase(l,r);                                // Count: [l,r)を削除し、実際に削除した個数を返す O(log(K+1)+D)
rs.erase(x);                                  // Count: 1点xを削除し、実際に削除した個数（0か1）を返す O(log(K+1))

// 被覆・重なりの判定
rs.covered(l,r);                              // bool: [l,r)全体が被覆されていればtrue O(log(K+1))
rs.covered(x);                                // bool: 1点xが被覆されていればtrue O(log(K+1))
rs.contains(x);                               // bool: covered(x)と同じ O(log(K+1))
rs.overlaps(l,r);                             // bool: [l,r)に被覆された整数が1つ以上あればtrue O(log(K+1))

// 保持区間の取得
rs.covering_interval(x);                      // optional<ptt>: xを含む保持区間、なければnullopt O(log(K+1))
rs.covering_interval(l,r);                    // optional<ptt>: [l,r)全体を含む保持区間、なければnullopt O(log(K+1))
rs.covered_by(l,r);                           // ptt: 包含する保持区間、なければ{-TINF,-TINF} O(log(K+1))

// mex・個数・状態
rs.mex(x);                                    // T: x以上の未被覆の整数のうち最小のものを返す O(log(K+1))
rs.mex();                                     // T: 0以上の未被覆の整数のうち最小のものを返す O(log(K+1))
rs.count_covered(l,r);                        // Count: [l,r)内で被覆された整数の個数を返す O(log(K+1)+D)、最悪O(K)
rs.cover_length();                            // Count: 集合全体で被覆された整数の個数を返す O(1)
rs.size();                                    // size_t: 保持する区間数を返す（整数の個数ではない） O(1)
rs.empty();                                   // bool: 空集合ならtrue O(1)
rs.clear();                                   // void: 全区間を削除し、総被覆数も0にする O(K)
rs.swap(other);                               // void: otherと区間集合・総被覆数を交換 O(1)

// 区間の列挙・表示
rs.intervals();                               // vector<ptt>: 左端の昇順で全保持区間のコピーを返す O(K)、追加メモリO(K)
rs.begin();                                   // const_iterator: 最初の区間を指すイテレータを返す O(1)
rs.end();                                     // const_iterator: 最後の区間の次を指すイテレータを返す O(1)
for (auto [l,r] : rs) { ... }                 // 読み取り専用で全区間を左端の昇順に走査 O(K)
rs.output();                                  // void: "RangeSet: [l,r) ..."形式で全区間を標準出力 O(K)

// 空区間（l>=r）の扱い
// insert/erase/count_coveredは0、coveredはtrue、overlapsはfalseを返す O(1)。
// covering_interval(l,r)はoptionalに{l,l}を格納、covered_by(l,r)は{0,0}を返す O(1)。
// emptyならbegin()==end()。デストラクタによる全区間の解放はO(K)。

// 型・例外・利用上の注意
// Tは64bit以下の符号付き整数型。Countは64bit以下の整数型（bool以外）または__int128_t。
// 大きな被覆数には RangeSet<long long, __int128_t> rs; を使用できる。
// 総被覆数がCountの最大値を超える追加はoverflow_error。集合は変わらない。
// Tの最大値は格納できない。insert(最大値)はout_of_range、erase(最大値)は0を返す。
// 区間の走査中に集合を更新しないこと。返されたvectorの変更は元の集合に影響しない。
// TINFはcovered_byの互換返り値用の定数で、内部の番兵や座標制限には使わない。
*/
