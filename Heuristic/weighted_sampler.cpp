namespace ahc {

class WeightedSampler {
public:
    using Weight = std::uint64_t;

    explicit WeightedSampler(std::size_t n) : weights_(n), tree_(capacity(n)) {}

    explicit WeightedSampler(std::span<const Weight> weights)
        : weights_(weights.begin(), weights.end()), tree_(capacity(weights.size())) {
        std::copy(weights_.begin(), weights_.end(), tree_.begin());
        std::size_t largest = 0;
        Weight peak = 0, peak_begin = 0;
        for (std::size_t i = 0; i < size(); ++i) {
            const Weight weight = weights_[i];
            if (weight > UINT64_MAX - total_) std::abort();
            if (weight > peak) {
                largest = i;
                peak = weight;
                peak_begin = total_;
            }
            total_ += weight;
        }
        if (dominates(peak)) {
            dominant_ = largest;
            dominant_begin_ = peak_begin;
            dominant_end_ = peak_begin + peak;
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
        if (value == old) return;
        if (value > UINT64_MAX - (total_ - old)) std::abort();
        total_ = total_ - old + value;
        weights_[i] = value;
        const Weight change = value - old;
        if (dominant_end_ != 0) {
            if (i < dominant_) dominant_begin_ += change;
            if (i <= dominant_) dominant_end_ += change;
            if (!dominates(dominant_end_ - dominant_begin_)) dominant_end_ = 0;
        }
        if (dominant_end_ == 0 && dominates(value)) {
            dominant_ = i;
            dominant_begin_ = prefix_weight(i);
            dominant_end_ = dominant_begin_ + value;
        }
        for (; i < tree_.size(); i |= i + 1) tree_[i] += change;
    }

    std::size_t select(Weight rank) const {
        if (rank >= total_) std::abort();
        if (rank < dominant_end_ && rank >= dominant_begin_) return dominant_;
        std::size_t prefix = 0;
        for (std::size_t step = tree_.size() / 2; step; step >>= 1) {
            const Weight block = tree_[prefix + step - 1];
            const Weight mask = Weight(0) - Weight(block <= rank);
            prefix += step & std::size_t(mask);
            rank -= block & mask;
        }
        return prefix;
    }

private:
    static std::size_t capacity(std::size_t n) {
        if (n == 0) return 0;
        if (n > std::bit_floor(std::vector<Weight>().max_size())) std::abort();
        return std::bit_ceil(n);
    }

    bool dominates(Weight weight) const {
        return weight != 0 && weight >= total_ - total_ / 8;
    }

    Weight prefix_weight(std::size_t end) const {
        Weight sum = 0;
        for (; end; end &= end - 1) sum += tree_[end - 1];
        return sum;
    }

    std::vector<Weight> weights_, tree_;
    Weight total_ = 0;
    std::size_t dominant_ = 0;
    Weight dominant_begin_ = 0, dominant_end_ = 0;
};

}

/*
細かい注意点
- 参照URL https://github.com/yuki-4869/Algorithm/blob/main/Heuristic/weighted_sampler.cpp
- 添字は0-indexed。重みは非負の整数で、全体和はuint64_tに収める。
- select(rank) のrankは [0,total())。全重み0では抽選できない。
- rankを一様に選べば重み比例。重み0の要素には対応するrankが存在しない。
- 木の内部は2冪長で、余分な葉の重みは0。元の要素数に対し、木の領域は2倍未満。
- 構築・更新で総重みの7/8以上の要素を検出した場合、その抽選範囲では直接添字を返す。

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
