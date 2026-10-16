// ANDが0になる順列の構築。
// Source: https://github.com/yuki-4869/Algorithm/blob/main/MathCombinatorics/and_zero_permutation.cpp
#include <stdexcept>
#include <vector>

// 各 i で (A[i] & B[i]) == 0 を満たす順列 B を返す。
// zero_indexed == true : A = {0, 1, ..., N-1}
// zero_indexed == false: A = {1, 2, ..., N}
// 返す vector はどちらも N 要素で、添字は 0 始まり。
// trueなら常に構築可能。falseではNが偶数のときだけ構築可能。
// 解がない場合は空配列を返す。N == 0 の有効な空順列も空配列。
// N == 0 は空配列、N < 0 は invalid_argument。
// 時間 O(N)、返却配列の領域 O(N)。
std::vector<long long> and_zero_permutation(long long N, bool zero_indexed = true) {
    if (N < 0) {
        throw std::invalid_argument("and_zero_permutation: N must be nonnegative");
    }
    // 1..NでNが奇数の場合、奇数のAに対応する偶数のBが足りない。
    if (!zero_indexed && (N & 1LL)) return {};

    std::vector<long long> B(static_cast<std::vector<long long>::size_type>(N));
    const long long first = zero_indexed ? 0 : 1;
    long long right = zero_indexed ? N - 1 : N;
    long long mask = 0;
    while (mask < right) mask = (mask << 1) | 1LL;

    while (right >= first) {
        // right以上の最小の「全ビット1」の数に調整する。
        // maskは単調に小さくなるため、このループの合計はO(log(N+1))。
        while (mask > 0 && (mask >> 1) >= right) mask >>= 1;
        const long long left = mask ^ right;
        // [left,right]内でビット反転を対応させる。
        // left=mask-rightなので、この区間内の全値が一度ずつ使われる。
        for (long long x = left; x <= right; ++x) {
            B[static_cast<std::vector<long long>::size_type>(x - first)] = mask ^ x;
        }
        right = left - 1;
    }
    return B;
}

/*
公開API一覧

N = 順列の要素数。N>=0。戻り値はどちらのモードでもN要素のvector。

and_zero_permutation(N);             // vector<long long>: A={0,...,N-1}に対し、各(A[i]&B[i])==0となる順列Bを返す O(N)
and_zero_permutation(N, true);       // vector<long long>: 上と同じ。0始まりの値域で構築 O(N)
and_zero_permutation(N, false);      // vector<long long>: A={1,...,N}に対する順列B。Nが奇数なら空配列を返す O(N)、解なしの判定はO(1)

// 値域と添字
// trueならBの値域は0..N-1、falseなら1..N。vectorの添字は常に0..N-1。
// 1始まりの場合は、各i=0..N-1について ((i+1)&B[i])==0 を満たす。
// falseかつNが奇数のとき、奇数のAに割り当てる偶数のBが不足するため解なし。
// N=0は有効な空順列。N<0はstd::invalid_argument。
// 解なしを返す場合はN要素の配列を確保しない。
// 計算量は時間O(N)、返却配列O(N)、それ以外の追加領域O(1)。
*/
