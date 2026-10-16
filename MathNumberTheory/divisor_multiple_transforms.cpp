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
}

template<class T> void divisor_zeta(vector<T>& f){
    do { if (!(!f.empty())) std::abort(); } while (false);
    long long n=(long long)f.size()-1;
    for(long long p:prime_list(n)){
        for(long long x=1;x<=n/p;x++) f[x*p]+=f[x];
    }
}

template<class T> void divisor_mobius(vector<T>& f){
    do { if (!(!f.empty())) std::abort(); } while (false);
    long long n=(long long)f.size()-1;
    for(long long p:prime_list(n)){
        for(long long x=n/p;x>=1;x--) f[x*p]-=f[x];
    }
}

template<class T> void multiple_zeta(vector<T>& f){
    do { if (!(!f.empty())) std::abort(); } while (false);
    long long n=(long long)f.size()-1;
    for(long long p:prime_list(n)){
        for(long long x=n/p;x>=1;x--) f[x]+=f[x*p];
    }
}

template<class T> void multiple_mobius(vector<T>& f){
    do { if (!(!f.empty())) std::abort(); } while (false);
    long long n=(long long)f.size()-1;
    for(long long p:prime_list(n)){
        for(long long x=1;x<=n/p;x++) f[x]-=f[x*p];
    }
}

/*
細かい注意点
vector<T> f(N+1)の添字1..Nを使用。f[0]は変更しない。空vectorは渡さない。
Tは+=と-=を持つ型。long longやmodintに対応。
divisor_zetaはf[x]=Σ_{d|x}a[d]、multiple_zetaはf[x]=Σ_{x|m}a[m]。
mobiusは対応するzetaの逆変換。約数側はLCM、倍数側はGCDの集計に使いやすい。
素数列挙を含めて時間O(N log log N)、追加メモリO(N)。
同名の変換関数を持つnumber_theory_bundleとは引数・返り値が異なる。

使える関数一覧
prime_list(N);        // vector<ll>: N以下の素数 O(N log log N)
divisor_zeta(f);      // void: 約数ゼータ変換、その場で更新 O(N log log N)
divisor_mobius(f);    // void: 約数メビウス変換 O(N log log N)
multiple_zeta(f);     // void: 倍数ゼータ変換 O(N log log N)
multiple_mobius(f);   // void: 倍数メビウス変換 O(N log log N)

基本の関数
vl counts(13,1); counts[0]=0;
divisor_zeta(counts);                         // counts[12]==6（約数個数）
divisor_mobius(counts);                       // 元の列へ戻す
multiple_zeta(counts);                        // 倍数側へ累積
multiple_mobius(counts);                      // 元の列へ戻す
*/
