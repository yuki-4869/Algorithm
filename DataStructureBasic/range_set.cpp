template<class T = long long, class Count = T>
class RangeSet {
    static_assert(std::is_integral_v<T> && std::is_signed_v<T> && sizeof(T) <= 8);
    static_assert((std::is_integral_v<Count> && !std::is_same_v<Count, bool> && sizeof(Count) <= 8)
                  || std::is_same_v<Count, __int128_t>);

public:
    using value_type = T;
    using count_type = Count;
    using ptt = std::pair<T, T>;
    using const_iterator = typename std::map<T, T>::const_iterator;

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
            std::abort();
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

    std::optional<ptt> covering_interval(T x) const {
        auto it = ranges_.upper_bound(x);
        if (it == ranges_.begin()) return std::nullopt;
        --it;
        if (x < it->second) return ptt{it->first, it->second};
        return std::nullopt;
    }

    std::optional<ptt> covering_interval(T l, T r) const {
        if (l >= r) return ptt{l, l};
        auto interval = covering_interval(l);
        if (interval && r <= interval->second) return interval;
        return std::nullopt;
    }
    bool covered(T l, T r) const { return covering_interval(l, r).has_value(); }
    bool covered(T x) const { return covering_interval(x).has_value(); }
    bool contains(T x) const { return covered(x); }

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
            std::abort();
        return insert(x, static_cast<T>(x + 1));
    }

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
    std::size_t size() const { return ranges_.size(); }
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
細かい注意点
整数座標の集合。区間は半開区間[l,r)。Tは座標、Countは個数の型（既定long long）。
ptt=pair<T,T>。K/K2はrs/otherの操作前の区間数、Mは初期区間数、Dは走査・統合・削除する区間数。
Tは64bit以下の符号付き整数。Countはbool以外の64bit以下の整数または__int128_t。
Tの最大値は格納不可。insert(最大値)や総被覆数のCount超過はメッセージなしで停止。
erase(最大値)は0。大きな被覆数にはRangeSet<ll,__int128_t>を使う。
l>=rではinsert/erase/count_coveredは0、coveredはtrue、overlapsはfalse。
空区間のcovering_interval(l,r)は{l,l}、covered_by(l,r)は{0,0}。
size()は保持区間数、cover_length()は被覆された整数の個数。
列挙は左端昇順の読み取り専用。走査中に集合を更新しない。intervals()は独立なコピー。
TINFはcovered_byの互換返り値用。座標制限や内部番兵ではない。
空ならbegin()==end()。解放O(K)。output()は明示呼び出し時だけ標準出力する。

使える関数一覧
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

基本の関数
RangeSet<ll> rs;                              // 空集合
rs.insert(1,10);                              // [1,10)を追加
rs.erase(3,7);                                // [3,7)を削除
auto covered = rs.covered(1,3);               // true
auto mex = rs.mex(1);                         // 3
auto count = rs.cover_length();               // 5
*/
