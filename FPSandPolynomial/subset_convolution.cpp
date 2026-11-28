template<class T>
std::vector<T> subset_convolution(const std::vector<T>& f, const std::vector<T>& g) {
    const std::size_t size = f.size();
    if (g.size() != size || !std::has_single_bit(size)) std::abort();
    const int bits = std::bit_width(size)-1;
    const std::size_t rows = bits+1;
    if (size > f.max_size()/rows) std::abort();
    std::vector<T> left(size*rows,T(0)), right(size*rows,T(0));
    std::vector<unsigned char> weight(size);
    for (std::size_t mask = 0; mask < size; ++mask) {
        weight[mask] = std::popcount(mask);
        const auto index = size*weight[mask]+mask;
        left[index] = f[mask];
        right[index] = g[mask];
    }

    for (std::size_t step = 1; step < size; step *= 2) {
        for (int rank = 0; rank < bits; ++rank) {
            auto* a = left.data()+rank*size;
            auto* b = right.data()+rank*size;
            for (std::size_t offset = 0; offset < size; offset += 2*step) {
                for (std::size_t i = offset; i < offset+step; ++i) {
                    a[i+step] += a[i];
                    b[i+step] += b[i];
                }
            }
        }
    }
    std::vector<T> result(size), layer(size);
    for (int degree = 0; degree <= bits; ++degree) {
        std::fill(layer.begin(),layer.end(),T(0));
        for (int rank = 0; rank <= degree; ++rank) {
            const auto* a = left.data()+rank*size;
            const auto* b = right.data()+(degree-rank)*size;
            for (std::size_t mask = 0; mask < size; ++mask) layer[mask] += a[mask]*b[mask];
        }
        for (std::size_t step = 1; step < size; step *= 2) {
            for (std::size_t offset = 0; offset < size; offset += 2*step) {
                for (std::size_t i = offset; i < offset+step; ++i) layer[i+step] -= layer[i];
            }
        }
        for (std::size_t mask = 0; mask < size; ++mask) {
            if (weight[mask] == degree) result[mask] = layer[mask];
        }
    }
    return result;
}

/*
細かい注意点
配列長は両方とも2^N。添字は部分集合のビットマスク。
加算・減算・乗算を使う。整数型では途中の値も型に収まること。modintを使える。
T=boolの代わりに、必要な環の演算を持つ型を使う。

使える関数一覧
subset_convolution(f,g); // vector<T>: h[S]=sum_{A subset S} f[A]*g[S xor A]
                         // 時間O((N+1)^2*2^N)、追加領域O((N+1)*2^N)

基本の関数
std::vector<mint> f={1,2,3,4},g={5,6,7,8};
auto h=subset_convolution(f,g);
if(h[3]!=60)std::abort();
*/
