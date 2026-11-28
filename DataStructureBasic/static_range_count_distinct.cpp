template<class T>
std::vector<int> static_range_count_distinct(const std::vector<T>& values,
                                             const std::vector<std::pair<int,int>>& queries) {
    if (values.size() >= static_cast<std::size_t>(INT_MAX) ||
        queries.size() > static_cast<std::size_t>(INT_MAX)) std::abort();
    const int n = static_cast<int>(values.size());
    const int q = static_cast<int>(queries.size());
    auto keys = values;
    std::sort(keys.begin(),keys.end());
    keys.erase(std::unique(keys.begin(),keys.end()),keys.end());
    std::vector<int> head(n+1,-1), next(q), last(keys.size(),-1), result(q);
    for (int i = 0; i < q; ++i) {
        auto [l,r] = queries[i];
        if (l < 0 || r < l || r > n) std::abort();
        next[i] = head[r];
        head[r] = i;
    }
    atcoder::fenwick_tree<int> active(n);
    for (int r = 0; r <= n; ++r) {
        if (r > 0) {
            const auto index = std::lower_bound(keys.begin(),keys.end(),values[r-1])-keys.begin();
            if (last[index] != -1) active.add(last[index],-1);
            last[index] = r-1;
            active.add(r-1,1);
        }
        for (int i = head[r]; i != -1; i = next[i]) result[i] = active.sum(queries[i].first,r);
    }
    return result;
}

/*
細かい注意点
各クエリは0始まりの半開区間[l,r)。空区間の答えは0。処理はオフライン。
Tの大小比較と等値比較は整合していること。codecfではatcoder/fenwicktreeを追加。

使える関数一覧
static_range_count_distinct(values,queries); // vector<int>: クエリ順の種類数 O((N+Q)log(N+2)) 時間、O(N+Q) 領域

基本の関数
auto count=static_range_count_distinct(std::vector<int>{1,2,1,3},{{0,3},{1,4},{2,2}});
if(count!=std::vector<int>{2,3,0})std::abort();
*/
