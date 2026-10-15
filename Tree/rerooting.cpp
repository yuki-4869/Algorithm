// 全方位木DP（Rerooting）

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
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);

        int id = edge_count++;

        graph[a].push_back({b, id, data});
        graph[b].push_back({a, id, data});

        return id;
    }

    vector<DP> build(int root = 0) {
        assert(0 <= root && root < n);
        assert(edge_count == n - 1);

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

        assert((int)order.size() == n);

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
0-indexed
木専用

問題ごとに以下を定義する
*/


struct DP {

};// ある1方向から来る情報の型


struct E {

};// 辺が持つ情報の型
 // 重み付き木なら ll w; など


DP merge(DP a, DP b){
    DP ret;

    return ret;
}// 複数方向から来るDPを結合する関数
 // 結合則を満たす必要がある


DP identity(){
    return ;
}// merge の単位元
 // 「どの方向からも情報が来ていない状態」


DP lift(DP x, E e, int from, int to){
    DP ret;

    return ret;
}// from 側のDP xを
 // 辺eを通ってto側から見た情報へ変換する関数


DP add_vertex(DP x, int v){
    DP ret;

    return ret;
}// 全方向から来た情報をmergeした後
 // 頂点v自身の情報を加える関数


/*
使用例

auto rr = make_rerooting<DP,E>(
    N,
    identity(),
    merge,
    lift,
    add_vertex
);

for(int i=0;i<N-1;i++){
    int u,v;
    cin >> u >> v;
    --u;
    --v;

    E e;

    // 必要なら辺情報を入力
    // cin >> e.w;

    rr.add_edge(u,v,e);
}

vector<DP> ans = rr.build(0);

ans[v];                                        // v を根としたときの答え
rr.answer[v];                                  // ans[v] と同じ
rr.subdp[v];                                   // build(root) における v の部分木側だけのDP


DP                                              // 1方向について何の情報を持つか
E                                               // 辺について何の情報を持つか

identity()                                      // 何もない状態
merge(a,b)                                      // 2方向の情報を結合
lift(x,e,from,to)                               // 辺を1本越えたときの変換
add_vertex(x,v)                                 // 頂点v自身を加える

rr.add_edge(u,v,e);                             // 辺u-vを追加 O(1)
rr.build(root);                                 // 全頂点を根としたDPを計算 O(N)

※ merge / lift / add_vertex が O(1) の場合、全体 O(N)
※ 再帰を使わない
※ root は内部計算用で、answer[v] の意味には影響しない
*/