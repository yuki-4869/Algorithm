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
bool isPrime(x) xが素数か返す O(1)
vector<pair<ll,ll>> factorize(x) xを{素因数,指数}に分解 O(logx)
vl divisors(x) xの約数を全て返す O(約数個数)

ls.prime n以下の素数一覧
ls.spf[x] xの最小素因数
ls.phi[x] Eulerのφ関数
ls.mu[x] Möbius関数

LinearSieve ls(N);
bool p=ls.isPrime(x);
auto f=ls.factorize(x);
auto d=ls.divisors(x);

初期構築 O(N)
factorize/divisorsは1<=x<=Nを想定
*/