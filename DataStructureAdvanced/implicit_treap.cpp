struct ImplicitTreap {
    struct Node {
        ll val, sum;
        ll lazy_add;
        bool rev;
        uint32_t pri;
        int sz;
        Node *l, *r;

        Node(ll v, uint32_t p)
            : val(v), sum(v), lazy_add(0), rev(false),
              pri(p), sz(1), l(nullptr), r(nullptr) {}
    };

    Node* root = nullptr;

    uint32_t xorshift() {
        static uint32_t x = 123456789;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        return x;
    }

    int size(Node* t) const {
        return t ? t->sz : 0;
    }

    ll sum(Node* t) const {
        return t ? t->sum : 0;
    }

    Node* new_node(ll v) {
        return new Node(v, xorshift());
    }

    void update(Node* t) {
        if (!t) return;
        t->sz = 1 + size(t->l) + size(t->r);
        t->sum = t->val + sum(t->l) + sum(t->r);
    }

    void apply_add(Node* t, ll x) {
        if (!t) return;
        t->val += x;
        t->sum += x * t->sz;
        t->lazy_add += x;
    }

    void apply_rev(Node* t) {
        if (!t) return;
        swap(t->l, t->r);
        t->rev ^= true;
    }

    void push(Node* t) {
        if (!t) return;

        if (t->rev) {
            apply_rev(t->l);
            apply_rev(t->r);
            t->rev = false;
        }

        if (t->lazy_add != 0) {
            apply_add(t->l, t->lazy_add);
            apply_add(t->r, t->lazy_add);
            t->lazy_add = 0;
        }
    }

    Node* merge(Node* a, Node* b) {
        if (!a) return b;
        if (!b) return a;

        if (a->pri > b->pri) {
            push(a);
            a->r = merge(a->r, b);
            update(a);
            return a;
        } else {
            push(b);
            b->l = merge(a, b->l);
            update(b);
            return b;
        }
    }

    pair<Node*, Node*> split(Node* t, int k) {
        if (!t) return {nullptr, nullptr};

        push(t);

        if (size(t->l) >= k) {
            auto [a, b] = split(t->l, k);
            t->l = b;
            update(t);
            return {a, t};
        } else {
            auto [a, b] = split(t->r, k - size(t->l) - 1);
            t->r = a;
            update(t);
            return {t, b};
        }
    }

    int size() const {
        return size(root);
    }

    void insert(int pos, ll x) {
        auto [a, b] = split(root, pos);
        root = merge(merge(a, new_node(x)), b);
    }

    void push_back(ll x) {
        insert(size(), x);
    }

    void erase(int pos) {
        auto [a, b] = split(root, pos);
        auto [c, d] = split(b, 1);
        root = merge(a, d);
    }

    ll get(int pos) {
        auto [a, b] = split(root, pos);
        auto [c, d] = split(b, 1);

        ll res = c->val;

        root = merge(a, merge(c, d));
        return res;
    }

    void set_val(int pos, ll x) {
        auto [a, b] = split(root, pos);
        auto [c, d] = split(b, 1);

        c->val = x;
        update(c);

        root = merge(a, merge(c, d));
    }

    void range_add(int l, int r, ll x) {
        auto [a, b] = split(root, l);
        auto [c, d] = split(b, r - l);

        apply_add(c, x);

        root = merge(a, merge(c, d));
    }

    void range_reverse(int l, int r) {
        auto [a, b] = split(root, l);
        auto [c, d] = split(b, r - l);

        apply_rev(c);

        root = merge(a, merge(c, d));
    }

    ll range_sum(int l, int r) {
        auto [a, b] = split(root, l);
        auto [c, d] = split(b, r - l);

        ll res = sum(c);

        root = merge(a, merge(c, d));
        return res;
    }

    void dfs(Node* t, vector<ll>& res) {
        if (!t) return;
        push(t);
        dfs(t->l, res);
        res.push_back(t->val);
        dfs(t->r, res);
    }

    vector<ll> to_vector() {
        vector<ll> res;
        res.reserve(size());
        dfs(root, res);
        return res;
    }

    void build(const vector<ll>& v) {
        root = nullptr;
        for (ll x : v) {
            root = merge(root, new_node(x));
        }
    }
};

/*
細かい注意点
列の添字は0-indexed、区間は[l,r)。挿入は0<=pos<=size()、取得・削除は0<=pos<size()。
各操作は乱択木による期待計算量。和・加算はlong longに収まること。
ノードを共有する浅いコピーをしない。削除・再構築時に古いノードを解放しない実装。
メモリ使用量は現在長だけでなく、これまでに確保したノード数に依存する。

使える関数一覧
tr.build(v);             // void: 列vから構築 期待O(N log N)
tr.size();               // int: 列長 O(1)
tr.insert(pos,x);        // void: pos直前に挿入 期待O(log N)
tr.push_back(x);         // void: 末尾に挿入 期待O(log N)
tr.erase(pos);           // void: posの要素を削除 期待O(log N)
tr.get(pos);             // ll: posの値 期待O(log N)
tr.set_val(pos,x);       // void: posの値を変更 期待O(log N)
tr.range_add(l,r,x);     // void: 区間加算 期待O(log N)
tr.range_reverse(l,r);   // void: 区間反転 期待O(log N)
tr.range_sum(l,r);       // ll: 区間和 期待O(log N)
tr.to_vector();          // vector<ll>: 現在の列 O(N)

基本の関数
ImplicitTreap tr;
tr.build({1,2,3});
tr.insert(1,10);                              // {1,10,2,3}
tr.range_add(0,2,5);                          // {6,15,2,3}
auto sum = tr.range_sum(0,4);                 // 26
tr.erase(1);                                  // {6,2,3}
*/
