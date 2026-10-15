//01-StringWavelerMatrix

class StringWaveletMatrix {
    struct Node {
        int ch[2] = {0, 0};
        int cnt = 0;
    };

    int L = 0;
    int root = 1;

    vector<Node> tr;
    vector<int> free_ids;

    // indexed data
    vector<string> a;
    vector<int> one_cnt;

    static int bval(char c) {
        assert(c == '0' || c == '1');
        return c - '0';
    }

    int new_node() {
        if (!free_ids.empty()) {
            int x = free_ids.back();
            free_ids.pop_back();
            tr[x] = Node();
            return x;
        }

        tr.emplace_back();
        return (int)tr.size() - 1;
    }

    void recycle_node(int x) {
        tr[x] = Node();
        free_ids.push_back(x);
    }

    void check_key(string_view s) const {
        assert((int)s.size() == L);
        for (char c : s) {
            assert(c == '0' || c == '1');
        }
    }

    void insert_raw(string_view s) {
        assert((int)s.size() == L);

        int x = root;
        ++tr[x].cnt;

        for (char c : s) {
            int b = bval(c);

            if (!tr[x].ch[b]) {
                tr[x].ch[b] = new_node();
            }

            x = tr[x].ch[b];
            ++tr[x].cnt;
        }
    }

    void erase_raw(string_view s) {
        assert((int)s.size() == L);

        vector<int> path(L + 1);

        path[0] = root;

        int x = root;

        for (int d = 0; d < L; d++) {
            int b = bval(s[d]);

            assert(tr[x].ch[b]);

            x = tr[x].ch[b];
            path[d + 1] = x;
        }

        assert(tr[x].cnt > 0);

        for (int v : path) {
            --tr[v].cnt;
        }

        // 不要になった suffix を再利用可能にする
        for (int d = L; d >= 1; d--) {
            int v = path[d];

            if (tr[v].cnt != 0) break;

            int p = path[d - 1];
            int b = bval(s[d - 1]);

            assert(tr[p].ch[b] == v);

            tr[p].ch[b] = 0;
            recycle_node(v);
        }
    }

public:
    StringWaveletMatrix() {
        tr.resize(2);
    }

    explicit StringWaveletMatrix(vector<string> s) {
        build(move(s));
    }

    void build(vector<string> s) {
        L = s.empty() ? 0 : (int)s[0].size();

        for (auto &x : s) {
            assert((int)x.size() == L);

            for (char c : x) {
                assert(c == '0' || c == '1');
            }
        }

        a = move(s);

        one_cnt.assign(a.size(), 0);

        for (int i = 0; i < (int)a.size(); i++) {
            one_cnt[i] = count(
                a[i].begin(),
                a[i].end(),
                '1'
            );
        }

        tr.clear();
        free_ids.clear();

        // 最大でもほぼ N*K ノード
        tr.reserve(
            2 + (size_t)a.size() * L
        );

        tr.resize(2);
        root = 1;

        for (auto &x : a) {
            insert_raw(x);
        }
    }

    // --------------------------------
    // basic
    // --------------------------------

    int size() const {
        return tr[root].cnt;
    }

    int rows() const {
        return (int)a.size();
    }

    int width() const {
        return L;
    }

    const string& get_key(int id) const {
        assert(0 <= id && id < rows());
        return a[id];
    }

    int get_bit(int id, int p) const {
        assert(0 <= id && id < rows());
        assert(0 <= p && p < L);

        return a[id][p] - '0';
    }

    int ones(int id) const {
        assert(0 <= id && id < rows());

        return one_cnt[id];
    }

    int zeros(int id) const {
        return L - ones(id);
    }

    // --------------------------------
    // update
    // --------------------------------

    void set_key(int id, string s) {
        assert(0 <= id && id < rows());

        check_key(s);

        if (a[id] == s) return;

        erase_raw(a[id]);

        one_cnt[id] = count(
            s.begin(),
            s.end(),
            '1'
        );

        a[id] = move(s);

        insert_raw(a[id]);
    }

