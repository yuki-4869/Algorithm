struct LinearSieve{
    ll n;
    vector<ll> prime,spf,phi,mu;

    LinearSieve(ll n):n(n),spf(n+1),phi(n+1),mu(n+1){
        if(n>=1)phi[1]=mu[1]=1;
        for(ll i=2;i<=n;i++){
            if(!spf[i])spf[i]=i,prime.push_back(i),phi[i]=i-1,mu[i]=-1;
            for(ll p:prime){
                if(p>spf[i]||i*p>n)break;
                spf[i*p]=p;
                if(i%p==0)phi[i*p]=phi[i]*p,mu[i*p]=0;
                else phi[i*p]=phi[i]*(p-1),mu[i*p]=-mu[i];
            }
        }
    }

    bool isPrime(ll x){return x>=2&&spf[x]==x;}

    vector<pair<ll,ll>> factorize(ll x){
        vector<pair<ll,ll>> res;
        while(x>1){
            ll p=spf[x],c=0;
            while(x%p==0)x/=p,c++;
            res.push_back({p,c});
        }
        return res;
    }

    vector<ll> divisors(ll x){
        vector<ll> d={1};
        for(auto [p,c]:factorize(x)){
            ll s=d.size(),q=1;
            for(ll k=1;k<=c;k++){
                q*=p;
                for(ll i=0;i<s;i++)d.push_back(d[i]*q);
            }
        }
        return d;
    }
};

/*
細かい注意点
NまでO(N)で前計算。isPrimeは対応範囲内、factorize/divisorsは1<=x<=Nで使用する。
primeは素数一覧、spfは最小素因数、phiはEulerのφ関数、muはMöbius関数。
number_theory_bundleのLinearSieveと同名なので同時に貼らない。

使える関数一覧
LinearSieve ls(N); // 構築 O(N)
ls.isPrime(x);     // bool: 素数判定 O(1)
ls.factorize(x);   // vector<pair<ll,ll>>: 素因数と指数 O(log x)
ls.divisors(x);    // vector<ll>: 約数列挙 O(約数個数)
ls.prime;          // vector<ll>: N以下の素数
ls.spf[x], ls.phi[x], ls.mu[x]; // 最小素因数、φ、μ O(1)

基本の関数
LinearSieve ls(100);
auto prime = ls.isPrime(97);                  // true
auto factors = ls.factorize(12);             // {{2,2},{3,1}}
auto divisors = ls.divisors(12);
*/
