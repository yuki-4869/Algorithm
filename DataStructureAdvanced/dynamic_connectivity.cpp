struct Sum {
    long long val;
    Sum() : val(0) {}
    Sum(long long v) : val(v) {}
    static Sum et() { return Sum(0); }
    static Sum fn(const Sum& a, const Sum& b) { return Sum(a.val + b.val); }
};

template<typename T>
class dynamic_connectivity {
    class euler_tour_tree {
    public:
        struct node;
        using np = node*;
        using lint = long long;

        struct node {
            np ch[2] = {nullptr, nullptr};
            np p = nullptr;
            int l, r, sz;
            T val = T::et();
            T sum = T::et();
            bool exact = false;
            bool child_exact = false;
            bool edge_connected = false;
            bool child_edge_connected = false;

            node() {}
            node(int l, int r) : l(l), r(r), sz(l == r ? 1 : 0), exact(l < r), child_exact(l < r) {}
        bool is_root() {
            return !p;
          }
        };

        vector<unordered_map<int, np>> ptr;

        np get_node(int l, int r) {
            if (ptr[l].find(r) == ptr[l].end()) {
                ptr[l][r] = new node(l, r);
            }
            return ptr[l][r];
        }

        np root(np t) {
            if (!t) return t;
            while (t->p) t = t->p;
            return t;
        }

        bool same(np s, np t) {
            if (s) splay(s);
            if (t) splay(t);
            return root(s) == root(t);
        }

        np reroot(np t) {
            auto s = split(t);
            return merge(s.second, s.first);
        }

        pair<np, np> split(np s) {
            splay(s);
            np t = s->ch[0];
            if (t) t->p = nullptr;
            s->ch[0] = nullptr;
            return {t, update(s)};
        }

        pair<np, np> split2(np s) {
            splay(s);
            np t = s->ch[0];
            np u = s->ch[1];
            if (t) t->p = nullptr;
            s->ch[0] = nullptr;
            if (u) u->p = nullptr;
            s->ch[1] = nullptr;
            return {t, u};
        }

        tuple<np, np, np> split(np s, np t) {
            auto u = split2(s);
            if (same(u.first, t)) {
                auto r = split2(t);
                return make_tuple(r.first, r.second, u.second);
            } else {
                auto r = split2(t);
                return make_tuple(u.first, r.first, r.second);
            }
        }

        np merge(np s, np t) {
            if (!s) return t;
            if (!t) return s;
            while (s->ch[1]) s = s->ch[1];
            splay(s);
            s->ch[1] = t;
            if (t) t->p = s;
            return update(s);
        }

        template<typename First, typename... Rest>
        np merge(First s, Rest... t) {
            return merge(s, merge(t...));
        }

        int size(np t) { return t ? t->sz : 0; }

        np update(np t) {
            if (!t) return t;
            t->sum = T::et();
            if (t->ch[0]) t->sum = T::fn(t->sum, t->ch[0]->sum);
            if (t->l == t->r) t->sum = T::fn(t->sum, t->val);
            if (t->ch[1]) t->sum = T::fn(t->sum, t->ch[1]->sum);

            t->sz = size(t->ch[0]) + (t->l == t->r) + size(t->ch[1]);

            t->child_edge_connected =
                (t->ch[0] ? t->ch[0]->child_edge_connected : false) |
                t->edge_connected |
                (t->ch[1] ? t->ch[1]->child_edge_connected : false);

            t->child_exact =
                (t->ch[0] ? t->ch[0]->child_exact : false) |
                t->exact |
                (t->ch[1] ? t->ch[1]->child_exact : false);

            return t;
        }

        void rot(np t, bool b) {
            np x = t->p, y = x->p;
            if ((x->ch[1 - b] = t->ch[b])) t->ch[b]->p = x;
            t->ch[b] = x; x->p = t;
            update(x); update(t);
            if ((t->p = y)) {
                if (y->ch[0] == x) y->ch[0] = t;
                if (y->ch[1] == x) y->ch[1] = t;
                update(y);
            }
        }

