namespace ahc {

template<class Distance>
auto tour_swap_delta(std::span<const int> tour, std::size_t i, std::size_t j, Distance&& distance) {
    using Cost = std::remove_cvref_t<std::invoke_result_t<Distance&, int, int>>;
    const std::size_t n = tour.size();
    if (i >= n || j >= n) std::abort();
    if (i == j) return Cost(0);
    if (n == 2) return Cost(0);
    const std::size_t prev_i = i == 0 ? n - 1 : i - 1;
    const std::size_t prev_j = j == 0 ? n - 1 : j - 1;
    const int u = tour[i], v = tour[j];
    const int left_i = tour[prev_i], right_i = tour[i + 1 == n ? 0 : i + 1];
    const int left_j = tour[prev_j], right_j = tour[j + 1 == n ? 0 : j + 1];
    Cost before = 0, after = 0;
    const auto count_edge = [&](int from, int to, int new_from, int new_to) {
        before += std::invoke(distance, from, to);
        after += std::invoke(distance, new_from, new_to);
    };
    if (prev_i == j) {
        count_edge(v,u,u,v);
        count_edge(u,right_i,v,right_i);
        count_edge(left_j,v,left_j,u);
    } else if (prev_j == i) {
        count_edge(left_i,u,left_i,v);
        count_edge(u,v,v,u);
        count_edge(v,right_j,u,right_j);
    } else {
        count_edge(left_i,u,left_i,v);
        count_edge(u,right_i,v,right_i);
        count_edge(left_j,v,left_j,u);
        count_edge(v,right_j,u,right_j);
    }
    return Cost(after - before);
}

template<class Distance>
auto tour_relocate_delta(std::span<const int> tour, std::size_t from,
                         std::size_t after, Distance&& distance) {
    using Cost = std::remove_cvref_t<std::invoke_result_t<Distance&, int, int>>;
    const std::size_t n = tour.size();
    if (from >= n || after >= n) std::abort();
    const std::size_t previous = from == 0 ? n - 1 : from - 1;
    if (from == after || previous == after) return Cost(0);
    const int u = tour[previous], v = tour[from], w = tour[from + 1 == n ? 0 : from + 1];
    const int a = tour[after], b = tour[after + 1 == n ? 0 : after + 1];
    const Cost removed = std::invoke(distance, u,v) + std::invoke(distance, v,w)
                       + std::invoke(distance, a,b);
    const Cost added = std::invoke(distance, u,w) + std::invoke(distance, a,v)
                     + std::invoke(distance, v,b);
    return Cost(added - removed);
}

void tour_relocate(std::span<int> tour, std::size_t from, std::size_t after) {
    const auto n = tour.size();
    if (from >= n || after >= n) std::abort();
    if (from == after || (from == 0 ? n - 1 : from - 1) == after) return;
    const auto begin = tour.begin();
    if (from < after) std::rotate(begin + from, begin + from + 1, begin + after + 1);
    else std::rotate(begin + after + 1, begin + from, begin + from + 1);
}

}

/*
細かい注意点
- 参照URL https://github.com/yuki-4869/Algorithm/blob/main/Heuristic/tour_moves.cpp
- 順序列は閉路を表し、始点を末尾に重複させない。非対称コストも利用できる。
- 添字は0-indexedで、afterは移動前の列で指定する。自分自身または閉路上の直前の位置を指定すると何もしない。
- 費用関数distance(u,v)は同じ頂点対で一定の符号付きの値を返す。中間和・差分が表現できる型を使う。

使える関数一覧
ahc::tour_swap_delta(tour,i,j,distance)             2点交換の変更後-変更前    O(1)
ahc::tour_relocate_delta(tour,from,after,distance)   1点移動の変更後-変更前    O(1)
ahc::tour_relocate(tour,from,after)                  1点を指定位置の後ろへ移動 O(|from-after|+1)
distance(u,v)がO(1)の場合。2点交換はstd::swapで実施できる。

基本の関数
vector<int> tour{0,1,2,3};
auto cost = [](int u,int v) -> long long { return 1LL*u*v*v; };
auto delta = ahc::tour_relocate_delta(tour,1,2,cost);
ahc::tour_relocate(tour,1,2);
assert(tour == (vector<int>{0,2,1,3}) && delta == -11);
*/