    void set_bit(int id, int p, int b) {
        assert(0 <= id && id < rows());
        assert(0 <= p && p < L);
        assert(b == 0 || b == 1);

        int old = a[id][p] - '0';

        if (old == b) return;

        erase_raw(a[id]);

        a[id][p] = char('0' + b);
        one_cnt[id] += b - old;

        insert_raw(a[id]);
    }

    void flip(int id, int p) {
        assert(0 <= id && id < rows());
        assert(0 <= p && p < L);

        set_bit(
            id,
            p,
            1 - get_bit(id, p)
        );
    }

    // --------------------------------
    // count
    // --------------------------------

    int count_equal(string_view s) const {
        check_key(s);

        int x = root;

        for (char c : s) {
            x = tr[x].ch[bval(c)];

            if (!x) return 0;
        }

        return tr[x].cnt;
    }

    // key < s
    int count_less(string_view s) const {
        check_key(s);

        int x = root;
        int ret = 0;

        for (char c : s) {
            if (!x) break;

            int b = bval(c);

            if (b == 1) {
                int y = tr[x].ch[0];

                if (y) {
                    ret += tr[y].cnt;
                }
            }

            x = tr[x].ch[b];
        }

        return ret;
    }

    // key <= s
    int count_leq(string_view s) const {
        return count_less(s)
             + count_equal(s);
    }

    // key > s
    int count_greater(string_view s) const {
        return size()
             - count_leq(s);
    }

    // key >= s
    int count_geq(string_view s) const {
        return size()
             - count_less(s);
    }

    // l <= key < r
    int count_range(
        string_view l,
        string_view r
    ) const {
        return count_less(r)
             - count_less(l);
    }

    // prefix から始まる文字列数
    int count_prefix(string_view p) const {
        assert((int)p.size() <= L);

        int x = root;

        for (char c : p) {
            int b = bval(c);

            x = tr[x].ch[b];

            if (!x) return 0;
        }

        return tr[x].cnt;
    }

    // --------------------------------
    // order statistics
    // --------------------------------

    // 0-indexed
    string kth_smallest(int k) const {
        assert(0 <= k && k < size());

        string ret;
        ret.reserve(L);

        int x = root;

        for (int d = 0; d < L; d++) {
            int z = 0;

            if (tr[x].ch[0]) {
                z = tr[
                    tr[x].ch[0]
                ].cnt;
            }

            if (k < z) {
                ret.push_back('0');
                x = tr[x].ch[0];
            }
            else {
                k -= z;

                ret.push_back('1');
                x = tr[x].ch[1];

                assert(x);
            }
        }

        return ret;
    }

    // 0-indexed
    string kth_largest(int k) const {
        assert(0 <= k && k < size());

        return kth_smallest(
            size() - 1 - k
        );
    }

    optional<string> min_key() const {
        if (size() == 0) {
            return nullopt;
        }

        return kth_smallest(0);
    }

    optional<string> max_key() const {
        if (size() == 0) {
            return nullopt;
        }

        return kth_largest(0);
    }

    // >= s の最小
    optional<string>
    lower_bound_key(string_view s) const {
        int k = count_less(s);

        if (k == size()) {
            return nullopt;
        }

        return kth_smallest(k);
    }

    // > s の最小
    optional<string>
    upper_bound_key(string_view s) const {
        int k = count_leq(s);

        if (k == size()) {
            return nullopt;
        }

        return kth_smallest(k);
    }

    // < s の最大
    optional<string>
    prev_key(string_view s) const {
        int k = count_less(s);

        if (k == 0) {
            return nullopt;
        }

        return kth_smallest(k - 1);
    }

    // >= s の最小
    optional<string>
    next_key(string_view s) const {
        return lower_bound_key(s);
    }

    // --------------------------------
    // indexed versions
    // --------------------------------

