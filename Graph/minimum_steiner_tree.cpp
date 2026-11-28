struct SteinerTreeResult {
    long long weight = 0;
    std::vector<int> edge_ids;
};

std::optional<SteinerTreeResult> minimum_steiner_tree(
        int n, const std::vector<std::tuple<int,int,long long>>& edges,
        std::vector<int> terminals) {
    if (n < 0 || edges.size() > static_cast<std::size_t>(INT_MAX)) std::abort();
    std::sort(terminals.begin(),terminals.end());
    terminals.erase(std::unique(terminals.begin(),terminals.end()),terminals.end());
    for (int v : terminals) if (v < 0 || v >= n) std::abort();
    if (terminals.size() > 20) std::abort();
    std::vector<std::vector<std::pair<int,int>>> adj(n);
    for (int i = 0; i < static_cast<int>(edges.size()); ++i) {
        auto [u,v,w] = edges[i];
        if (u < 0 || v < 0 || u >= n || v >= n || w < 0) std::abort();
        adj[u].emplace_back(v,i);
        adj[v].emplace_back(u,i);
    }
    if (terminals.size() <= 1) return SteinerTreeResult{};

    constexpr long long infinity = LLONG_MAX;
    const int masks = 1 << terminals.size();
    std::vector<std::vector<long long>> dist(masks,std::vector<long long>(n,infinity));
    std::vector<std::vector<int>> trace(masks,std::vector<int>(n));
    for (int i = 0; i < static_cast<int>(terminals.size()); ++i) dist[1<<i][terminals[i]] = 0;
    for (int mask = 1; mask < masks; ++mask) {
        auto& costs = dist[mask];
        auto& prev = trace[mask];
        for (int a = (mask-1)&mask; a != 0; a = (a-1)&mask) {
            const int b = mask^a;
            if (a > b) continue;
            for (int v = 0; v < n; ++v) {
                const auto x = dist[a][v], y = dist[b][v];
                if (x < costs[v] && y < costs[v]-x) {
                    costs[v] = x+y;
                    prev[v] = a;
                }
            }
        }
        using State = std::pair<long long,int>;
        std::priority_queue<State,std::vector<State>,std::greater<State>> queue;
        for (int v = 0; v < n; ++v) if (costs[v] != infinity) queue.emplace(costs[v],v);
        while (!queue.empty()) {
            const auto [d,u] = queue.top();
            queue.pop();
            if (d != costs[u]) continue;
            for (auto [v,id] : adj[u]) {
                const auto w = std::get<2>(edges[id]);
                if (d < costs[v] && w < costs[v]-d) {
                    costs[v] = d+w;
                    prev[v] = -1-id;
                    queue.emplace(costs[v],v);
                }
            }
        }
    }

    const auto best = std::min_element(dist.back().begin(),dist.back().end());
    if (*best == infinity) return std::nullopt;
    std::vector<std::pair<int,int>> pending{{masks-1,static_cast<int>(best-dist.back().begin())}};
    std::vector<std::vector<bool>> used(masks,std::vector<bool>(n));
    atcoder::dsu components(n);
    SteinerTreeResult result{*best,{}};
    while (!pending.empty()) {
        const auto [mask,v] = pending.back();
        pending.pop_back();
        if (used[mask][v]) continue;
        used[mask][v] = true;
        const int from = trace[mask][v];
        if (from > 0) {
            pending.emplace_back(from,v);
            pending.emplace_back(mask^from,v);
        } else if (from < 0) {
            const int id = -1-from;
            const auto [a,b,w] = edges[id];
            if (!components.same(a,b)) {
                components.merge(a,b);
                result.edge_ids.push_back(id);
            }
            pending.emplace_back(mask,a^b^v);
        }
    }
    return result;
}

/*
細かい注意点
0始まりの無向グラフ。辺は(a,b,非負重み)、順番が辺ID。Kは重複除外後の指定頂点数。
K<=20。計算量・領域はKに対し指数的なので、実際の使える上限はNとMによる。
解の重みがLLONG_MAX以上、または指定頂点を結べない場合はnullopt。K<=1では重み0。
codecfではatcoder/dsuを追加。

使える関数一覧
minimum_steiner_tree(n,edges,terminals); // optional<SteinerTreeResult>: 最小重みで指定頂点を結ぶ木
                                        // O(3^K N+2^K (N+M) log(N+M+2))、領域O(2^K N+M)
result->weight;                         // long long: 最小重み
result->edge_ids;                       // vector<int>: 採用した入力辺番号

基本の関数
auto result=minimum_steiner_tree(4,{{0,3,1},{1,3,1},{2,3,1},{0,1,9}}, {0,1,2});
if(!result || result->weight!=3 || result->edge_ids.size()!=3)std::abort();
*/
