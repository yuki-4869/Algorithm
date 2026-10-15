//DynamicWeightedWaveletMatrix

template<class Key = long long, class Val = long long>
class DynamicWeightedWaveletMatrix {
    template<class T>
    struct BIT {
        int n = 0;
        vector<T> bit;
        BIT() = default;
        BIT(int n) : n(n), bit(n + 1, T{}) {}
        void add(int p, T x) { for (++p; p <= n; p += p & -p) bit[p] += x; }
        T sum_prefix(int r) const {
            T ret{};
            for (; r; r -= r & -r) ret += bit[r];
            return ret;
        }
        T sum(int l, int r) const { return sum_prefix(r) - sum_prefix(l); }
    };

    struct BitSeq {
        struct Node {
            int l = 0, r = 0, sz = 1, ones = 0;
            uint32_t pri = 0;
            bool bit = false;
            Val val{}, sum0{}, sum{};
        };
        struct Agg {
            int ones = 0;
            Val sum0{}, sum{};
        };

        vector<Node> tr{Node()};
        vector<int> free_ids;
        int root = 0;
        uint64_t rng_state = 0x9e3779b97f4a7c15ULL;

        uint32_t rng() {
            rng_state ^= rng_state << 7;
            rng_state ^= rng_state >> 9;
            return (uint32_t)rng_state;
        }

        int size(int x) const { return x ? tr[x].sz : 0; }
        int ones(int x) const { return x ? tr[x].ones : 0; }
        Val sum0(int x) const { return x ? tr[x].sum0 : Val{}; }
        Val sum(int x) const { return x ? tr[x].sum : Val{}; }

        void pull(int x) {
            if (!x) return;
            auto &t = tr[x];
            t.sz = 1 + size(t.l) + size(t.r);
            t.ones = ones(t.l) + t.bit + ones(t.r);
            t.sum0 = sum0(t.l) + (t.bit ? Val{} : t.val) + sum0(t.r);
            t.sum = sum(t.l) + t.val + sum(t.r);
        }

        int new_node(bool bit, Val val) {
            int x;
            if (free_ids.empty()) {
                x = tr.size();
                tr.emplace_back();
            } else {
                x = free_ids.back();
                free_ids.pop_back();
                tr[x] = Node();
            }
            tr[x].bit = bit;
            tr[x].val = val;
            tr[x].pri = rng();
            tr[x].ones = bit;
            tr[x].sum0 = bit ? Val{} : val;
            tr[x].sum = val;
            return x;
        }

        int merge_node(int a, int b) {
            if (!a || !b) return a ? a : b;
            if (tr[a].pri > tr[b].pri) {
                tr[a].r = merge_node(tr[a].r, b);
                pull(a);
                return a;
            }
            tr[b].l = merge_node(a, tr[b].l);
            pull(b);
            return b;
        }

        void split_node(int x, int k, int& a, int& b) {
            if (!x) return void(a = b = 0);
            if (size(tr[x].l) >= k) {
                split_node(tr[x].l, k, a, tr[x].l);
                pull(x);
                b = x;
            } else {
                split_node(tr[x].r, k - size(tr[x].l) - 1, tr[x].r, b);
                pull(x);
                a = x;
            }
        }

        void build(const vector<char>& bits, const vector<Val>& vals) {
            int n = bits.size();
            tr.clear(); tr.reserve(n + 1); tr.emplace_back();
            free_ids.clear(); root = 0;

            vector<int> st;
            for (int i = 0; i < n; i++) {
                int x = new_node(bits[i], vals[i]), last = 0;
                while (!st.empty() && tr[st.back()].pri < tr[x].pri) {
                    last = st.back(); st.pop_back();
                }
                tr[x].l = last;
                if (!st.empty()) tr[st.back()].r = x;
                st.push_back(x);
            }
            if (st.empty()) return;
            root = st.front();

            vector<int> ord, dfs{root};
            while (!dfs.empty()) {
                int x = dfs.back(); dfs.pop_back();
                ord.push_back(x);
                if (tr[x].l) dfs.push_back(tr[x].l);
                if (tr[x].r) dfs.push_back(tr[x].r);
            }
            reverse(ord.begin(), ord.end());
            for (int x : ord) pull(x);
        }

