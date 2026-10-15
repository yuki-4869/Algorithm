//LowLink

struct LowLink {
    int n;
    vector<pair<int, int>> edges;
    vector<vector<pair<int, int>>> graph;

    vector<int> ord;
    vector<int> low;
    vector<int> parent;
    vector<int> parent_edge;

    vector<bool> articulation;
    vector<bool> bridge;

    vector<int> articulation_points;
    vector<int> bridge_ids;
    vector<pair<int, int>> bridges;

    vector<pair<int, int>> dfs_edges;
    vl dfs_edge_ids;

    int components;

    LowLink() : n(0) {}

    explicit LowLink(int n)
        : n(n),
          graph(n) {}

    int add_edge(int a, int b) {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);

        int id = edges.size();

        edges.push_back({a, b});

        graph[a].push_back({b, id});
        graph[b].push_back({a, id});

        return id;
    }

    void build() {
        int m = edges.size();

        ord.assign(n, -1);
        low.assign(n, -1);
        parent.assign(n, -1);
        parent_edge.assign(n, -1);

        articulation.assign(n, false);
        bridge.assign(m, false);

        articulation_points.clear();
        bridge_ids.clear();
        bridges.clear();

        dfs_edges.clear();
        dfs_edge_ids.clear();

        components = 0;

        vector<int> iter(n, 0);
        vector<int> child_count(n, 0);

        int timer = 0;

        for (int root = 0; root < n; root++) {
            if (ord[root] != -1) continue;

            components++;

            vector<int> st;

            ord[root] = low[root] = timer++;
            st.push_back(root);

            while (!st.empty()) {
                int v = st.back();

                if (iter[v] < (int)graph[v].size()) {
                    auto [to, id] = graph[v][iter[v]++];

                    if (id == parent_edge[v]) continue;

                    if (ord[to] == -1) {
                        parent[to] = v;
                        parent_edge[to] = id;
                        child_count[v]++;

                        dfs_edges.push_back({v, to});
                        dfs_edge_ids.push_back(id);

                        ord[to] = low[to] = timer++;

                        st.push_back(to);
                    } else {
                        low[v] = min(low[v], ord[to]);
                    }

                    continue;
                }

                st.pop_back();

                int p = parent[v];

                if (p == -1) {
                    if (child_count[v] >= 2) {
                        articulation[v] = true;
                    }

                    continue;
                }

                low[p] = min(low[p], low[v]);

                if (parent[p] != -1 && ord[p] <= low[v]) {
                    articulation[p] = true;
                }

                if (ord[p] < low[v]) {
                    int id = parent_edge[v];
                    bridge[id] = true;
                }
            }
        }

        for (int v = 0; v < n; v++) {
            if (articulation[v]) {
                articulation_points.push_back(v);
            }
        }

        for (int id = 0; id < m; id++) {
            if (!bridge[id]) continue;

            bridge_ids.push_back(id);

            auto [a, b] = edges[id];

            if (a > b) swap(a, b);

            bridges.push_back({a, b});
        }
    }

    vl all_ord() const {
        return vl(ord.begin(), ord.end());
    }

    vl all_low() const {
        return vl(low.begin(), low.end());
    }

    int connected_components() const {
        return components;
    }

    bool is_articulation(int v) const {
        assert(0 <= v && v < n);

        return articulation[v];
    }

    bool is_bridge(int id) const {
        assert(0 <= id && id < (int)edges.size());

        return bridge[id];
    }
};

/*
0-indexed
非連結グラフ・多重辺・自己ループ対応

LowLink lowlink(N);                            // N 頂点で初期化 O(N)

int id = lowlink.add_edge(a,b);                // 無向辺 a-b を追加し辺番号を返す O(1)

lowlink.build();                               // LowLink を構築 O(N+M)

lowlink.ord[v];                                // v の DFS 訪問順 O(1)
lowlink.low[v];                                // v から後退辺を高々1本使って到達可能な最小 ord O(1)

lowlink.all_ord();                             // ord の配列を vl で返す O(N)
lowlink.all_low();                             // low の配列を vl で返す O(N)

lowlink.connected_components();                // 連結成分数を返す O(1)

lowlink.is_articulation(v);                    // v が関節点なら true O(1)
lowlink.is_bridge(id);                         // 辺 id が橋なら true O(1)

lowlink.articulation_points;                   // 全関節点の vector<int>
lowlink.bridge_ids;                            // 全ての橋の辺番号の vector<int>
lowlink.bridges;                               // 全ての橋 {min(a,b),max(a,b)} の vector<pair<int,int>>

lowlink.dfs_edges;                             // 全ての DFS 木の辺 {親,子}
lowlink.dfs_edge_ids;                          // 全ての DFS 木の辺番号を vl で取得

lowlink.edges[id];                             // 辺 id の両端 {a,b} O(1)
*/