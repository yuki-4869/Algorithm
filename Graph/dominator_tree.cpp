std::vector<int> dominator_tree(const std::vector<std::vector<int>>& graph, int root) {
    if (graph.size() >= static_cast<std::size_t>(INT_MAX)) std::abort();
    const int n = static_cast<int>(graph.size());
    if (root < 0 || root >= n) std::abort();

    std::vector<int> index(n), vertex{0,root}, parent{0,0};
    std::vector<std::size_t> edge_index(n);
    std::vector<int> stack{root};
    index[root] = 1;
    while (!stack.empty()) {
        const int u = stack.back();
        if (edge_index[u] == graph[u].size()) {
            stack.pop_back();
            continue;
        }
        const int v = graph[u][edge_index[u]++];
        if (v < 0 || v >= n) std::abort();
        if (index[v] != 0) continue;
        index[v] = static_cast<int>(vertex.size());
        vertex.push_back(v);
        parent.push_back(index[u]);
        stack.push_back(v);
    }

    const int count = static_cast<int>(vertex.size());
    std::vector<std::vector<int>> pred(count), bucket(count);
    for (int u = 0; u < n; ++u) if (index[u] != 0) {
        for (int v : graph[u]) pred[index[v]].push_back(index[u]);
    }
    std::vector<int> semi(count), label(count), ancestor(count), idom(count), path;
    std::iota(semi.begin(), semi.end(), 0);
    std::iota(label.begin(), label.end(), 0);
    auto eval = [&](int v) {
        int u = v;
        while (ancestor[ancestor[u]] != 0) {
            path.push_back(u);
            u = ancestor[u];
        }
        while (!path.empty()) {
            const int x = path.back();
            path.pop_back();
            const int a = ancestor[x];
            if (semi[label[a]] < semi[label[x]]) label[x] = label[a];
            ancestor[x] = ancestor[a];
        }
        return label[v];
    };

    for (int i = count-1; i >= 2; --i) {
        for (int u : pred[i]) semi[i] = std::min(semi[i], semi[eval(u)]);
        bucket[semi[i]].push_back(i);
        const int p = parent[i];
        ancestor[i] = p;
        for (int v : bucket[p]) {
            const int u = eval(v);
            idom[v] = semi[u] < semi[v] ? u : p;
        }
        bucket[p].clear();
    }
    for (int i = 2; i < count; ++i) {
        if (idom[i] != semi[i]) idom[i] = idom[idom[i]];
    }
    std::vector<int> result(n,-1);
    result[root] = root;
    for (int i = 2; i < count; ++i) result[vertex[i]] = vertex[idom[i]];
    return result;
}

/*
細かい注意点
有向グラフの0-indexed隣接リストを渡す。根は[0,N)、0<N<INT_MAX。自己ループ・多重辺可。
全端点は[0,N)。根の返り値は根自身、根から未到達の頂点は-1。
DFSと経路圧縮は反復処理で行う。

使える関数一覧
dominator_tree(graph,root); // vector<int>: 直近支配頂点を頂点順に返す O((N+M) log(N+1)) 時間・O(N+M) 領域

基本の関数
std::vector<std::vector<int>> g={{1,2},{3},{3},{},{}};
auto immediate=dominator_tree(g,0);
if(immediate!=std::vector<int>{0,0,0,0,-1})std::abort();
*/
