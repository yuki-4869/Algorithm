std::vector<long long> and_zero_permutation(long long N, bool zero_indexed = true) {
    if (N < 0) {
        std::abort();
    }

    if (!zero_indexed && (N & 1LL)) return {};

    std::vector<long long> B(static_cast<std::vector<long long>::size_type>(N));
    const long long first = zero_indexed ? 0 : 1;
    long long right = zero_indexed ? N - 1 : N;
    long long mask = 0;
    while (mask < right) mask = (mask << 1) | 1LL;

    while (right >= first) {
        while (mask > 0 && (mask >> 1) >= right) mask >>= 1;
        const long long left = mask ^ right;

        for (long long x = left; x <= right; ++x) {
            B[static_cast<std::vector<long long>::size_type>(x - first)] = mask ^ x;
        }
        right = left - 1;
    }
    return B;
}

/*
細かい注意点
N>=0。各iについてA[i]&B[i]==0となる順列を返す。
true（既定）は値域0..N-1、falseは1..N。返却vectorの添字は常に0..N-1。
trueは常に構築可能。falseかつNが奇数なら解なしで空配列、N=0の有効な順列も空配列。
解なし判定はO(1)で配列を確保しない。N<0はメッセージなしで停止。
時間O(N)、返却配列O(N)、それ以外の追加領域O(1)。

使える関数一覧
and_zero_permutation(N);       // vector<ll>: 値域0..N-1の順列 O(N)
and_zero_permutation(N,true);  // vector<ll>: 既定と同じ O(N)
and_zero_permutation(N,false); // vector<ll>: 値域1..Nの順列、解なしなら{}

基本の関数
auto zero_based = and_zero_permutation(5);    // {0,2,1,4,3}
auto one_based = and_zero_permutation(4,false);// {2,1,4,3}
auto impossible = and_zero_permutation(3,false); // 空配列
*/
