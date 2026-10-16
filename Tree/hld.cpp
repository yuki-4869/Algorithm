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
        do { if (!(0 <= a && a < n)) std::abort(); } while (false);
        do { if (!(0 <= b && b < n)) std::abort(); } while (false);

        graph[a].push_back(b);
        graph[b].push_back(a);
    }

    void build(int root = 0) {
        do { if (!(0 <= root && root < n)) std::abort(); } while (false);

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
        do { if (!(0 <= a && a < n)) std::abort(); } while (false);
        do { if (!(0 <= b && b < n)) std::abort(); } while (false);

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
        do { if (!(0 <= v && v < n)) std::abort(); } while (false);

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
        do { if (!(0 <= a && a < n)) std::abort(); } while (false);
        do { if (!(0 <= b && b < n)) std::abort(); } while (false);

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
        do { if (!(0 <= a && a < n)) std::abort(); } while (false);
        do { if (!(0 <= b && b < n)) std::abort(); } while (false);

        return in[a] <= in[b] && in[b] < out[a];
    }

    pair<int, int> subtree(int v, bool edge = false) const {
        do { if (!(0 <= v && v < n)) std::abort(); } while (false);

        return {in[v] + (edge ? 1 : 0), out[v]};
    }

    vector<pair<int, int>> path(int a, int b, bool edge = false) const {
        do { if (!(0 <= a && a < n)) std::abort(); } while (false);
        do { if (!(0 <= b && b < n)) std::abort(); } while (false);

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
        do { if (!(0 <= a && a < n)) std::abort(); } while (false);
        do { if (!(0 <= b && b < n)) std::abort(); } while (false);

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
細かい注意点
0-indexed、無向木。全辺をadd_edgeした後にbuild(root)を呼ぶ。
部分木・パスの各区間はEuler Tour上の半開区間[l,r)。
辺の値は深い側の頂点位置に置く。path(...,true)/subtree(...,true)で辺区間を取得する。
pathは列挙順を保証せず、和/min/max/xorなど可換演算向け。
path_orderedはa->b順で列挙し、reversed=trueの区間はr-1からlへ読む。
返すパス区間数はO(log N)。非可換演算では向きも反映する。

使える関数一覧
HLD hld(N);                         // 初期化 O(N)
hld.add_edge(a,b);                  // void: 無向辺を追加 O(1)
hld.build(root=0);                  // void: HL分解 O(N)
hld.lca(a,b);                       // int: LCA O(log N)
hld.dist_edges(a,b);                // int: 辺数 O(log N)
hld.kth_ancestor(v,k);              // int: k個上の祖先、なければ-1 O(log N)
hld.jump(a,b,k);                    // int: a->bをk辺進む、なければ-1 O(log N)
hld.is_ancestor(a,b);               // bool: aがbの祖先か O(1)
hld.subtree(v,edge=false);          // pair<int,int>: 部分木区間 O(1)
hld.path(a,b,edge=false);           // vector<pair<int,int>>: パス区間 O(log N)
hld.path_ordered(a,b,edge=false);   // vector<tuple<int,int,bool>>: 向き付きパス O(log N)
hld.in[v], hld.out[v], hld.rev[i];   // Euler Tourの位置と逆引き O(1)
hld.parent[v], hld.depth[v], hld.size[v], hld.head[v]; // 根付き木・分解情報 O(1)

基本の関数
HLD hld(3);
hld.add_edge(0,1); hld.add_edge(1,2);
hld.build(0);
auto ancestor = hld.lca(0,2);                 // 0
auto intervals = hld.path(0,2);               // 頂点パスの区間列
auto [l,r] = hld.subtree(1);                  // 部分木の区間
*/
