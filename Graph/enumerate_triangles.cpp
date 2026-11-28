template<class F>
void enumerate_triangles(int n, const std::vector<std::pair<int,int>>& edges, F&& visit) {
    if (n < 0) std::abort();
    std::vector<std::size_t> degree(n);
    for (auto [u,v] : edges) {
        if (u < 0 || v < 0 || u >= n || v >= n || u == v) std::abort();
        ++degree[u];
        ++degree[v];
    }
    std::vector<std::vector<int>> out(n);
    for (auto [u,v] : edges) {
        if (std::pair{degree[u],u} > std::pair{degree[v],v}) std::swap(u,v);
        out[u].push_back(v);
    }
    std::vector<int> mark(n,-1);
    for (int u = 0; u < n; ++u) {
        for (int v : out[u]) mark[v] = u;
        for (int v : out[u]) {
            for (int w : out[v]) {
                if (mark[w] == u) visit(u,v,w);
            }
        }
    }
}

/*
細かい注意点
0始まりの単純無向グラフ。各辺を1回だけ登録する。三角形内の順序は次数・頂点番号の昇順。
列挙全体の順序には依存しないこと。入力の辺を列挙中に変更しない。

使える関数一覧
enumerate_triangles(n,edges,visit); // void: 全三角形でvisit(u,v,w)を1回呼ぶ
                                  // 時間O(N+M sqrt(M+1))+コールバック、領域O(N+M)

基本の関数
int triangles=0;
enumerate_triangles(4,{{0,1},{1,2},{2,0},{2,3}},[&](int,int,int){++triangles;});
if(triangles!=1)std::abort();
*/
