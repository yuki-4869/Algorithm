#include <bits/stdc++.h>
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
        assert(0 <= digits && digits <= B + 1);
        assert(0 <= prefix && (prefix >> digits) == 0);

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

BinaryTrie bt;
bt.insert(5);
bt.insert(10);
ll a=bt.min_xor(7);
ll b=bt.kth(0);
bt.xor_all(3);
ll v=bt.get_node(6);
ll c=bt.node_count(v);
ll w=bt.get_prefix_node(0b00110,5);

重複要素に対応
erase(x)はxが存在する状態で使う
min_xor(x),max_xor(x)はsize()>0の状態で使う
kth(k)は0<=k<size()を想定
扱う整数は0以上2^(B+1)未満を想定。B=60ならbit60からbit0までを扱う
get_prefix_node(0b00110,5)はB=60ならbit60〜56が00110である値を表す
get_prefix_node(0,0)は空でなければ根Node 0を返す
get_node(x)とget_prefix_node(prefix,digits)はxor_all適用後の値を検索する
child(v,bit)のbitはTrie内に保存された値の桁（xor_all適用前）を表す
Node番号はtの添字。insertでtが拡張されても番号は変わらない

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
*/