std::vector<int> rooted_subtree_ids(const std::vector<int>& parent) {
    if (parent.size() > static_cast<std::size_t>(INT_MAX)) std::abort();
    const int n = static_cast<int>(parent.size());
    std::vector<std::vector<int>> children(n);
    std::vector<int> order;
    order.reserve(n);
    for (int v = 0; v < n; ++v) {
        const int u = parent[v];
        if (u == -1) order.push_back(v);
        else {
            if (u < 0 || u >= n || u == v) std::abort();
            children[u].push_back(v);
        }
    }
    for (std::size_t i = 0; i < order.size(); ++i) {
        for (int child : children[order[i]]) order.push_back(child);
    }
    if (order.size() != parent.size()) std::abort();
    std::vector<int> ids(n);
    std::map<std::vector<int>,int> signatures;
    for (auto it = order.rbegin(); it != order.rend(); ++it) {
        const int v = *it;
        std::vector<int> child_ids;
        child_ids.reserve(children[v].size());
        for (int u : children[v]) child_ids.push_back(ids[u]);
        std::sort(child_ids.begin(),child_ids.end());
        ids[v] = signatures.try_emplace(std::move(child_ids),static_cast<int>(signatures.size())).first->second;
    }
    return ids;
}

/*
細かい注意点
parent[v]は親の0始まり頂点番号、根は-1。木・森・空入力に対応。親番号に大小関係は不要。
子の順序を区別しない。同じ呼び出し内で、根付き部分木が同型である場合に限りIDが一致する。

使える関数一覧
rooted_subtree_ids(parent); // vector<int>: 各頂点の部分木の同型ID。0から連番
                            // 時間O(N log(N+2))、追加領域O(N)

基本の関数
auto ids=rooted_subtree_ids({-1,0,0,1,2});
if(ids[1]!=ids[2] || ids[3]!=ids[4] || ids[0]==ids[1])std::abort();
*/