        Agg prefix(int k) const {
            Agg ret;
            int x = root;
            while (x) {
                int ls = size(tr[x].l);
                if (k <= ls) { x = tr[x].l; continue; }

                ret.ones += ones(tr[x].l);
                ret.sum0 += sum0(tr[x].l);
                ret.sum += sum(tr[x].l);

                ret.ones += tr[x].bit;
                if (!tr[x].bit) ret.sum0 += tr[x].val;
                ret.sum += tr[x].val;

                k -= ls + 1;
                if (!k) break;
                x = tr[x].r;
            }
            return ret;
        }

        int rank1(int p) const { return prefix(p).ones; }
        int zeros() const { return size(root) - ones(root); }

        Val range_sum0(int l, int r) const {
            auto a = prefix(l), b = prefix(r);
            return b.sum0 - a.sum0;
        }

        Val range_sum(int l, int r) const {
            auto a = prefix(l), b = prefix(r);
            return b.sum - a.sum;
        }

        bool access(int p) const {
            int x = root;
            while (x) {
                int ls = size(tr[x].l);
                if (p < ls) x = tr[x].l;
                else if (p == ls) return tr[x].bit;
                else p -= ls + 1, x = tr[x].r;
            }
            assert(false);
            return false;
        }

        void set_val_impl(int x, int p, Val val) {
            int ls = size(tr[x].l);
            if (p < ls) set_val_impl(tr[x].l, p, val);
            else if (p == ls) tr[x].val = val;
            else set_val_impl(tr[x].r, p - ls - 1, val);
            pull(x);
        }

        void set_val(int p, Val val) { set_val_impl(root, p, val); }

        void insert(int p, bool bit, Val val) {
            int a, b;
            split_node(root, p, a, b);
            root = merge_node(merge_node(a, new_node(bit, val)), b);
        }

        void erase(int p) {
            int a, b, c;
            split_node(root, p, a, b);
            split_node(b, 1, b, c);
            assert(b);
            tr[b].l = tr[b].r = 0;
            free_ids.push_back(b);
            root = merge_node(a, c);
        }
    };

    int n = 0, sigma = 0, lg = 1;
    vector<Key> values;
    vector<int> rank_now;
    vector<Val> val_now;
    vector<BitSeq> mat;
    BitSeq leaf;
    BIT<Val> total;
    BIT<int> negative;

    int get_rank(const Key& x) const {
        auto it = lower_bound(values.begin(), values.end(), x);
        return it != values.end() && *it == x ? it - values.begin() : -1;
    }
    int lower_rank(const Key& x) const {
        return lower_bound(values.begin(), values.end(), x) - values.begin();
    }
    int upper_rank(const Key& x) const {
        return upper_bound(values.begin(), values.end(), x) - values.begin();
    }

    void erase_one(int pos, int rank) {
        int p = pos;
        for (int lv = 0; lv < lg; lv++) {
            int b = (rank >> (lg - 1 - lv)) & 1;
            assert(mat[lv].access(p) == b);
            int o = mat[lv].rank1(p), z = p - o, ztot = mat[lv].zeros();
            int np = b ? ztot + o : z;
            mat[lv].erase(p);
            p = np;
        }
        leaf.erase(p);
    }

    void insert_one(int pos, int rank, Val val) {
        int p = pos;
        for (int lv = 0; lv < lg; lv++) {
            int b = (rank >> (lg - 1 - lv)) & 1;
            int o = mat[lv].rank1(p), z = p - o, ztot = mat[lv].zeros();
            int np = b ? ztot + o : z;
            mat[lv].insert(p, b, val);
            p = np;
        }
        leaf.insert(p, false, val);
    }

