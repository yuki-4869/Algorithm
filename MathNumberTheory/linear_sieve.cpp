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
        if(x>n){
            for(ll p:prime){
                if(x<=n||p>x/p)break;
                if(x%p!=0)continue;
                ll c=0;
                while(x%p==0)x/=p,c++;
                res.push_back({p,c});
            }
            if(x>n){
                res.push_back({x,1});
                return res;
            }
        }
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
N>=1としてNまでO(N)で前計算。isPrimeとspf/phi/muの範囲はNまで。
factorizeは1<=x<=LLONG_MAXに対応。x<=Nなら従来の最小素因数表を使う。
x>NならN以下の素数で試し割りし、同じ素数で割り切った後の残りがN以下なら表へ切り替える。
試し割りはp>残り/pでも打ち切り、最後の残りr>1は{r,1}として返す。
x<=N^2では正しい素因数分解を保証。x>N^2では最後の因子が合成数の場合もある。
結果は因子の昇順で、因子^指数の積は入力xに等しい。x=1は空。
divisorsもfactorizeを使うため1<=x<=N^2なら全約数を列挙。それを超える場合は全列挙を保証しない。
PはN以下の素数の個数、F(x)はx<=NならO(log x)、それ以外はO(P+log x)。
primeは素数一覧、spfは最小素因数、phiはEulerのφ関数、muはMöbius関数。
number_theory_bundleのLinearSieveと同名なので同時に貼らない。

使える関数一覧
LinearSieve ls(N); // 構築 O(N)
ls.isPrime(x);     // bool: 素数判定 O(1)
ls.factorize(x);   // vector<pair<ll,ll>>: 因子と指数 F(x)
ls.divisors(x);    // vector<ll>: 約数列挙 F(x)+O(出力個数)
ls.prime;          // vector<ll>: N以下の素数
ls.spf[x], ls.phi[x], ls.mu[x]; // 最小素因数、φ、μ O(1)

基本の関数
LinearSieve ls(100);
auto prime = ls.isPrime(97);                  // true
auto factors = ls.factorize(12);             // {{2,2},{3,1}}
auto divisors = ls.divisors(12);
LinearSieve small(10);
auto joined = small.factorize(72);           // {{2,3},{3,2}}: 2で割り切ると残り9なので表へ切り替え
auto partial = small.factorize(143);         // {{143,1}}: 11*13は分解せず残りを1因子として返す
*/
