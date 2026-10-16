class StringWaveletMatrix {
    struct Node {
        int ch[2] = {0, 0};
        int cnt = 0;
    };

    int L = 0;
    int root = 1;

    vector<Node> tr;
    vector<int> free_ids;

    vector<string> a;
    vector<int> one_cnt;

    static int bval(char c) {
        do { if (!(c == '0' || c == '1')) std::abort(); } while (false);
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
        do { if (!((int)s.size() == L)) std::abort(); } while (false);
        for (char c : s) {
            do { if (!(c == '0' || c == '1')) std::abort(); } while (false);
        }
    }

    void insert_raw(string_view s) {
        do { if (!((int)s.size() == L)) std::abort(); } while (false);

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
        do { if (!((int)s.size() == L)) std::abort(); } while (false);

        vector<int> path(L + 1);

        path[0] = root;

        int x = root;

        for (int d = 0; d < L; d++) {
            int b = bval(s[d]);

            do { if (!(tr[x].ch[b])) std::abort(); } while (false);

            x = tr[x].ch[b];
            path[d + 1] = x;
        }

        do { if (!(tr[x].cnt > 0)) std::abort(); } while (false);

        for (int v : path) {
            --tr[v].cnt;
        }

        for (int d = L; d >= 1; d--) {
            int v = path[d];

            if (tr[v].cnt != 0) break;

            int p = path[d - 1];
            int b = bval(s[d - 1]);

            do { if (!(tr[p].ch[b] == v)) std::abort(); } while (false);

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
            do { if (!((int)x.size() == L)) std::abort(); } while (false);

            for (char c : x) {
                do { if (!(c == '0' || c == '1')) std::abort(); } while (false);
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

        tr.reserve(
            2 + (size_t)a.size() * L
        );

        tr.resize(2);
        root = 1;

        for (auto &x : a) {
            insert_raw(x);
        }
    }

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
        do { if (!(0 <= id && id < rows())) std::abort(); } while (false);
        return a[id];
    }

    int get_bit(int id, int p) const {
        do { if (!(0 <= id && id < rows())) std::abort(); } while (false);
        do { if (!(0 <= p && p < L)) std::abort(); } while (false);

        return a[id][p] - '0';
    }

    int ones(int id) const {
        do { if (!(0 <= id && id < rows())) std::abort(); } while (false);

        return one_cnt[id];
    }

    int zeros(int id) const {
        return L - ones(id);
    }

    void set_key(int id, string s) {
        do { if (!(0 <= id && id < rows())) std::abort(); } while (false);

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
        do { if (!(0 <= id && id < rows())) std::abort(); } while (false);
        do { if (!(0 <= p && p < L)) std::abort(); } while (false);
        do { if (!(b == 0 || b == 1)) std::abort(); } while (false);

        int old = a[id][p] - '0';

        if (old == b) return;

        erase_raw(a[id]);

        a[id][p] = char('0' + b);
        one_cnt[id] += b - old;

        insert_raw(a[id]);
    }

    void flip(int id, int p) {
        do { if (!(0 <= id && id < rows())) std::abort(); } while (false);
        do { if (!(0 <= p && p < L)) std::abort(); } while (false);

        set_bit(
            id,
            p,
            1 - get_bit(id, p)
        );
    }

    int count_equal(string_view s) const {
        check_key(s);

        int x = root;

        for (char c : s) {
            x = tr[x].ch[bval(c)];

            if (!x) return 0;
        }

        return tr[x].cnt;
    }

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

    int count_leq(string_view s) const {
        return count_less(s)
             + count_equal(s);
    }

    int count_greater(string_view s) const {
        return size()
             - count_leq(s);
    }

    int count_geq(string_view s) const {
        return size()
             - count_less(s);
    }

    int count_range(
        string_view l,
        string_view r
    ) const {
        return count_less(r)
             - count_less(l);
    }

    int count_prefix(string_view p) const {
        do { if (!((int)p.size() <= L)) std::abort(); } while (false);

        int x = root;

        for (char c : p) {
            int b = bval(c);

            x = tr[x].ch[b];

            if (!x) return 0;
        }

        return tr[x].cnt;
    }

    string kth_smallest(int k) const {
        do { if (!(0 <= k && k < size())) std::abort(); } while (false);

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

                do { if (!(x)) std::abort(); } while (false);
            }
        }

        return ret;
    }

    string kth_largest(int k) const {
        do { if (!(0 <= k && k < size())) std::abort(); } while (false);

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

    optional<string>
    lower_bound_key(string_view s) const {
        int k = count_less(s);

        if (k == size()) {
            return nullopt;
        }

        return kth_smallest(k);
    }

    optional<string>
    upper_bound_key(string_view s) const {
        int k = count_leq(s);

        if (k == size()) {
            return nullopt;
        }

        return kth_smallest(k);
    }

    optional<string>
    prev_key(string_view s) const {
        int k = count_less(s);

        if (k == 0) {
            return nullopt;
        }

        return kth_smallest(k - 1);
    }

    optional<string>
    next_key(string_view s) const {
        return lower_bound_key(s);
    }

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
細かい注意点
0-indexed。全て同じ長さKの01文字列を管理し、大小は辞書順（同長の二進数順と一致）。
キーの重複可。kthのkは0-indexedで、重複も1つずつ数える。
build後も同じ長さの任意のキーに変更でき、universeの事前登録は不要。
flipは削除・1bit反転・再挿入。count_prefixは指定prefixで始まるキーの個数。
構築O(NK)、更新・通常検索O(K)、メモリO(NK)。キー取得の参照を更新後まで保持しない。

使える関数一覧
StringWaveletMatrix wm(A);            // O(NK)

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
                                     // key>=key[i] の人数が M 以下なら true  O(K)

基本の関数
StringWaveletMatrix wm(vector<string>{"001","101","001"});
auto count = wm.count_equal("001");          // 2
wm.flip(0,0);                                // 先頭キーが101へ変化
auto smallest = wm.kth_smallest(0);           // 001
*/
