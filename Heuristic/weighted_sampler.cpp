namespace ahc {

class WeightedSampler {
public:
    using Weight = std::uint64_t;

    explicit WeightedSampler(std::size_t n) : weights_(n), tree_(n) {}

    explicit WeightedSampler(std::span<const Weight> weights)
        : weights_(weights.begin(), weights.end()), tree_(weights_) {
        for (Weight weight : weights_) {
            if (weight > UINT64_MAX - total_) std::abort();
            total_ += weight;
        }
        for (std::size_t i = 0; i < tree_.size(); ++i) {
            const std::size_t parent = i | (i + 1);
            if (parent < tree_.size()) tree_[parent] += tree_[i];
        }
    }

    std::size_t size() const { return weights_.size(); }
    Weight weight(std::size_t i) const { return weights_[i]; }
    Weight total() const { return total_; }

    void set(std::size_t i, Weight value) {
        const Weight old = weights_[i];
        if (value > UINT64_MAX - (total_ - old)) std::abort();
        total_ = total_ - old + value;
        weights_[i] = value;
        for (; i < tree_.size(); i |= i + 1) tree_[i] = tree_[i] - old + value;
    }

    std::size_t select(Weight rank) const {
        if (rank >= total_) std::abort();
        std::size_t prefix = 0;
        for (std::size_t step = std::bit_floor(size()); step; step >>= 1) {
            if (step <= size() - prefix && tree_[prefix + step - 1] <= rank) {
                prefix += step;
                rank -= tree_[prefix - 1];
            }
        }
        return prefix;
    }

private:
    std::vector<Weight> weights_, tree_;
    Weight total_ = 0;
};

}

/*
細かい注意点
- 参照URL https://github.com/yuki-4869/Algorithm/blob/main/Heuristic/weighted_sampler.cpp
- 添字は0-indexed。重みは非負の整数で、全体和はuint64_tに収める。
- select(rank) のrankは [0,total())。全重み0では抽選できない。
- rankを一様に選べば重み比例。重み0の要素には対応するrankが存在しない。

使える関数一覧
ahc::WeightedSampler ws(n)         重み0で構築                        O(N)
ahc::WeightedSampler ws(weights)   span<const Weight>で構築          O(N)
ahc::WeightedSampler::Weight       uint64_t
ws.size(), ws.weight(i), ws.total()  要素数、点の重み、全重み        O(1)
ws.set(i,x)                        点の重みをxへ置き換える          O(log N)
ws.select(rank)                    重み付き順序でrank番目の添字     O(log N)

基本の関数
vector<uint64_t> weights{3,0,2};
ahc::WeightedSampler ws(weights);
assert(ws.select(2) == 0 && ws.select(3) == 2);
ws.set(0,0);
assert(ws.total() == 2 && ws.select(0) == 2);
*/
