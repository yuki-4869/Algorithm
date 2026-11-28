struct ManhattanMSTResult {
    __int128 weight = 0;
    std::vector<std::pair<int,int>> edges;
};

ManhattanMSTResult manhattan_mst(const std::vector<std::pair<long long,long long>>& points) {
    if (points.size() > static_cast<std::size_t>(INT_MAX)) std::abort();
    const int n = static_cast<int>(points.size());
    std::vector<std::pair<__int128,__int128>> rotated(points.begin(), points.end());
    std::vector<std::tuple<__int128,int,int>> candidates;
    std::vector<int> order(n);
    std::iota(order.begin(), order.end(), 0);
    for (int direction = 0; direction < 4; ++direction) {
        std::sort(order.begin(), order.end(), [&](int u, int v) {
            auto [xu,yu] = rotated[u];
            auto [xv,yv] = rotated[v];
            return std::pair{xu+yu,u} < std::pair{xv+yv,v};
        });
        std::map<__int128,int> sweep;
        for (int u : order) {
            auto [xu,yu] = rotated[u];
            auto it = sweep.lower_bound(-yu);
            while (it != sweep.end()) {
                const int v = it->second;
                auto [xv,yv] = rotated[v];
                if (xu-xv < yu-yv) break;
                candidates.emplace_back(xu-xv+yu-yv,u,v);
                it = sweep.erase(it);
            }
            sweep[-yu] = u;
        }
        for (auto& [x,y] : rotated) {
            if (direction & 1) x = -x;
            else std::swap(x,y);
        }
    }
    std::sort(candidates.begin(), candidates.end());
    atcoder::dsu components(n);
    ManhattanMSTResult result;
    if (n > 0) result.edges.reserve(n-1);
    for (auto [cost,u,v] : candidates) {
        if (components.same(u,v)) continue;
        components.merge(u,v);
        result.weight += cost;
        result.edges.emplace_back(u,v);
    }
    return result;
}

/*
細かい注意点
頂点番号は入力順の0-indexed。重複座標・負座標にも対応。空入力の重みは0。
座標はlong long。座標変換・辺の重み・総和は__int128で扱う。点数はINT_MAX以下。
codecfではatcoder/dsuを追加で読む。

使える関数一覧
manhattan_mst(points); // ManhattanMSTResult: 最小全域木を構築 O(N log(N+1)) 時間・O(N) 領域
result.weight;         // __int128: 木の総重み O(1)
result.edges;          // vector<pair<int,int>>: 採用した端点の組 O(1) 参照

基本の関数
auto result=manhattan_mst({{0,0},{2,0},{2,1}});
if(result.weight!=3 || result.edges.size()!=2)std::abort();
*/
