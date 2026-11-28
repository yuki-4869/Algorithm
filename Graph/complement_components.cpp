std::vector<std::vector<int>> complement_components(int n, const std::vector<std::pair<int,int>>& edges) {
    if (n < 0) std::abort();
    std::vector<std::vector<int>> adj(n);
    for (auto [u,v] : edges) {
        if (u < 0 || v < 0 || u >= n || v >= n) std::abort();
        if (u != v) {
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }
    std::vector<int> next(n), blocked(n,-1);
    std::iota(next.begin(),next.end(),1);
    int head = 0;
    std::vector<std::vector<int>> result;
    while (head != n) {
        result.push_back({head});
        head = next[head];
        auto& component = result.back();
        for (std::size_t i = 0; i < component.size(); ++i) {
            const int u = component[i];
            for (int v : adj[u]) blocked[v] = u;
            int* link = &head;
            while (*link != n) {
                const int v = *link;
                if (blocked[v] == u) link = &next[v];
                else {
                    *link = next[v];
                    component.push_back(v);
                }
            }
        }
    }
    return result;
}

/*
細かい注意点
頂点は0始まり。入力は元の無向グラフの辺。自己ループは無視し、多重辺も許す。
補グラフを明示的に構築せず、未確定頂点を連結リストで管理する。

使える関数一覧
complement_components(n,edges); // vector<vector<int>>: 補グラフの各連結成分 O(N+M) 時間・領域

基本の関数
auto components=complement_components(3,{{0,1},{0,2}});
if(components!=std::vector<std::vector<int>>{{0},{1,2}})std::abort();
*/
