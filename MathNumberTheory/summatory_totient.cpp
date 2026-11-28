template<class T>
T summatory_totient(long long n) {
    if (n < 0 || n > 10000000000LL) std::abort();
    if (n == 0) return T(0);
    const long long limit = std::clamp(static_cast<long long>(std::pow(static_cast<long double>(n),2.0L/3)),1LL,n);
    std::vector<T> prefix(limit+1,T(0));
    {
        LinearSieve sieve(limit);
        for (long long i = 1; i <= limit; ++i) prefix[i] = prefix[i-1]+T(sieve.phi[i]);
    }
    std::vector<T> memo(n/(limit+1)+1,T(0));
    std::vector<bool> ready(memo.size());
    auto solve = [&](auto&& self, long long x) -> T {
        if (x <= limit) return prefix[x];
        const long long key = n/x;
        if (ready[key]) return memo[key];
        T result = (x&1) ? T(x)*T((x+1)/2) : T(x/2)*T(x+1);
        for (long long l = 2; l <= x; ) {
            const long long quotient = x/l, r = x/quotient;
            result -= T(r-l+1)*self(self,quotient);
            l = r+1;
        }
        ready[key] = true;
        return memo[key] = result;
    };
    return solve(solve,n);
}

/*
細かい注意点
0<=N<=10^10。先にLinearSieve（linear_sieve.cpp）を貼る。
Tにmodintを指定できる。整数で厳密に求める場合は__int128を使用できる。
三角数では法の上の割り算を行わない。phi(1)=1、N=0では0。

使える関数一覧
summatory_totient<T>(n); // T: phi(1)からphi(n)の総和。O(N^(2/3)) 時間・領域

基本の関数
if(summatory_totient<mint>(10)!=32 || summatory_totient<__int128>(0)!=0)std::abort();
*/
