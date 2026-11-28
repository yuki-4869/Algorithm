namespace ahc {

template<class Distance>
std::vector<int> nearest_neighbor_tour(int n, int start, Distance&& distance) {
    if (n < 0 || (n == 0 ? start != 0 : (start < 0 || start >= n))) std::abort();
    if (n == 0) return {};
    using Cost = std::remove_cvref_t<std::invoke_result_t<Distance&, int, int>>;
    std::vector<int> tour;
    tour.reserve(n);
    std::vector<char> visited(n, false);
    tour.push_back(start);
    visited[start] = true;
    for (int i = 1; i < n; ++i) {
        int next = -1;
        Cost best_cost = 0;
        const int last = tour.back();
        for (int vertex = 0; vertex < n; ++vertex) {
            if (visited[vertex]) continue;
            const Cost cost = std::invoke(distance, last, vertex);
            if (next == -1 || cost < best_cost) {
                next = vertex;
                best_cost = cost;
            }
        }
        visited[next] = true;
        tour.push_back(next);
    }
    return tour;
}

template<class Distance>
auto cycle_cost(std::span<const int> tour, Distance&& distance) {
    using Cost = std::remove_cvref_t<std::invoke_result_t<Distance&, int, int>>;
    Cost total = 0;
    if (tour.size() < 2) return total;
    int previous = tour.back();
    for (int vertex : tour) {
        total += std::invoke(distance, previous, vertex);
        previous = vertex;
    }
    return total;
}

template<class Distance>
auto two_opt_delta(std::span<const int> tour, std::size_t left, std::size_t right, Distance&& distance) {
    using Cost = std::remove_cvref_t<std::invoke_result_t<Distance&, int, int>>;
    if (left > right || right > tour.size()) std::abort();
    if (right - left < 2 || right - left == tour.size()) return Cost(0);
    const int before = tour[left == 0 ? tour.size() - 1 : left - 1];
    const int after = tour[right == tour.size() ? 0 : right];
    const int first = tour[left];
    const int last = tour[right - 1];
    const Cost removed = std::invoke(distance, before, first) + std::invoke(distance, last, after);
    const Cost added = std::invoke(distance, before, last) + std::invoke(distance, first, after);
    return Cost(added - removed);
}

template<class Distance>
bool two_opt_first_improvement(
    std::span<int> tour, Distance&& distance,
    std::remove_cvref_t<std::invoke_result_t<Distance&, int, int>> epsilon = 0
) {
    if (!(epsilon >= 0)) std::abort();
    for (std::size_t left = 1; left + 1 < tour.size(); ++left) {
        for (std::size_t right = left + 2; right <= tour.size(); ++right) {
            const auto delta = two_opt_delta(tour, left, right, distance);
            if (delta < -epsilon) {
                std::reverse(tour.begin() + left, tour.begin() + right);
                return true;
            }
        }
    }
    return false;
}

}

/*
細かい注意点
- 参照URL https://github.com/yuki-4869/Algorithm/blob/main/Heuristic/two_opt.cpp
- 巡回路は頂点列。末尾に始点を重複させず、閉路の最後の辺は自動で数える。
- 2-optは対称コストのみ。反転位置は0-indexed半開区間 [left,right)。
- コスト・差分・中間和には十分広い符号付き型を使う。浮動小数は有限値を使う。
- first_improvementは先頭を固定し、改善を1つ反映する。epsilonは非負。
- 最近傍法は同距離なら小さい頂点番号を選ぶ。n=0ならstart=0。

使える関数一覧
ahc::nearest_neighbor_tour(n,start,distance)    最近傍法で初期巡回路を作る    O(N²)
ahc::cycle_cost(tour,distance)                   末尾→先頭込みの合計          O(N)
ahc::two_opt_delta(tour,l,r,distance)           反転による変更後-変更前      O(1)
ahc::two_opt_first_improvement(tour,distance,epsilon=0)
                                               改善手を1つ反映してtrue      最悪O(N²)
distance(u,v)がO(1)の場合。自分で反転を適用するとO(r-l)。

基本の関数
vector<pair<int,int>> points{{0,0},{4,0},{0,4},{4,4}};
auto distance = [&](int u,int v) -> long long {
    return llabs(1LL*points[u].first-points[v].first)
         + llabs(1LL*points[u].second-points[v].second);
};
auto tour = ahc::nearest_neighbor_tour(points.size(),0,distance);
while (ahc::two_opt_first_improvement(tour,distance)) {}
auto cost = ahc::cycle_cost(tour,distance);
*/
