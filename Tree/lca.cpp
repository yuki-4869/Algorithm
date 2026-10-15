//LCA


template<class T = long long>
struct LCA {
    int n;
    int log;
    vector<vector<pair<int, T>>> graph;
    vector<vector<int>> parent;
    vector<int> depth;
    vector<T> dist_root;

    LCA() : n(0), log(0) {}

    explicit LCA(int n)
        : n(n),
          log(1),
          graph(n),
          depth(n, -1),
          dist_root(n, T{}) {

        while ((1LL << log) <= n) log++;

        parent.assign(log, vector<int>(n, -1));
    }

    void add_edge(int a, int b, T w = T{1}) {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);

        graph[a].push_back({b, w});
        graph[b].push_back({a, w});
    }

    void build(int root = 0) {
        assert(0 <= root && root < n);

        fill(depth.begin(), depth.end(), -1);
        fill(dist_root.begin(), dist_root.end(), T{});

        for (int k = 0; k < log; k++) {
            fill(parent[k].begin(), parent[k].end(), -1);
        }

        queue<int> que;

        depth[root] = 0;
        que.push(root);

        while (!que.empty()) {
            int v = que.front();
            que.pop();

            for (auto [to, w] : graph[v]) {
                if (depth[to] != -1) continue;

                depth[to] = depth[v] + 1;
                dist_root[to] = dist_root[v] + w;
                parent[0][to] = v;

                que.push(to);
            }
        }

        for (int k = 0; k + 1 < log; k++) {
            for (int v = 0; v < n; v++) {
                if (parent[k][v] == -1) continue;

                parent[k + 1][v] = parent[k][parent[k][v]];
            }
        }
    }

    int kth_ancestor(int v, long long k) const {
        assert(0 <= v && v < n);

        if (k < 0 || k > depth[v]) return -1;

        for (int i = 0; i < log; i++) {
            if ((k >> i) & 1LL) {
                v = parent[i][v];
            }
        }

        return v;
    }

    int lca(int a, int b) const {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);
        assert(depth[a] != -1);
        assert(depth[b] != -1);

        if (depth[a] < depth[b]) swap(a, b);

        a = kth_ancestor(a, depth[a] - depth[b]);

        if (a == b) return a;

        for (int k = log - 1; k >= 0; k--) {
            if (parent[k][a] == parent[k][b]) continue;

            a = parent[k][a];
            b = parent[k][b];
        }

        return parent[0][a];
    }

    int dist_edges(int a, int b) const {
        int c = lca(a, b);

        return depth[a] + depth[b] - 2 * depth[c];
    }

    T dist(int a, int b) const {
        int c = lca(a, b);

        return dist_root[a] + dist_root[b] - T{2} * dist_root[c];
    }

    int jump(int a, int b, long long k) const {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);

        int c = lca(a, b);

        long long da = depth[a] - depth[c];
        long long db = depth[b] - depth[c];
        long long d = da + db;

        if (k < 0 || k > d) return -1;

        if (k <= da) {
            return kth_ancestor(a, k);
        }

        return kth_ancestor(b, d - k);
    }

    bool is_ancestor(int a, int b) const {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);

        return lca(a, b) == a;
    }
};

/*
LCA<ll> lca(N);                                // N 頂点で初期化 O(N log N) 0-index

lca.add_edge(a,b);                             // 無重み辺 a-b を追加 O(1)
lca.add_edge(a,b,w);                           // 重み w の辺 a-b を追加 O(1)
lca.build(root);                               // root を根として前計算 O(N log N)

lca.lca(a,b);                                  // a,b の最小共通祖先を返す O(log N)
lca.kth_ancestor(v,k);                         // v から k 個上の祖先、存在しなければ -1 O(log N)

lca.dist_edges(a,b);                           // a-b 間の辺数 O(log N)
lca.dist(a,b);                                 // a-b 間の辺重みの総和 O(log N)

lca.jump(a,b,k);                               // a から b へのパスを k 辺進んだ頂点 O(log N)
                                                // k=0 なら a、k=dist_edges(a,b) なら b
                                                // パスの長さを超えるなら -1

lca.is_ancestor(a,b);                          // a が b の祖先なら true O(log N)

lca.depth[v];                                  // root から v までの辺数 O(1)
lca.dist_root[v];                              // root から v までの重みの総和 O(1)
*/