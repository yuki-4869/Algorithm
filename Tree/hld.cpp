//HLD

struct HLD {
    int n;
    vector<vector<int>> graph;

    vector<int> parent;
    vector<int> depth;
    vector<int> size;
    vector<int> heavy;

    vector<int> head;
    vector<int> in;
    vector<int> out;
    vector<int> rev;

    HLD() : n(0) {}

    explicit HLD(int n)
        : n(n),
          graph(n),
          parent(n, -1),
          depth(n, 0),
          size(n, 1),
          heavy(n, -1),
          head(n, -1),
          in(n, -1),
          out(n, -1),
          rev(n, -1) {}

    void add_edge(int a, int b) {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);

        graph[a].push_back(b);
        graph[b].push_back(a);
    }

    void build(int root = 0) {
        assert(0 <= root && root < n);

        fill(parent.begin(), parent.end(), -2);
        fill(depth.begin(), depth.end(), 0);
        fill(size.begin(), size.end(), 1);
        fill(heavy.begin(), heavy.end(), -1);
        fill(head.begin(), head.end(), -1);
        fill(in.begin(), in.end(), -1);
        fill(out.begin(), out.end(), -1);
        fill(rev.begin(), rev.end(), -1);

        vector<int> order;
        order.reserve(n);

        vector<int> st = {root};
        parent[root] = -1;

        while (!st.empty()) {
            int v = st.back();
            st.pop_back();

            order.push_back(v);

            for (int to : graph[v]) {
                if (to == parent[v]) continue;
                if (parent[to] != -2) continue;

                parent[to] = v;
                depth[to] = depth[v] + 1;
                st.push_back(to);
            }
        }

        for (int i = (int)order.size() - 1; i >= 0; i--) {
            int v = order[i];

            int best_size = 0;

            for (int to : graph[v]) {
                if (parent[to] != v) continue;

                size[v] += size[to];

                if (size[to] > best_size) {
                    best_size = size[to];
                    heavy[v] = to;
                }
            }
        }

        int timer = 0;

        vector<pair<int, int>> chains;
        chains.push_back({root, root});

        while (!chains.empty()) {
            auto [start, h] = chains.back();
            chains.pop_back();

            for (int v = start; v != -1; v = heavy[v]) {
                head[v] = h;
                in[v] = timer;
                rev[timer] = v;
                timer++;

                for (int to : graph[v]) {
                    if (parent[to] != v) continue;
                    if (to == heavy[v]) continue;

                    chains.push_back({to, to});
                }
            }
        }

        for (int v = 0; v < n; v++) {
            out[v] = in[v] + size[v];
        }
    }

    int lca(int a, int b) const {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);

        while (head[a] != head[b]) {
            if (depth[head[a]] < depth[head[b]]) {
                swap(a, b);
            }

            a = parent[head[a]];
        }

        return depth[a] < depth[b] ? a : b;
    }

    int dist_edges(int a, int b) const {
        int c = lca(a, b);

        return depth[a] + depth[b] - 2 * depth[c];
    }

    int kth_ancestor(int v, int k) const {
        assert(0 <= v && v < n);

        if (k < 0 || k > depth[v]) return -1;

        while (true) {
            int h = head[v];
            int d = depth[v] - depth[h];

            if (k <= d) {
                return rev[in[v] - k];
            }

            k -= d + 1;
            v = parent[h];
        }
    }

    int jump(int a, int b, int k) const {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);

        int c = lca(a, b);

        int da = depth[a] - depth[c];
        int db = depth[b] - depth[c];
        int d = da + db;

        if (k < 0 || k > d) return -1;

        if (k <= da) {
            return kth_ancestor(a, k);
        }

        return kth_ancestor(b, d - k);
    }

    bool is_ancestor(int a, int b) const {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);

        return in[a] <= in[b] && in[b] < out[a];
    }

    pair<int, int> subtree(int v, bool edge = false) const {
        assert(0 <= v && v < n);

        return {in[v] + (edge ? 1 : 0), out[v]};
    }

    vector<pair<int, int>> path(int a, int b, bool edge = false) const {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);

        vector<pair<int, int>> result;

        while (head[a] != head[b]) {
            if (depth[head[a]] < depth[head[b]]) {
                swap(a, b);
            }

            result.push_back({
                in[head[a]],
                in[a] + 1
            });

            a = parent[head[a]];
        }

        if (depth[a] < depth[b]) {
            swap(a, b);
        }

        int l = in[b] + (edge ? 1 : 0);
        int r = in[a] + 1;

        if (l < r) {
            result.push_back({l, r});
        }

        return result;
    }

    vector<tuple<int, int, bool>> path_ordered(
        int a,
        int b,
        bool edge = false
    ) const {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);

        vector<tuple<int, int, bool>> left;
        vector<tuple<int, int, bool>> right;

        while (head[a] != head[b]) {
            if (depth[head[a]] >= depth[head[b]]) {
                left.push_back({
                    in[head[a]],
                    in[a] + 1,
                    true
                });

                a = parent[head[a]];
            } else {
                right.push_back({
                    in[head[b]],
                    in[b] + 1,
                    false
                });

                b = parent[head[b]];
            }
        }

        if (depth[a] >= depth[b]) {
            int l = in[b] + (edge ? 1 : 0);
            int r = in[a] + 1;

            if (l < r) {
                left.push_back({l, r, true});
            }
        } else {
            int l = in[a] + (edge ? 1 : 0);
            int r = in[b] + 1;

            if (l < r) {
                right.push_back({l, r, false});
            }
        }

        reverse(right.begin(), right.end());

        left.insert(
            left.end(),
            right.begin(),
            right.end()
        );

        return left;
    }
};