        void splay(np t) {
            while (!t->is_root()) {
                np q = t->p;
                if (q->is_root()) {
                    rot(t, q->ch[0] == t);
                } else {
                    np r = q->p;
                    bool b = r->ch[0] == q;
                    if (q->ch[1 - b] == t) rot(q, b), rot(t, b);
                    else rot(t, 1 - b), rot(t, b);
                }
            }
        }

    public:
        euler_tour_tree() {}
        euler_tour_tree(int sz) {
            ptr.resize(sz);
            for (int i = 0; i < sz; i++) {
                ptr[i][i] = new node(i, i);
            }
        }

        void set_size(int sz) {
            ptr.resize(sz);
            for (int i = 0; i < sz; i++) {
                if (ptr[i].find(i) == ptr[i].end()) {
                    ptr[i][i] = new node(i, i);
                }
            }
        }

        int size(int s) {
            np t = get_node(s, s);
            splay(t);
            return t->sz;
        }

        bool same(int s, int t) {
            return same(get_node(s, s), get_node(t, t));
        }

        void update(int s, const T& x) {
            np t = get_node(s, s);
            splay(t);
            t->val = x;
            update(t);
        }

        void edge_update(int s, auto g) {
            np t = get_node(s, s);
            splay(t);
            function<void(np)> dfs = [&](np t) {
                do { if (!(t)) std::abort(); } while (false);
                if (t->l < t->r && t->exact) {
                    splay(t);
                    t->exact = 0;
                    update(t);
                    g(t->l, t->r);
                    return;
                }
                if (t->ch[0] && t->ch[0]->child_exact) dfs(t->ch[0]);
                else dfs(t->ch[1]);
            };
            while (t && t->child_exact) {
                dfs(t);
                splay(t);
            }
        }

        bool try_reconnect(int s, auto f) {
            np t = get_node(s, s);
            splay(t);
            function<bool(np)> dfs = [&](np t) -> bool {
                do { if (!(t)) std::abort(); } while (false);
                if (t->edge_connected) {
                    splay(t);
                    return f(t->l);
                }
                if (t->ch[0] && t->ch[0]->child_edge_connected) return dfs(t->ch[0]);
                else return dfs(t->ch[1]);
            };
            while (t->child_edge_connected) {
                if (dfs(t)) return true;
                splay(t);
            }
            return false;
        }

        void edge_connected_update(int s, bool b) {
            np t = get_node(s, s);
            splay(t);
            t->edge_connected = b;
            update(t);
        }

        bool link(int l, int r) {
            if (same(l, r)) return false;
            merge(reroot(get_node(l, l)), get_node(l, r), reroot(get_node(r, r)), get_node(r, l));
            return true;
        }

        bool cut(int l, int r) {
            if (ptr[l].find(r) == ptr[l].end()) return false;
            np s, t, u;
            tie(s, t, u) = split(get_node(l, r), get_node(r, l));
            merge(s, u);
            np p = ptr[l][r];
            np q = ptr[r][l];
            ptr[l].erase(r);
            ptr[r].erase(l);
            delete p; delete q;
            return true;
        }

        T get_sum(int v) {
            np t = get_node(v, v);
            splay(t);
            return t->sum;
        }

        T get_val(int v) {
            np t = get_node(v, v);
            splay(t);
            return t->val;
        }
    };

    int dep = 1;
    vector<euler_tour_tree> ett;
    vector<vector<unordered_set<int>>> edges;
    int n_vertices;

public:
    dynamic_connectivity(int n) : n_vertices(n) {
        ett.emplace_back(n);
        edges.emplace_back(n);
        for (int i = 0; i < n; i++) {
            ett[0].update(i, T());
        }
    }

    bool link(int s, int t) {
        if (s == t) return false;
        if (ett[0].link(s, t)) return true;
        edges[0][s].insert(t);
        edges[0][t].insert(s);
        if (edges[0][s].size() == 1) ett[0].edge_connected_update(s, true);
        if (edges[0][t].size() == 1) ett[0].edge_connected_update(t, true);
        return false;
    }

