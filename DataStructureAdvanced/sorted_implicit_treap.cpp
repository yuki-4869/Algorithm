struct SortedImplicitTreap {
    struct Node {
        ll val, sum, lazy_add;
        uint64_t pri;
        int sz;
        Node *l, *r;

        Node(ll v, uint64_t p)
            : val(v), sum(v), lazy_add(0),
              pri(p), sz(1), l(nullptr), r(nullptr) {}
    };

    Node* root = nullptr;

    static uint64_t rng() {
        static uint64_t x = 0x123456789abcdefULL;
        x += 0x9e3779b97f4a7c15ULL;
        uint64_t z = x;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        return z ^ (z >> 31);
    }

    static int sz(Node* t) {
        return t ? t->sz : 0;
    }

    static ll sm(Node* t) {
        return t ? t->sum : 0;
    }

    static void apply_add(Node* t, ll x) {
        if (!t) return;
        t->val += x;
        t->sum += x * (ll)t->sz;
        t->lazy_add += x;
    }

    static void push(Node* t) {
        if (!t || t->lazy_add == 0) return;
        apply_add(t->l, t->lazy_add);
        apply_add(t->r, t->lazy_add);
        t->lazy_add = 0;
    }

    static void update(Node* t) {
        if (!t) return;
        t->sz = 1 + sz(t->l) + sz(t->r);
        t->sum = t->val + sm(t->l) + sm(t->r);
    }

    static Node* merge_node(Node* a, Node* b) {
        if (!a) return b;
        if (!b) return a;

        if (a->pri > b->pri) {
            push(a);
            a->r = merge_node(a->r, b);
            update(a);
            return a;
        } else {
            push(b);
            b->l = merge_node(a, b->l);
            update(b);
            return b;
        }
    }

    static pair<Node*,Node*> split_lt_node(Node* t, ll x) {
        if (!t) return {nullptr, nullptr};

        push(t);

        if (t->val < x) {
            auto [a,b] = split_lt_node(t->r, x);
            t->r = a;
            update(t);
            return {t,b};
        } else {
            auto [a,b] = split_lt_node(t->l, x);
            t->l = b;
            update(t);
            return {a,t};
        }
    }

    static pair<Node*,Node*> split_le_node(Node* t, ll x) {
        if (!t) return {nullptr, nullptr};

        push(t);

        if (t->val <= x) {
            auto [a,b] = split_le_node(t->r, x);
            t->r = a;
            update(t);
            return {t,b};
        } else {
            auto [a,b] = split_le_node(t->l, x);
            t->l = b;
            update(t);
            return {a,t};
        }
    }

    static pair<Node*,Node*> split_size_node(Node* t, int k) {
        if (!t) return {nullptr, nullptr};

        push(t);

        if (sz(t->l) >= k) {
            auto [a,b] = split_size_node(t->l, k);
            t->l = b;
            update(t);
            return {a,t};
        } else {
            auto [a,b] = split_size_node(t->r, k - sz(t->l) - 1);
            t->r = a;
            update(t);
            return {t,b};
        }
    }

    static void destroy(Node* t) {
        if (!t) return;

        vector<Node*> st = {t};

        while (!st.empty()) {
            Node* v = st.back();
            st.pop_back();

            if (v->l) st.push_back(v->l);
            if (v->r) st.push_back(v->r);

            delete v;
        }
    }

    static ll kth_node(Node* t, int k) {
        while (true) {
            push(t);

            int ls = sz(t->l);

            if (k < ls) {
                t = t->l;
            } else if (k == ls) {
                return t->val;
            } else {
                k -= ls + 1;
                t = t->r;
            }
        }
    }

    static ll prefix_sum_node(Node* t, int k) {
        if (!t || k <= 0) return 0;
        if (k >= sz(t)) return sm(t);

        push(t);

        int ls = sz(t->l);

        if (k <= ls) {
            return prefix_sum_node(t->l, k);
        }

        return sm(t->l)
             + t->val
             + prefix_sum_node(t->r, k - ls - 1);
    }

    static int lower_bound_node(Node* t, ll x) {
        int passed = 0;
        int ans = sz(t);

        while (t) {
            push(t);

            if (t->val >= x) {
                ans = passed + sz(t->l);
                t = t->l;
            } else {
                passed += sz(t->l) + 1;
                t = t->r;
            }
        }

        return ans;
    }

    static int upper_bound_node(Node* t, ll x) {
        int passed = 0;
        int ans = sz(t);

        while (t) {
            push(t);

            if (t->val > x) {
                ans = passed + sz(t->l);
                t = t->l;
            } else {
                passed += sz(t->l) + 1;
                t = t->r;
            }
        }

        return ans;
    }

    static void dfs(Node* t, vector<ll>& res) {
        if (!t) return;

        push(t);

        dfs(t->l, res);
        res.push_back(t->val);
        dfs(t->r, res);
    }

    SortedImplicitTreap() = default;

    ~SortedImplicitTreap() {
        clear();
    }

    SortedImplicitTreap(const SortedImplicitTreap&) = delete;

    SortedImplicitTreap& operator=(const SortedImplicitTreap&) = delete;

    SortedImplicitTreap(SortedImplicitTreap&& o) noexcept
        : root(o.root) {
        o.root = nullptr;
    }

    SortedImplicitTreap& operator=(SortedImplicitTreap&& o) noexcept {
        if (this != &o) {
            clear();
            root = o.root;
            o.root = nullptr;
        }
        return *this;
    }

    int size() const {
        return sz(root);
    }

    bool empty() const {
        return root == nullptr;
    }

    ll total_sum() const {
        return sm(root);
    }

    void clear() {
        destroy(root);
        root = nullptr;
    }

    void insert(ll x) {
        auto [a,b] = split_lt_node(root, x);

        root = merge_node(
            merge_node(
                a,
                new Node(x, rng())
            ),
            b
        );
    }

    bool erase(ll x) {
        auto [a,bc] = split_lt_node(root, x);
        auto [b,c] = split_le_node(bc, x);

        if (!b) {
            root = merge_node(a,c);
            return false;
        }

        auto [del,rest] = split_size_node(b, 1);

        destroy(del);

        root = merge_node(
            a,
            merge_node(rest,c)
        );

        return true;
    }

    int erase_all(ll x) {
        auto [a,bc] = split_lt_node(root, x);
        auto [b,c] = split_le_node(bc, x);

        int ret = sz(b);

        destroy(b);

        root = merge_node(a,c);

        return ret;
    }

    ll erase_kth(int k) {
        do { if (!(0 <= k && k < size())) std::abort(); } while (false);

        auto [a,bc] = split_size_node(root, k);
        auto [b,c] = split_size_node(bc, 1);

        ll ret = b->val;

        destroy(b);

        root = merge_node(a,c);

        return ret;
    }

    void erase_at(int k) {
        (void)erase_kth(k);
    }

    int erase_index_range(int l, int r) {
        do { if (!(0 <= l && l <= r && r <= size())) std::abort(); } while (false);

        auto [a,bc] = split_size_node(root, l);
        auto [b,c] = split_size_node(bc, r-l);

        int ret = sz(b);

        destroy(b);

        root = merge_node(a,c);

        return ret;
    }

    int erase_value_range(ll L, ll R) {
        if (L > R) return 0;

        auto [a,bc] = split_lt_node(root, L);
        auto [b,c] = split_le_node(bc, R);

        int ret = sz(b);

        destroy(b);

        root = merge_node(a,c);

        return ret;
    }

    int erase_less(ll x) {
        auto [a,b] = split_lt_node(root,x);

        int ret = sz(a);

        destroy(a);
        root = b;

        return ret;
    }

    int erase_leq(ll x) {
        auto [a,b] = split_le_node(root,x);

        int ret = sz(a);

        destroy(a);
        root = b;

        return ret;
    }

    int erase_geq(ll x) {
        auto [a,b] = split_lt_node(root,x);

        int ret = sz(b);

        destroy(b);
        root = a;

        return ret;
    }

    int erase_greater(ll x) {
        auto [a,b] = split_le_node(root,x);

        int ret = sz(b);

        destroy(b);
        root = a;

        return ret;
    }

    ll get(int k) {
        do { if (!(0 <= k && k < size())) std::abort(); } while (false);
        return kth_node(root,k);
    }

    ll kth(int k) {
        return get(k);
    }

    ll front() {
        do { if (!(!empty())) std::abort(); } while (false);
        return get(0);
    }

    ll back() {
        do { if (!(!empty())) std::abort(); } while (false);
        return get(size()-1);
    }

    void set_val(int k, ll x) {
        erase_kth(k);
        insert(x);
    }

    int lower_bound(ll x) {
        return lower_bound_node(root,x);
    }

    int upper_bound(ll x) {
        return upper_bound_node(root,x);
    }

    int order_of_key(ll x) {
        return lower_bound(x);
    }

    int count_less(ll x) {
        return lower_bound(x);
    }

    int count_leq(ll x) {
        return upper_bound(x);
    }

    int count_geq(ll x) {
        return size() - lower_bound(x);
    }

    int count_greater(ll x) {
        return size() - upper_bound(x);
    }

    int count(ll x) {
        return upper_bound(x) - lower_bound(x);
    }

    bool contains(ll x) {
        return lower_bound(x) < upper_bound(x);
    }

    int first_index(ll x) {
        int p = lower_bound(x);

        if (p < size() && get(p) == x) {
            return p;
        }

        return -1;
    }

    int last_index(ll x) {
        int p = upper_bound(x) - 1;

        if (p >= 0 && get(p) == x) {
            return p;
        }

        return -1;
    }

    ll min_value() {
        do { if (!(!empty())) std::abort(); } while (false);
        return get(0);
    }

    ll max_value() {
        do { if (!(!empty())) std::abort(); } while (false);
        return get(size()-1);
    }

    ll pop_min() {
        do { if (!(!empty())) std::abort(); } while (false);
        return erase_kth(0);
    }

    ll pop_max() {
        do { if (!(!empty())) std::abort(); } while (false);
        return erase_kth(size()-1);
    }

    optional<ll> predecessor(ll x) {
        int p = lower_bound(x);

        if (p == 0) {
            return nullopt;
        }

        return get(p-1);
    }

    optional<ll> successor(ll x) {
        int p = upper_bound(x);

        if (p == size()) {
            return nullopt;
        }

        return get(p);
    }

    optional<ll> floor_value(ll x) {
        int p = upper_bound(x);

        if (p == 0) {
            return nullopt;
        }

        return get(p-1);
    }

    optional<ll> ceil_value(ll x) {
        int p = lower_bound(x);

        if (p == size()) {
            return nullopt;
        }

        return get(p);
    }

    ll median_lower() {
        do { if (!(!empty())) std::abort(); } while (false);
        return get((size()-1)/2);
    }

    ll median_upper() {
        do { if (!(!empty())) std::abort(); } while (false);
        return get(size()/2);
    }

    pair<ll,ll> median_pair() {
        do { if (!(!empty())) std::abort(); } while (false);

        return {
            median_lower(),
            median_upper()
        };
    }

    ll prefix_sum(int k) {
        do { if (!(0 <= k && k <= size())) std::abort(); } while (false);

        return prefix_sum_node(root,k);
    }

    ll range_sum(int l, int r) {
        do { if (!(0 <= l && l <= r && r <= size())) std::abort(); } while (false);

        return prefix_sum(r) - prefix_sum(l);
    }

    ll sum_smallest(int k) {
        do { if (!(0 <= k && k <= size())) std::abort(); } while (false);

        return prefix_sum(k);
    }

    ll sum_largest(int k) {
        do { if (!(0 <= k && k <= size())) std::abort(); } while (false);

        return total_sum() - prefix_sum(size()-k);
    }

    ll sum_less(ll x) {
        return prefix_sum(lower_bound(x));
    }

    ll sum_leq(ll x) {
        return prefix_sum(upper_bound(x));
    }

    ll sum_geq(ll x) {
        return total_sum() - sum_less(x);
    }

    ll sum_greater(ll x) {
        return total_sum() - sum_leq(x);
    }

    ll sum_value_range(ll L, ll R) {
        if (L > R) return 0;

        return sum_leq(R) - sum_less(L);
    }

    int count_value_range(ll L, ll R) {
        if (L > R) return 0;

        return upper_bound(R) - lower_bound(L);
    }

    void add_all(ll x) {
        apply_add(root,x);
    }

    pair<SortedImplicitTreap,SortedImplicitTreap> split_at(int k) {
        do { if (!(0 <= k && k <= size())) std::abort(); } while (false);

        auto [a,b] = split_size_node(root,k);

        root = nullptr;

        SortedImplicitTreap A,B;

        A.root = a;
        B.root = b;

        return {
            move(A),
            move(B)
        };
    }

    pair<SortedImplicitTreap,SortedImplicitTreap> split_less(ll x) {
        auto [a,b] = split_lt_node(root,x);

        root = nullptr;

        SortedImplicitTreap A,B;

        A.root = a;
        B.root = b;

        return {
            move(A),
            move(B)
        };
    }

    pair<SortedImplicitTreap,SortedImplicitTreap> split_leq(ll x) {
        auto [a,b] = split_le_node(root,x);

        root = nullptr;

        SortedImplicitTreap A,B;

        A.root = a;
        B.root = b;

        return {
            move(A),
            move(B)
        };
    }

    SortedImplicitTreap extract_range(int l, int r) {
        do { if (!(0 <= l && l <= r && r <= size())) std::abort(); } while (false);

        auto [a,bc] = split_size_node(root,l);
        auto [b,c] = split_size_node(bc,r-l);

        root = merge_node(a,c);

        SortedImplicitTreap ret;
        ret.root = b;

        return ret;
    }

    SortedImplicitTreap extract_value_range(ll L, ll R) {
        SortedImplicitTreap ret;

        if (L > R) {
            return ret;
        }

        auto [a,bc] = split_lt_node(root,L);
        auto [b,c] = split_le_node(bc,R);

        root = merge_node(a,c);

        ret.root = b;

        return ret;
    }

    void merge_with(SortedImplicitTreap&& other) {
        if (!empty() && !other.empty()) {
            do { if (!(max_value() <= other.min_value())) std::abort(); } while (false);
        }

        root = merge_node(
            root,
            other.root
        );

        other.root = nullptr;
    }

    vector<ll> to_vector() {
        vector<ll> res;
        res.reserve(size());

        dfs(root,res);

        return res;
    }

    void build(vector<ll> v) {
        clear();

        sort(v.begin(),v.end());

        for (ll x : v) {
            root = merge_node(
                root,
                new Node(x,rng())
            );
        }
    }

    void build_sorted(const vector<ll>& v) {
        clear();

        for (ll x : v) {
            root = merge_node(
                root,
                new Node(x,rng())
            );
        }
    }
};