/*
0-indexed

HLD hld(N);                                     // N 頂点で初期化 O(N)

hld.add_edge(a,b);                             // 無向辺 a-b を追加 O(1)
hld.build(root);                               // root を根として HL 分解 O(N)

hld.lca(a,b);                                  // a,b の最小共通祖先 O(log N)
hld.dist_edges(a,b);                           // a-b 間の辺数 O(log N)
hld.kth_ancestor(v,k);                         // v から k 個上の祖先、存在しなければ -1 O(log N)
hld.jump(a,b,k);                               // a→b のパスを k 辺進んだ頂点、存在しなければ -1 O(log N)
hld.is_ancestor(a,b);                          // a が b の祖先なら true O(1)

hld.in[v];                                     // v の Euler Tour 上の位置 O(1)
hld.out[v];                                    // v の部分木区間の右端 O(1)
hld.rev[i];                                    // Euler Tour 上の位置 i に対応する頂点 O(1)
hld.parent[v];                                 // v の親、root は -1 O(1)
hld.depth[v];                                  // root から v までの辺数 O(1)
hld.size[v];                                   // v の部分木サイズ O(1)
hld.head[v];                                   // v が属する heavy path の先頭 O(1)

auto [l,r] = hld.subtree(v);                   // v の部分木の頂点区間 [l,r) O(1)
auto [l,r] = hld.subtree(v,true);              // v の部分木に完全に含まれる辺区間 [l,r) O(1)

for(auto [l,r] : hld.path(a,b)){               // a-b パスを O(log N) 個の頂点区間 [l,r) に分解
    // seg.prod(l,r) など
}

for(auto [l,r] : hld.path(a,b,true)){          // a-b パスを辺区間として分解
    // 各辺の値を「深い側の頂点」に持たせる
}

// path() は区間の順序を保証しないため、和・min・max・xor など可換な演算向け

for(auto [l,r,reversed] : hld.path_ordered(a,b)){
    // a→b の順番で区間を取得
    // reversed=false : l→r-1 の順
    // reversed=true  : r-1→l の順
}

// path_ordered() は文字列結合・行列積など非可換演算にも利用可能
*/