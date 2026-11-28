std::vector<long long> tree_distance_frequency(const graph& g) {
    if (g.size() > (1u<<23)) std::abort();
    const int n = static_cast<int>(g.size());
    const CentroidDecomposition decomposition(g);
    std::vector<std::vector<long long>> whole(n), branch(n);
    auto increment = [](std::vector<long long>& histogram, int d) {
        if (static_cast<int>(histogram.size()) <= d) histogram.resize(d+1);
        ++histogram[d];
    };
    for (int v = 0; v < n; ++v) {
        const auto& ancestors = decomposition.ancestors(v);
        for (std::size_t i = 0; i < ancestors.size(); ++i) {
            const auto [center,distance] = ancestors[i];
            increment(whole[center],static_cast<int>(distance));
            if (i+1 < ancestors.size()) {
                increment(branch[ancestors[i+1].first],static_cast<int>(distance));
            }
        }
    }
    std::vector<long long> result(n);
    auto accumulate_square = [&](const std::vector<long long>& histogram, int sign) {
        const int size = static_cast<int>(histogram.size());
        if (size <= 48) {
            for (int i = 0; i < size; ++i) if (histogram[i] != 0) {
                for (int j = std::max(i,1); j < size && i+j < n; ++j) {
                    result[i+j] += sign*histogram[i]*histogram[j]*(i == j ? 1 : 2);
                }
            }
        } else {
            auto convolution = atcoder::convolution_ll(histogram,histogram);
            const int limit = std::min(n,static_cast<int>(convolution.size()));
            for (int i = 1; i < limit; ++i) result[i] += sign*convolution[i];
        }
    };
    for (int v = 0; v < n; ++v) {
        accumulate_square(whole[v],1);
        accumulate_square(branch[v],-1);
    }
    for (auto& count : result) count /= 2;
    return result;
}

/*
細かい注意点
0始まり・重みなし無向木。両方向に各辺を1回登録する。N<=2^23、空の木に対応。
centroid_decomposition.cppを先に貼る。codecfではatcoder/convolutionを追加。
長さNの結果で、添字dが距離dの異なる無順序頂点対の数。添字0は0。

使える関数一覧
tree_distance_frequency(g); // vector<long long>: 距離別の頂点対数
                            // 時間O(N log^2(N+2))、追加領域O(N log(N+2))

基本の関数
graph g={{1},{0,2},{1,3},{2}};
auto count=tree_distance_frequency(g);
if(count!=std::vector<long long>{0,3,2,1})std::abort();
*/
