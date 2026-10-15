/*
約数・倍数ゼータ変換 / メビウス変換

添字規約:
    vector<T> f(N+1) を用意し、f[1]～f[N] を使用する。
    f[0] は変換されず、未使用とする。

型 T の条件:
    T が += と -= に対応していること。
    long long、modint998244353 などで使用可能。

計算量:
    素数列挙を含めて O(N log log N)
    メモリ O(N)
*/

inline vector<long long> prime_list(long long n){
    vector<bool> is_prime(n+1,true);
    vector<long long> ret;
    if(n>=0) is_prime[0]=false;
    if(n>=1) is_prime[1]=false;
    for(long long p=2;p<=n;p++){
        if(!is_prime[p]) continue;
        ret.push_back(p);
        if(p<=n/p) for(long long x=p*p;x<=n;x+=p) is_prime[x]=false;
    }
    return ret;
} // n以下の素数列挙

// 約数ゼータ変換
// 変換後: f[x] = Σ_{d|x} 変換前のf[d]
template<class T> void divisor_zeta(vector<T>& f){
    assert(!f.empty());
    long long n=(long long)f.size()-1;
    for(long long p:prime_list(n)){
        for(long long x=1;x<=n/p;x++) f[x*p]+=f[x];
    }
}

// 約数メビウス変換
// divisor_zeta の逆変換
template<class T> void divisor_mobius(vector<T>& f){
    assert(!f.empty());
    long long n=(long long)f.size()-1;
    for(long long p:prime_list(n)){
        for(long long x=n/p;x>=1;x--) f[x*p]-=f[x];
    }
}

// 倍数ゼータ変換
// 変換後: f[x] = Σ_{x|m} 変換前のf[m]
template<class T> void multiple_zeta(vector<T>& f){
    assert(!f.empty());
    long long n=(long long)f.size()-1;
    for(long long p:prime_list(n)){
        for(long long x=n/p;x>=1;x--) f[x]+=f[x*p];
    }
}

// 倍数メビウス変換
// multiple_zeta の逆変換
template<class T> void multiple_mobius(vector<T>& f){
    assert(!f.empty());
    long long n=(long long)f.size()-1;
    for(long long p:prime_list(n)){
        for(long long x=1;x<=n/p;x++) f[x]-=f[x*p];
    }
}

/*
============================================================
使用例1: 1～N の各整数の約数個数を求める
============================================================

long long N;
vector<long long> cnt(N+1,1);
cnt[0]=0;
divisor_zeta(cnt);

// cnt[x] = x の正の約数の個数


============================================================
使用例2: 各 x について、A_i | x を満たす要素数を求める
============================================================

long long N,M;
vector<long long> A(N);
vector<long long> cnt(M+1);
for(long long x:A) cnt[x]++;
divisor_zeta(cnt);

// cnt[x] = A_i が x の約数であるような i の個数


============================================================
使用例3: LCM がちょうど x の順序付きペア数を求める
============================================================

long long N,M;
vector<long long> A(N);
vector<mint> cnt(M+1);
for(long long x:A) cnt[x]++;

divisor_zeta(cnt);
for(long long x=1;x<=M;x++) cnt[x]*=cnt[x];
divisor_mobius(cnt);

// cnt[x] = lcm(A_i,A_j)==x を満たす順序付きペア (i,j) の個数
// i<j のペアなら、二乗の代わりに cnt[x]*(cnt[x]-1)/2 を使う。


============================================================
使用例4: 各 d について、d | A_i を満たす要素数を求める
============================================================

long long N,M;
vector<long long> A(N);
vector<long long> cnt(M+1);
for(long long x:A) cnt[x]++;
multiple_zeta(cnt);

// cnt[d] = d の倍数である A_i の個数


============================================================
使用例5: GCD がちょうど d の順序付きペア数を求める
============================================================

long long N,M;
vector<long long> A(N);
vector<mint> cnt(M+1);
for(long long x:A) cnt[x]++;

multiple_zeta(cnt);
for(long long d=1;d<=M;d++) cnt[d]*=cnt[d];
multiple_mobius(cnt);

// cnt[d] = gcd(A_i,A_j)==d を満たす順序付きペア (i,j) の個数
// i<j のペアなら、二乗の代わりに cnt[d]*(cnt[d]-1)/2 を使う。


============================================================
変換方向の対応
============================================================

divisor_zeta:
    約数 d から、その倍数 x へ値を集める。
    f[x] = Σ_{d|x} a[d]
    LCM 系で使いやすい。

divisor_mobius:
    divisor_zeta で累積した値から「ちょうど x」を復元する。

multiple_zeta:
    倍数 m から、その約数 x へ値を集める。
    f[x] = Σ_{x|m} a[m]
    GCD 系で使いやすい。

multiple_mobius:
    multiple_zeta で累積した値から「ちょうど x」を復元する。
*/