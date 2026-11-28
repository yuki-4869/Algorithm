std::vector<int> maximum_independent_set(int n, const std::vector<std::pair<int,int>>& edges) {
    if (n < 0 || n > 40) std::abort();
    std::vector<std::uint64_t> neighbors(n);
    std::uint64_t allowed = (std::uint64_t{1} << n)-1;
    for (auto [u,v] : edges) {
        if (u < 0 || u >= n || v < 0 || v >= n) std::abort();
        if (u == v) allowed &= ~(std::uint64_t{1} << u);
        else {
            neighbors[u] |= std::uint64_t{1} << v;
            neighbors[v] |= std::uint64_t{1} << u;
        }
    }
    const int left_size = n/2;
    const unsigned left_mask = (1u << left_size)-1;
    std::vector<unsigned> best_right(1u << (n-left_size));
    for (unsigned mask = 1; mask < best_right.size(); ++mask) {
        const int i = std::countr_zero(mask)+left_size;
        const unsigned rest = mask & (mask-1);
        const unsigned skip = best_right[rest];
        best_right[mask] = skip;
        if ((allowed >> i & 1) == 0) continue;
        const unsigned take = (mask ^ rest) |
                              best_right[rest & ~static_cast<unsigned>(neighbors[i] >> left_size)];
        if (std::popcount(take) > std::popcount(skip)) best_right[mask] = take;
    }

    std::uint64_t optimum = 0;
    int best_size = 0;
    auto search = [&](auto&& self, unsigned candidates, unsigned right, unsigned chosen) -> void {
        if (candidates == 0) {
            const auto full = (std::uint64_t{best_right[right]} << left_size) | chosen;
            const int size = std::popcount(full);
            if (size > best_size) {
                best_size = size;
                optimum = full;
            }
            return;
        }
        const int v = std::countr_zero(candidates);
        const unsigned rest = candidates & (candidates-1);
        self(self,rest,right,chosen);
        self(self,rest & ~static_cast<unsigned>(neighbors[v]),
             right & ~static_cast<unsigned>(neighbors[v] >> left_size),chosen | (1u << v));
    };
    search(search,static_cast<unsigned>(allowed) & left_mask,
           static_cast<unsigned>(allowed >> left_size),0);

    std::vector<int> result;
    for (; optimum != 0; optimum &= optimum-1) result.push_back(std::countr_zero(optimum));
    return result;
}

/*
細かい注意点
無向グラフ。頂点番号は0-indexed、0<=N<=40。自己ループのある頂点は選ばない。重複辺は可。
最大集合を1つ返す。頂点は昇順。

使える関数一覧
maximum_independent_set(n,edges); // vector<int>: 最大独立集合 O(M+2^ceil(N/2)) 時間・O(N+2^ceil(N/2)) 領域

基本の関数
auto selected=maximum_independent_set(4,{{0,1},{1,2},{2,3}});
if(selected.size()!=2)std::abort();
*/