    void update_val_one(int pos, int rank, Val val) {
        int p = pos;
        for (int lv = 0; lv < lg; lv++) {
            int b = (rank >> (lg - 1 - lv)) & 1;
            int o = mat[lv].rank1(p), z = p - o, ztot = mat[lv].zeros();
            int np = b ? ztot + o : z;
            mat[lv].set_val(p, val);
            p = np;
        }
        leaf.set_val(p, val);
    }

    int freq_rank(int l, int r, int upper) const {
        if (upper <= 0) return 0;
        if (upper >= sigma) return r - l;

        int ret = 0;
        for (int lv = 0; lv < lg; lv++) {
            int b = (upper >> (lg - 1 - lv)) & 1;
            int l1 = mat[lv].rank1(l), r1 = mat[lv].rank1(r);
            int l0 = l - l1, r0 = r - r1;

            if (b) {
                ret += r0 - l0;
                l = mat[lv].zeros() + l1;
                r = mat[lv].zeros() + r1;
            } else l = l0, r = r0;
        }
        return ret;
    }

    Val sum_rank(int l, int r, int upper) const {
        if (upper <= 0) return Val{};
        if (upper >= sigma) return range_sum(l, r);

        Val ret{};
        for (int lv = 0; lv < lg; lv++) {
            int b = (upper >> (lg - 1 - lv)) & 1;
            int l1 = mat[lv].rank1(l), r1 = mat[lv].rank1(r);
            int l0 = l - l1, r0 = r - r1;

            if (b) {
                ret += mat[lv].range_sum0(l, r);
                l = mat[lv].zeros() + l1;
                r = mat[lv].zeros() + r1;
            } else l = l0, r = r0;
        }
        return ret;
    }

public:
    DynamicWeightedWaveletMatrix() = default;

    DynamicWeightedWaveletMatrix(
        const vector<Key>& key,
        const vector<Val>& val,
        vector<Key> universe
    ) {
        build(key, val, move(universe));
    }

    DynamicWeightedWaveletMatrix(const vector<Key>& a, vector<Key> universe)
        requires same_as<Key, Val>
    {
        build(a, a, move(universe));
    }

    void build(
        const vector<Key>& key,
        const vector<Val>& val,
        vector<Key> universe
    ) {
        assert(key.size() == val.size());
        n = key.size();

        values = move(universe);
        values.insert(values.end(), key.begin(), key.end());
        sort(values.begin(), values.end());
        values.erase(unique(values.begin(), values.end()), values.end());

        sigma = values.size();
        assert(sigma || !n);

        lg = 1;
        while ((1LL << lg) < max(1, sigma)) lg++;

        rank_now.resize(n);
        val_now = val;

        vector<int> rk(n);
        vector<Val> w = val;

        for (int i = 0; i < n; i++) rk[i] = rank_now[i] = get_rank(key[i]);

        mat.assign(lg, BitSeq());

        for (int lv = 0; lv < lg; lv++) {
            int bit = lg - 1 - lv;
            vector<char> bits(n);
            for (int i = 0; i < n; i++) bits[i] = (rk[i] >> bit) & 1;

            mat[lv].build(bits, w);

            vector<int> nr;
            vector<Val> nw;
            nr.reserve(n); nw.reserve(n);

            for (int b = 0; b < 2; b++) {
                for (int i = 0; i < n; i++) if (bits[i] == b) {
                    nr.push_back(rk[i]);
                    nw.push_back(w[i]);
                }
            }
            rk.swap(nr);
            w.swap(nw);
        }

        leaf.build(vector<char>(n, 0), w);

        total = BIT<Val>(n);
        negative = BIT<int>(n);

        for (int i = 0; i < n; i++) {
            total.add(i, val[i]);
            if (val[i] < Val{}) negative.add(i, 1);
        }
    }