/*
細かい注意点
重複を許す昇順の多重集合。添字は0-indexed、添字区間は[l,r)、値区間は[L,R]。
get/erase_kthは0<=k<size()。front/back/pop/medianは非空で使用する。
build_sortedは昇順配列、merge_withは自分の最大値<=otherの最小値で使用する。
split/extract/mergeはノードの所有権を移す。split後の元の木やmerge後のotherは空。
値・和・遅延加算はlong longに収まること。計算量は乱択による期待値。

使える関数一覧
SortedImplicitTreap tr;                         // 空の昇順 multiset を作成
tr.build(v);                                   // v をソートして構築 期待 O(N log N)
tr.build_sorted(v);                            // 昇順 v から構築 期待 O(N log N)

tr.size();                                     // 要素数 O(1)
tr.empty();                                    // 空か判定 O(1)
tr.clear();                                    // 全要素削除 O(N)
tr.total_sum();                                // 全要素の総和 O(1)

tr.insert(x);                                  // x を挿入 期待 O(log N)
tr.erase(x);                                   // x を1個削除、成功なら true 期待 O(log N)
tr.erase_all(x);                               // x を全て削除、削除数を返す 期待 O(log N + 削除数)
tr.erase_kth(k);                               // 0-indexed k 番目を削除して値を返す 期待 O(log N)
tr.erase_at(k);                                // 0-indexed k 番目を削除 期待 O(log N)
tr.erase_index_range(l,r);                     // index [l,r) を削除、削除数を返す 期待 O(log N + 削除数)
tr.erase_value_range(L,R);                     // 値 [L,R] を削除、削除数を返す 期待 O(log N + 削除数)
tr.erase_less(x);                              // x 未満を全削除、削除数を返す 期待 O(log N + 削除数)
tr.erase_leq(x);                               // x 以下を全削除、削除数を返す 期待 O(log N + 削除数)
tr.erase_geq(x);                               // x 以上を全削除、削除数を返す 期待 O(log N + 削除数)
tr.erase_greater(x);                           // x より大きい値を全削除、削除数を返す 期待 O(log N + 削除数)

tr.get(k);                                     // 0-indexed k 番目の値 期待 O(log N)
tr.kth(k);                                     // get(k) と同じ 期待 O(log N)
tr.front();                                    // 最小値 期待 O(log N)
tr.back();                                     // 最大値 期待 O(log N)
tr.set_val(k,x);                               // k 番目を削除して x を再挿入 期待 O(log N)

tr.lower_bound(x);                             // x 以上の最初の index 期待 O(log N)
tr.upper_bound(x);                             // x より大きい最初の index 期待 O(log N)
tr.order_of_key(x);                            // x 未満の要素数 期待 O(log N)

tr.count(x);                                   // x の個数 期待 O(log N)
tr.contains(x);                                // x が存在するか 期待 O(log N)
tr.count_less(x);                              // x 未満の個数 期待 O(log N)
tr.count_leq(x);                               // x 以下の個数 期待 O(log N)
tr.count_geq(x);                               // x 以上の個数 期待 O(log N)
tr.count_greater(x);                           // x より大きい個数 期待 O(log N)
tr.count_value_range(L,R);                     // L<=x<=R の個数 期待 O(log N)
tr.first_index(x);                             // x の最初の index、なければ -1 期待 O(log N)
tr.last_index(x);                              // x の最後の index、なければ -1 期待 O(log N)

tr.min_value();                                // 最小値 期待 O(log N)
tr.max_value();                                // 最大値 期待 O(log N)
tr.pop_min();                                  // 最小値を削除して返す 期待 O(log N)
tr.pop_max();                                  // 最大値を削除して返す 期待 O(log N)

tr.predecessor(x);                             // 最大の y<x、なければ nullopt 期待 O(log N)
tr.successor(x);                               // 最小の y>x、なければ nullopt 期待 O(log N)
tr.floor_value(x);                             // 最大の y<=x、なければ nullopt 期待 O(log N)
tr.ceil_value(x);                              // 最小の y>=x、なければ nullopt 期待 O(log N)

tr.median_lower();                             // 小さい側の中央値 期待 O(log N)
tr.median_upper();                             // 大きい側の中央値 期待 O(log N)
tr.median_pair();                              // {小さい側,大きい側} の中央値 期待 O(log N)

tr.prefix_sum(k);                              // 小さい方から k 個の総和 期待 O(log N)
tr.range_sum(l,r);                             // index [l,r) の総和 期待 O(log N)
tr.sum_smallest(k);                            // 小さい方から k 個の総和 期待 O(log N)
tr.sum_largest(k);                             // 大きい方から k 個の総和 期待 O(log N)

tr.sum_less(x);                                // x 未満の値の総和 期待 O(log N)
tr.sum_leq(x);                                 // x 以下の値の総和 期待 O(log N)
tr.sum_geq(x);                                 // x 以上の値の総和 期待 O(log N)
tr.sum_greater(x);                             // x より大きい値の総和 期待 O(log N)
tr.sum_value_range(L,R);                       // L<=x<=R の総和 期待 O(log N)

tr.add_all(x);                                 // 全要素に x を加算 O(1)

auto [a,b]=tr.split_at(k);                     // [0,k) と [k,N) に分割、tr は空 期待 O(log N)
auto [a,b]=tr.split_less(x);                   // 値 <x と >=x に分割、tr は空 期待 O(log N)
auto [a,b]=tr.split_leq(x);                    // 値 <=x と >x に分割、tr は空 期待 O(log N)

auto t=tr.extract_range(l,r);                  // index [l,r) を tr から切り出して返す 期待 O(log N)
auto t=tr.extract_value_range(L,R);            // 値 [L,R] を tr から切り出して返す 期待 O(log N)

tr.merge_with(move(other));                    // tr の最大値<=other の最小値として結合、other は空 期待 O(log N)

auto v=tr.to_vector();                         // 昇順 vector に変換 O(N)

基本の関数
SortedImplicitTreap tr;
tr.build(vl{3,1,3});
tr.insert(2);
auto second = tr.kth(1);                      // 2
auto count = tr.count(3);                     // 2
tr.erase(3);                                  // 3を1個削除
*/
