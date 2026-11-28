namespace ahc {

class Random {
public:
    using result_type = std::uint64_t;

    explicit Random(result_type seed = 0) : state_(seed) {}

    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return UINT64_MAX; }

    result_type operator()() {
        result_type value = (state_ += 0x9e3779b97f4a7c15ULL);
        value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
        value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
        return value ^ (value >> 31);
    }

    result_type below(result_type bound) {
        if (bound == 0) std::abort();
        __uint128_t product = __uint128_t((*this)()) * bound;
        if (result_type(product) < bound) {
            const result_type threshold = (result_type(0) - bound) % bound;
            while (result_type(product) < threshold) {
                product = __uint128_t((*this)()) * bound;
            }
        }
        return result_type(product >> 64);
    }

    double real() {
        return double((*this)() >> 11) * 0x1.0p-53;
    }

    template<std::random_access_iterator It>
    void shuffle(It first, It last) {
        for (auto remaining = last - first; remaining > 1; --remaining) {
            const auto index = below(result_type(remaining));
            std::iter_swap(first + (remaining - 1), first + index);
        }
    }

private:
    result_type state_;
};

}

/*
細かい注意点
- 参照URL https://github.com/yuki-4869/Algorithm/blob/main/Heuristic/random.cpp
- SplitMix64。seedと呼び出し順が同じなら再現する。seed=0も有効。
- below(bound) の bound は正。real() は [0,1)。shuffle は [first,last)。
- gnu++23の __uint128_t を使う。

使える関数一覧
ahc::Random rng(seed=0)       乱数器を作る                          O(1)
ahc::Random::result_type      std::uint64_t
ahc::Random::min(), max()     生の乱数の理論上の端点を返す           O(1)
rng()                        64 bitの乱数を返す                    O(1)
rng.below(bound)             [0,bound) の一様整数を返す             期待O(1)
rng.real()                   [0,1) の乱数を返す                    O(1)
rng.shuffle(first,last)      半開区間をその場でシャッフルする       期待O(N)

基本の関数
ahc::Random rng(42);
vector<int> order{0,1,2,3};
rng.shuffle(order.begin(),order.end());
int index = rng.below(order.size());
double u = rng.real();
*/