    void build(const vector<Key>& a, vector<Key> universe)
        requires same_as<Key, Val>
    {
        build(a, a, move(universe));
    }

    int size() const { return n; }
    Key get_key(int p) const { return values[rank_now[p]]; }
    Val get_value(int p) const { return val_now[p]; }
    pair<Key, Val> get(int p) const { return {get_key(p), get_value(p)}; }

    void set_key(int p, const Key& key) {
        int nr = get_rank(key);
        assert(0 <= p && p < n && nr != -1);
        if (rank_now[p] == nr) return;

        erase_one(p, rank_now[p]);
        insert_one(p, nr, val_now[p]);
        rank_now[p] = nr;
    }

    void set_value(int p, Val val) {
        assert(0 <= p && p < n);
        Val old = val_now[p];
        if (old == val) return;

        update_val_one(p, rank_now[p], val);
        val_now[p] = val;
        total.add(p, val - old);
        negative.add(p, int(val < Val{}) - int(old < Val{}));
    }

    void set(int p, const Key& key, Val val) {
        int nr = get_rank(key);
        assert(0 <= p && p < n && nr != -1);

        if (rank_now[p] == nr) return set_value(p, val);

        Val old = val_now[p];
        erase_one(p, rank_now[p]);
        insert_one(p, nr, val);

        rank_now[p] = nr;
        val_now[p] = val;

        total.add(p, val - old);
        negative.add(p, int(val < Val{}) - int(old < Val{}));
    }

    Val range_sum(int l, int r) const { return total.sum(l, r); }

    int count(const Key& x, int l, int r) const {
        int id = get_rank(x);
        return id == -1 ? 0 : freq_rank(l, r, id + 1) - freq_rank(l, r, id);
    }

    int range_freq(int l, int r, const Key& x) const {
        return freq_rank(l, r, lower_rank(x));
    }

    int range_freq(int l, int r, const Key& a, const Key& b) const {
        return range_freq(l, r, b) - range_freq(l, r, a);
    }

    int range_freq_leq(int l, int r, const Key& x) const {
        return freq_rank(l, r, upper_rank(x));
    }

    Val sum_less(int l, int r, const Key& x) const {
        return sum_rank(l, r, lower_rank(x));
    }

    Val sum_leq(int l, int r, const Key& x) const {
        return sum_rank(l, r, upper_rank(x));
    }

    Val sum_range(int l, int r, const Key& a, const Key& b) const {
        return sum_less(l, r, b) - sum_less(l, r, a);
    }

    Val sum_equal(int l, int r, const Key& x) const {
        return sum_leq(l, r, x) - sum_less(l, r, x);
    }

    Key kth_smallest(int l, int r, int k) const {
        assert(0 <= k && k < r - l);
        int rank = 0;

        for (int lv = 0; lv < lg; lv++) {
            int l1 = mat[lv].rank1(l), r1 = mat[lv].rank1(r);
            int l0 = l - l1, r0 = r - r1, z = r0 - l0;

            if (k < z) l = l0, r = r0;
            else {
                k -= z;
                rank |= 1 << (lg - 1 - lv);
                l = mat[lv].zeros() + l1;
                r = mat[lv].zeros() + r1;
            }
        }
        return values[rank];
    }

    Key kth_largest(int l, int r, int k) const {
        return kth_smallest(l, r, r - l - 1 - k);
    }

    optional<Key> prev_key(int l, int r, const Key& x) const {
        int k = range_freq(l, r, x);
        return k ? optional<Key>(kth_smallest(l, r, k - 1)) : nullopt;
    }

    optional<Key> next_key(int l, int r, const Key& x) const {
        int k = range_freq(l, r, x);
        return k == r - l ? nullopt : optional<Key>(kth_smallest(l, r, k));
    }

    optional<Key> min_key(int l, int r) const {
        return l == r ? nullopt : optional<Key>(kth_smallest(l, r, 0));
    }

