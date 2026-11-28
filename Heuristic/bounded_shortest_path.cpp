namespace ahc {

struct BoundedShortestPaths {
    std::vector<long long> distance;
    std::vector<int> parent;

    std::optional<std::vector<int>> path(int target) const {
        if (target < 0 || std::size_t(target) >= distance.size()) std::abort();
        if (distance[target] < 0) return std::nullopt;
        std::vector<int> result;
        for (int vertex = target; vertex != -1; vertex = parent[vertex]) result.push_back(vertex);
        std::reverse(result.begin(), result.end());
        return result;
    }
};

namespace shortest_path_detail {

inline void radix(
    const std::vector<std::vector<std::pair<int,int>>>& edges, std::span<const int> starts,
    int max_edge, BoundedShortestPaths& result
) {
    using Item = std::pair<long long,int>;
    std::array<std::vector<Item>,64> buckets;
    std::uint64_t occupied = 0;
    long long current = 0;
    const int n = int(edges.size());
    auto push = [&](int vertex, long long distance) {
        const auto slot = std::bit_width(std::uint64_t(current ^ distance));
        buckets[slot].emplace_back(distance, vertex);
        occupied |= std::uint64_t(1) << slot;
    };
    for (int source : starts) {
        if (source < 0 || source >= n) std::abort();
        if (result.distance[source] == 0) continue;
        result.distance[source] = 0;
        push(source, 0);
    }
    while (occupied != 0) {
        if (buckets[0].empty()) {
            const int slot = std::countr_zero(occupied);
            auto& source = buckets[slot];
            current = std::min_element(source.begin(), source.end())->first;
            for (const auto& [distance, vertex] : source) push(vertex, distance);
            source.clear();
            occupied &= ~(std::uint64_t(1) << slot);
        }
        const auto [current_distance, vertex] = buckets[0].back();
        buckets[0].pop_back();
        if (buckets[0].empty()) occupied &= ~std::uint64_t(1);
        if (result.distance[vertex] != current_distance) continue;
        for (const auto& [to, cost] : edges[vertex]) {
            if (to < 0 || to >= n || cost < 0 || cost > max_edge) std::abort();
            const long long distance = current_distance + cost;
            if (result.distance[to] >= 0 && result.distance[to] <= distance) continue;
            result.distance[to] = distance;
            result.parent[to] = vertex;
            push(to, distance);
        }
    }
}

}

BoundedShortestPaths dial_shortest_paths(
    const std::vector<std::vector<std::pair<int,int>>>& edges,
    std::span<const int> starts, int max_edge
) {
    if (max_edge < 0 || edges.size() > INT_MAX) std::abort();
    const int n = int(edges.size());
    BoundedShortestPaths result{std::vector<long long>(n, -1), std::vector<int>(n, -1)};
    if (starts.empty()) return result;
    if (n == 0) std::abort();
    constexpr int radix_threshold = 4096;
    if (max_edge >= radix_threshold) {
        int largest = 0;
        for (const auto& row : edges) {
            for (const auto& edge : row) largest = std::max(largest, edge.second);
            if (largest >= radix_threshold) {
                shortest_path_detail::radix(edges, starts, max_edge, result);
                return result;
            }
        }
        max_edge = largest;
    }
    const std::size_t bucket_count = std::size_t(max_edge) + 1;
    std::vector<std::vector<int>> buckets(bucket_count);
    std::size_t remaining = 0, slot = 0;
    long long current = 0;
    for (int source : starts) {
        if (source < 0 || source >= n) std::abort();
        if (result.distance[source] == 0) continue;
        result.distance[source] = 0;
        buckets[0].push_back(source);
        ++remaining;
    }
    while (remaining != 0) {
        if (buckets[slot].empty()) {
            ++current;
            if (++slot == bucket_count) slot = 0;
            continue;
        }
        const int vertex = buckets[slot].back();
        buckets[slot].pop_back();
        --remaining;
        if (result.distance[vertex] != current) continue;
        for (const auto& [to, cost] : edges[vertex]) {
            if (to < 0 || to >= n || cost < 0 || cost > max_edge) std::abort();
            const long long distance = current + cost;
            if (result.distance[to] >= 0 && result.distance[to] <= distance) continue;
            result.distance[to] = distance;
            result.parent[to] = vertex;
            std::size_t next_slot = slot + std::size_t(cost);
            if (next_slot >= bucket_count) next_slot -= bucket_count;
            buckets[next_slot].push_back(to);
            ++remaining;
        }
    }
    return result;
}

BoundedShortestPaths zero_one_bfs(
    const std::vector<std::vector<std::pair<int,int>>>& edges, std::span<const int> starts
) {
    return dial_shortest_paths(edges, starts, 1);
}

}

/*
細かい注意点
- 参照URL https://github.com/yuki-4869/Algorithm/blob/main/Heuristic/bounded_shortest_path.cpp
- 頂点は0-indexed。辺は (行き先,コスト) の有向辺。全コストは [0,max_edge] の整数。
- NはINT_MAX以下。コスト上限4096未満は循環バケット。上限が4096以上なら実際の辺の重みを確認し、4096以上の辺があればradix heapを使う。
- 距離に-1は未到達。parentに-1は始点または未到達。始点数をSとする。

使える関数一覧
ahc::dial_shortest_paths(edges,starts,max_edge)  多始点最短距離を計算  循環バケットO(N+M+S+NC)、radix heap O((N+M)B+S)
ahc::zero_one_bfs(edges,starts)                 辺重み0/1の最短距離  O(N+M+S)
result.distance, result.parent                 各頂点の距離・直前頂点
result.path(target)                            頂点列またはnullopt  O(1+経路長)
Cは循環バケットで使う重み上限。Bは距離のビット数に1を足した値（最大64）。
入力以外に循環バケットではO(N+M+C)領域を使う。radix heapは64 bit固定でO(N+M)領域。
同距離の経路の辞書順は指定しない。

基本の関数
vector<vector<pair<int,int>>> edges{{{1,1},{2,0}},{},{{1,0}}};
array<int,1> starts{0};
auto result = ahc::zero_one_bfs(edges,starts);
assert(result.distance[1] == 0);
assert(*result.path(1) == (vector<int>{0,2,1}));
*/