    int count_equal_id(int id) const {
        return count_equal(
            get_key(id)
        );
    }

    int count_less_id(int id) const {
        return count_less(
            get_key(id)
        );
    }

    int count_leq_id(int id) const {
        return count_leq(
            get_key(id)
        );
    }

    int count_greater_id(int id) const {
        return count_greater(
            get_key(id)
        );
    }

    int count_geq_id(int id) const {
        return count_geq(
            get_key(id)
        );
    }

    bool can_pass(int id, int M) const {
        if (ones(id) == 0) {
            return false;
        }

        return count_geq_id(id) <= M;
    }
};

/*
0-indexed



※ key の大小比較は01-stringの辞書順
   長さが全て等しいので、二進整数としての大小関係とも一致する

※ build 後も任意の01-stringへ set_key 可能
   universe の事前登録は不要

※ flip(i,j) は
   erase -> 1bit反転 -> insert
   を内部で行う

※ 同じ key を複数個持つことが可能

※ kth_smallest / kth_largest は重複も1個ずつ数える

※ count_prefix("101") は
   "101..." で始まる全 key の個数

※ 全体計算量
   build   : O(NK)
   各更新  : O(K)
   各検索  : O(K)
   メモリ  : O(NK)


StringWaveletMatrix wm(A);            // O(NK)
// A : vector<string>
// 全て同じ長さ K の 01-string

wm.size();                            // 管理している文字列数                    O(1)
wm.rows();                            // size() と同じ                           O(1)
wm.width();                           // 各01-stringの長さ                       O(1)

wm.get_key(i);                        // i番目の01-string                        O(1)
wm.get_bit(i,j);                      // i番目の文字列の j bit目 (0 or 1)        O(1)
wm.ones(i);                           // i番目の文字列に含まれる 1 の個数         O(1)
wm.zeros(i);                          // i番目の文字列に含まれる 0 の個数         O(1)

wm.set_key(i,s);                      // i番目の文字列を s に変更                 O(K)
wm.set_bit(i,j,b);                    // i番目の j bit目を b に変更               O(K)
wm.flip(i,j);                         // i番目の j bit目を 0<->1 反転             O(K)

wm.count_equal(s);                    // key==s の個数                           O(K)
wm.count_less(s);                     // key<s の個数                            O(K)
wm.count_leq(s);                      // key<=s の個数                           O(K)
wm.count_greater(s);                  // key>s の個数                            O(K)
wm.count_geq(s);                      // key>=s の個数                           O(K)
wm.count_range(l,r);                  // l<=key<r の個数                         O(K)
wm.count_prefix(p);                   // prefix が p の文字列の個数               O(|p|)

wm.count_equal_id(i);                 // key==key[i] の個数                      O(K)
wm.count_less_id(i);                  // key<key[i] の個数                       O(K)
wm.count_leq_id(i);                   // key<=key[i] の個数                      O(K)
wm.count_greater_id(i);               // key>key[i] の個数                       O(K)
wm.count_geq_id(i);                   // key>=key[i] の個数                      O(K)

wm.kth_smallest(k);                   // 0-indexed k番目に小さい key             O(K)
wm.kth_largest(k);                    // 0-indexed k番目に大きい key             O(K)

wm.lower_bound_key(s);                // s以上の最小 key、なければ nullopt        O(K)
wm.upper_bound_key(s);                // sより大きい最小 key、なければ nullopt    O(K)
wm.prev_key(s);                       // s未満の最大 key、なければ nullopt        O(K)
wm.next_key(s);                       // s以上の最小 key、なければ nullopt        O(K)

wm.min_key();                         // 全体の最小 key、空なら nullopt           O(K)
wm.max_key();                         // 全体の最大 key、空なら nullopt           O(K)

wm.can_pass(i,M);                    
                                     // key[i] != 00...0 かつ
                                     // key>=key[i] の人数が M 以下なら true  0(K)
*/