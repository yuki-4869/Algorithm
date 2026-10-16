long long modpow(long long a, long long n, long long mod) {
    long long res = 1;
    while (n > 0) {
        if (n & 1) res = res * a % mod;
        a = a * a % mod;
        n >>= 1;
    }
    return res;
}

long long modinv(long long a, long long mod) {
    return modpow(a, mod - 2, mod);
}

/*
細かい注意点
modpowはn>=0、mod>0。中間のa*aとres*aがlong longに収まること。
modinvはFermatの小定理を使う。法modは素数、aはmodの倍数でないこと。
負の基数や非常に大きい法は、この実装の正規化・積の範囲に注意する。

使える関数一覧
modpow(a,n,mod); // ll: a^n mod mod O(log n)
modinv(a,mod);   // ll: aの逆元、素数法用 O(log mod)

基本の関数
auto power = modpow(2,10,1000000007);         // 1024
auto inverse = modinv(2,1000000007);          // 500000004
*/