    bool cut(int s, int t) {
        if (s == t) return false;
        for (int i = 0; i < dep; i++) {
            edges[i][s].erase(t);
            edges[i][t].erase(s);
            if (edges[i][s].empty()) ett[i].edge_connected_update(s, false);
            if (edges[i][t].empty()) ett[i].edge_connected_update(t, false);
        }
        for (int i = dep - 1; i >= 0; i--) {
            if (ett[i].cut(s, t)) {
                if (dep - 1 == i) {
                    dep++;
                    ett.emplace_back(n_vertices);
                    edges.emplace_back(n_vertices);
                }
                return !try_reconnect(s, t, i);
            }
        }
        return false;
    }

    bool same(int s, int t) {
        return ett[0].same(s, t);
    }

    int size(int s) {
        return ett[0].size(s);
    }

    void update(int s, long long x) {
        T val(x);
        for (int i = 0; i < dep; i++) {
            ett[i].update(s, val);
        }
    }

    long long get_sum(int s) {
        return ett[0].get_sum(s).val;
    }

    long long get_val(int s) {
        return ett[0].get_val(s).val;
    }

private:
    bool try_reconnect(int s, int t, int k) {
        for (int i = 0; i < k; i++) {
            ett[i].cut(s, t);
        }
        for (int i = k; i >= 0; i--) {
            if (ett[i].size(s) > ett[i].size(t)) swap(s, t);
            auto g = [&](int u, int v) { ett[i + 1].link(u, v); };
            ett[i].edge_update(s, g);

            auto f = [&](int x) -> bool {
                for (auto itr = edges[i][x].begin(); itr != edges[i][x].end(); ) {
                    int y = *itr;
                    itr = edges[i][x].erase(itr);
                    edges[i][y].erase(x);
                    if (edges[i][x].empty()) ett[i].edge_connected_update(x, false);
                    if (edges[i][y].empty()) ett[i].edge_connected_update(y, false);

                    if (ett[i].same(x, y)) {
                        edges[i + 1][x].insert(y);
                        edges[i + 1][y].insert(x);
                        if (edges[i + 1][x].size() == 1) ett[i + 1].edge_connected_update(x, true);
                        if (edges[i + 1][y].size() == 1) ett[i + 1].edge_connected_update(y, true);
                    } else {
                        for (int j = 0; j <= i; j++) {
                            ett[j].link(x, y);
                        }
                        return true;
                    }
                }
                return false;
            };
            if (ett[i].try_reconnect(s, f)) return true;
        }
        return false;
    }
};

/*
細かい注意点
0-indexed、無向単純グラフ用。同じ辺の重複追加や自己ループは避ける。
Sumで頂点値と連結成分和を管理する。updateは加算ではなく代入。
linkのtrueは異なる成分を結んだ場合、cutのtrueは削除後に成分が分離した場合。
Euler Tour Treeと多段階の再接続探索を用いる。辺更新は償却O(log^2 N)。
頂点値・集約値はlong longに収まること。ノード所有権を共有するコピーをしない。
C++20以降を使用する。

使える関数一覧
dynamic_connectivity<Sum> dc(n); // 頂点値0のn頂点で構築
dc.link(u,v);                   // bool: 辺追加、森の辺として追加されたか
dc.cut(u,v);                    // bool: 辺削除で成分が分離したか
dc.same(u,v);                   // bool: 連結判定
dc.size(u);                     // int: uの成分の頂点数
dc.update(u,x);                 // void: 頂点値をxに代入
dc.get_val(u);                  // ll: 頂点値
dc.get_sum(u);                  // ll: 成分和

基本の関数
dynamic_connectivity<Sum> dc(3);
dc.update(0,5);
dc.update(1,2);
dc.link(0,1);
auto connected = dc.same(0,1);                // true
auto sum = dc.get_sum(0);                     // 7
dc.cut(0,1);
*/
