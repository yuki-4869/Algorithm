template<
    class DP,
    class E,
    class Merge,
    class Lift,
    class AddVertex
>
struct Rerooting {
    struct Edge {
        int to;
        int id;
        E data;
    };

    int n;
    int edge_count;

    DP identity;
    Merge merge;
    Lift lift;
    AddVertex add_vertex;

    vector<vector<Edge>> graph;

    vector<int> parent;
    vector<int> parent_edge;
    vector<int> order;

    vector<DP> subdp;
    vector<DP> answer;

    Rerooting(
        int n,
        DP identity,
        Merge merge,
        Lift lift,
        AddVertex add_vertex
    )
        : n(n),
          edge_count(0),
          identity(identity),
          merge(merge),
          lift(lift),
          add_vertex(add_vertex),
          graph(n) {}

    int add_edge(int a, int b, const E& data) {
        do { if (!(0 <= a && a < n)) std::abort(); } while (false);
        do { if (!(0 <= b && b < n)) std::abort(); } while (false);

        int id = edge_count++;

        graph[a].push_back({b, id, data});
        graph[b].push_back({a, id, data});

        return id;
    }

    vector<DP> build(int root = 0) {
        do { if (!(0 <= root && root < n)) std::abort(); } while (false);
        do { if (!(edge_count == n - 1)) std::abort(); } while (false);

        parent.assign(n, -2);
        parent_edge.assign(n, -1);

        order.clear();
        order.reserve(n);

        vector<int> st = {root};
        parent[root] = -1;

        while (!st.empty()) {
            int v = st.back();
            st.pop_back();

            order.push_back(v);

            for (auto& e : graph[v]) {
                if (parent[e.to] != -2) continue;

                parent[e.to] = v;
                parent_edge[e.to] = e.id;

                st.push_back(e.to);
            }
        }

        do { if (!((int)order.size() == n)) std::abort(); } while (false);

        subdp.assign(n, identity);

        for (int oi = n - 1; oi >= 0; oi--) {
            int v = order[oi];

            DP value = identity;

            for (auto& e : graph[v]) {
                int to = e.to;

                if (parent[to] != v) continue;

                value = merge(
                    value,
                    lift(
                        subdp[to],
                        e.data,
                        to,
                        v
                    )
                );
            }

            subdp[v] = add_vertex(value, v);
        }

        vector<DP> up(n, identity);

        answer.assign(n, identity);

        for (int v : order) {
            int m = graph[v].size();

            vector<DP> value(m, identity);

            for (int i = 0; i < m; i++) {
                auto& e = graph[v][i];

                if (e.id == parent_edge[v]) {
                    value[i] = up[v];
                } else {
                    value[i] = lift(
                        subdp[e.to],
                        e.data,
                        e.to,
                        v
                    );
                }
            }

            vector<DP> prefix(m + 1, identity);
            vector<DP> suffix(m + 1, identity);

            for (int i = 0; i < m; i++) {
                prefix[i + 1] =
                    merge(prefix[i], value[i]);
            }

            for (int i = m - 1; i >= 0; i--) {
                suffix[i] =
                    merge(value[i], suffix[i + 1]);
            }

            answer[v] =
                add_vertex(prefix[m], v);

            for (int i = 0; i < m; i++) {
                auto& e = graph[v][i];

                if (parent[e.to] != v) continue;

                DP without_child = merge(
                    prefix[i],
                    suffix[i + 1]
                );

                DP from_v =
                    add_vertex(without_child, v);

                up[e.to] = lift(
                    from_v,
                    e.data,
                    v,
                    e.to
                );
            }
        }

        return answer;
    }
};

template<
    class DP,
    class E,
    class Merge,
    class Lift,
    class AddVertex
>
auto make_rerooting(
    int n,
    DP identity,
    Merge merge,
    Lift lift,
    AddVertex add_vertex
) {
    return Rerooting<
        DP,
        E,
        Merge,
        Lift,
        AddVertex
    >(
        n,
        identity,
        merge,
        lift,
        add_vertex
    );
}

/*
細かい注意点
0-indexed、連結な無向木。DP/Eと各演算は問題に合わせて指定する。
mergeは結合則を満たし、identityは単位元。liftはfrom側の情報を辺越しにto側へ渡す。
add_vertexは集約結果に頂点自身を反映する。各演算O(1)なら全体O(N)。
再帰を使わない。build(root)のrootは内部計算用で、answer[v]は常にvを根とした答え。
subdp[v]はbuildで指定した根に対する部分木側のDP。

使える関数一覧
make_rerooting<DP,E>(N,identity,merge,lift,add_vertex); // Rerootingを構築 O(N)
rr.add_edge(u,v,e);     // void: 辺情報eで無向辺を追加 O(1)
rr.build(root=0);       // vector<DP>: 全頂点を根にしたDP O(N)
rr.answer[v];          // DP: vを根とした答え O(1)
rr.subdp[v];           // DP: 内部の根に対する部分木側DP O(1)
merge(a,b);            // DP: 複数方向を結合する利用者定義の演算
lift(x,e,from,to);     // DP: 辺越しの変換を行う利用者定義の演算
add_vertex(x,v);       // DP: 頂点vを加える利用者定義の演算

基本の関数
auto rr = make_rerooting<ll,int>(3,0LL,
    [](ll a,ll b){return max(a,b);},
    [](ll x,int w,int from,int to){return x+w;},
    [](ll x,int v){return x;});
rr.add_edge(0,1,1); rr.add_edge(1,2,1);
auto answer = rr.build(0);                    // 各頂点からの最遠距離{2,1,2}
*/