    optional<Key> max_key(int l, int r) const {
        return l == r ? nullopt : optional<Key>(kth_largest(l, r, 0));
    }

    Val sum_k_smallest(int l, int r, int k) const {
        assert(0 <= k && k <= r - l);
        Val ret{};

        for (int lv = 0; lv < lg; lv++) {
            int l1 = mat[lv].rank1(l), r1 = mat[lv].rank1(r);
            int l0 = l - l1, r0 = r - r1, z = r0 - l0;

            if (k <= z) l = l0, r = r0;
            else {
                ret += mat[lv].range_sum0(l, r);
                k -= z;
                l = mat[lv].zeros() + l1;
                r = mat[lv].zeros() + r1;
            }
        }
        return ret + leaf.range_sum(l, l + k);
    }

    Val sum_k_largest(int l, int r, int k) const {
        return range_sum(l, r) - sum_k_smallest(l, r, r - l - k);
    }

    optional<Key> lower_bound_by_sum(int l, int r, Val k) const {
        assert(negative.sum(l, r) == 0);
        if (k <= Val{} || range_sum(l, r) < k) return nullopt;

        int rank = 0;

        for (int lv = 0; lv < lg; lv++) {
            int l1 = mat[lv].rank1(l), r1 = mat[lv].rank1(r);
            int l0 = l - l1, r0 = r - r1;
            Val s = mat[lv].range_sum0(l, r);

            if (s >= k) l = l0, r = r0;
            else {
                k -= s;
                rank |= 1 << (lg - 1 - lv);
                l = mat[lv].zeros() + l1;
                r = mat[lv].zeros() + r1;
            }
        }
        return values[rank];
    }
};

/*
0-indexed、区間は [l,r)

DynamicWeightedWaveletMatrix<ll,ll> wm(key,val,universe);
DynamicWeightedWaveletMatrix<ll,ll> wm(A,universe); // key==val

wm.get_key(p);                       // key[p]
wm.get_value(p);                     // val[p]
wm.get(p);                           // {key[p],val[p]}

wm.set_key(p,key);                   // key のみ点代入
wm.set_value(p,val);                 // val のみ点代入
wm.set(p,key,val);                   // key,val を同時点代入

wm.count(x,l,r);                     // key==x の個数
wm.range_freq(l,r,x);                // key<x の個数
wm.range_freq(l,r,a,b);              // a<=key<b の個数
wm.range_freq_leq(l,r,x);            // key<=x の個数

wm.kth_smallest(l,r,k);              // 0-indexed k番目に小さい key
wm.kth_largest(l,r,k);               // 0-indexed k番目に大きい key
wm.prev_key(l,r,x);                  // x未満の最大 key、なければ nullopt
wm.next_key(l,r,x);                  // x以上の最小 key、なければ nullopt
wm.min_key(l,r);                     // 最小 key、空なら nullopt
wm.max_key(l,r);                     // 最大 key、空なら nullopt

wm.range_sum(l,r);                   // Σ val
wm.sum_less(l,r,x);                  // key<x の val 総和
wm.sum_leq(l,r,x);                   // key<=x の val 総和
wm.sum_range(l,r,a,b);               // a<=key<b の val 総和
wm.sum_equal(l,r,x);                 // key==x の val 総和

wm.sum_k_smallest(l,r,k);            // key が小さい方から k 個の val 総和
wm.sum_k_largest(l,r,k);             // key が大きい方から k 個の val 総和

wm.lower_bound_by_sum(l,r,x);        // key昇順に val を足して累積>=xとなる最小key
                                     // [l,r) の val が全て非負である必要あり

※ set_key/set で使う可能性のある key は universe に事前登録
※ 同一 key の sum_k_smallest は index 小さい順で tie-break
※ 通常操作 O(log(sigma) log N)、range_sum O(log N)
*/