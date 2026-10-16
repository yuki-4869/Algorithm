using namespace std;
using ll = long long;

struct BinaryTrie {
    static const int B = 60;

    struct N {
        ll ch[2] = {-1, -1};
        ll cnt = 0;
    };

    vector<N> t = vector<N>(1);
    ll lazy = 0;

    void insert(ll x) {
        x ^= lazy;
        ll v = 0;
        t[v].cnt++;
        for (ll b = B; b >= 0; b--) {
            ll c = (x >> b) & 1;
            if (t[v].ch[c] < 0) {
                t[v].ch[c] = t.size();
                t.push_back({});
            }
            v = t[v].ch[c];
            t[v].cnt++;
        }
    }

    void erase(ll x) {
        x ^= lazy;
        ll v = 0;
        t[v].cnt--;
        for (ll b = B; b >= 0; b--) {
            v = t[v].ch[(x >> b) & 1];
            t[v].cnt--;
        }
    }

    ll count(ll x) const {
        x ^= lazy;
        ll v = 0;
        for (ll b = B; b >= 0; b--) {
            v = t[v].ch[(x >> b) & 1];
            if (v < 0) return 0;
        }
        return t[v].cnt;
    }

    ll min_xor(ll x) const {
        x ^= lazy;
        ll v = 0, res = 0;
        for (ll b = B; b >= 0; b--) {
            ll c = (x >> b) & 1, n = t[v].ch[c];
            if (n < 0 || !t[n].cnt) c ^= 1, res |= 1LL << b;
            v = t[v].ch[c];
        }
        return res;
    }

    ll max_xor(ll x) const {
        x ^= lazy;
        ll v = 0, res = 0;
        for (ll b = B; b >= 0; b--) {
            ll c = ((x >> b) & 1) ^ 1, n = t[v].ch[c];
            if (n < 0 || !t[n].cnt) c ^= 1;
            else res |= 1LL << b;
            v = t[v].ch[c];
        }
        return res;
    }

    ll kth(ll k) const {
        ll v = 0, res = 0;
        for (ll b = B; b >= 0; b--) {
            ll z = (lazy >> b) & 1, n = t[v].ch[z];
            ll c = n < 0 ? 0 : t[n].cnt;
            if (k >= c) k -= c, z ^= 1;
            if (z != ((lazy >> b) & 1)) res |= 1LL << b;
            v = t[v].ch[z];
        }
        return res;
    }

    void xor_all(ll x) { lazy ^= x; }

    ll size() const { return t[0].cnt; }

    ll child(ll v, ll bit) const {
        if (v < 0) return -1;
        ll u = t[v].ch[bit];
        return u >= 0 && t[u].cnt > 0 ? u : -1;
    }

    ll node_count(ll v) const { return v < 0 ? 0 : t[v].cnt; }

    ll get_node(ll x) const {
        x ^= lazy;
        ll v = node_count(0) > 0 ? 0 : -1;
        for (ll b = B; b >= 0 && v >= 0; b--) {
            v = child(v, (x >> b) & 1);
        }
        return v;
    }

    ll get_prefix_node(ll prefix, int digits) const {
        do { if (!(0 <= digits && digits <= B + 1)) std::abort(); } while (false);
        do { if (!(0 <= prefix && (prefix >> digits) == 0)) std::abort(); } while (false);

        ll v = node_count(0) > 0 ? 0 : -1;
        for (int j = digits - 1; j >= 0 && v >= 0; j--) {
            int b = B - (digits - 1 - j);
            ll bit = ((prefix >> j) & 1) ^ ((lazy >> b) & 1);
            v = child(v, bit);
        }
        return v;
    }
};

/*
細かい注意点
非負整数の多重集合。値は0以上2^(B+1)未満（既定B=60）。
eraseは登録済みの値、min_xor/max_xorは非空、kthは0<=k<size()で使用する。
get_node/get_prefix_nodeはxor_all後の値、childのbitは内部保存値に対する桁。
get_prefix_node(prefix,digits)は上位digits桁を検索。digits=0は根を表す。
ノード番号はinsertによる領域拡張でも変わらない。log AはB+1に相当する。

使える関数一覧
BinaryTrie bt;構築

void insert(x) xを1個追加 O(logA)
void erase(x) xを1個削除 O(logA)
ll count(x) xの個数を返す O(logA)
ll min_xor(x) 登録値vに対するmin(v xor x)を返す O(logA)
ll max_xor(x) 登録値vに対するmax(v xor x)を返す O(logA)
ll kth(k) 値の昇順で0-indexed k番目を返す O(logA)
void xor_all(x) 全要素にxor xを作用させる O(1)
ll size() 登録されている要素数を返す O(1)

ll get_node(x) 現在の値xに対応する葉Nodeの番号を返す。存在しなければ-1 O(logA)
ll child(v,bit) Node vから保存値の次の桁がbitの子Nodeの番号を返す。存在しなければ-1 O(1)
ll node_count(v) Node v以下の要素数を返す。v==-1なら0 O(1)
ll get_prefix_node(prefix,digits) 現在の値の上位digits桁がprefixのNodeの番号を返す。存在しなければ-1 O(digits)

基本の関数
BinaryTrie bt;                                // 空で構築
bt.insert(5);                                 // 5を追加
bt.insert(10);
auto minimum = bt.min_xor(7);                 // min(v xor 7)=2
auto first = bt.kth(0);                       // 最小値5
bt.erase(5);                                  // 5を1個削除
*/
