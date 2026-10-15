
struct LinearSieve{
    ll n;
    vl mn,primes,id,pi;

    LinearSieve(ll n):n(n),mn(n+1),id(n+1,-1),pi(n+1){
        if(n>=1) mn[1]=1;
        for(ll i=2;i<=n;i++){
            if(!mn[i]){
                mn[i]=i;
                id[i]=primes.size();
                primes.pb(i);
            }
            for(ll p:primes){
                if(p>mn[i]||i*p>n) break;
                mn[i*p]=p;
            }
        }
        for(ll i=1;i<=n;i++)
            pi[i]=pi[i-1]+(i>=2&&mn[i]==i);
    }

    bool is_prime(ll x)const{
        return 2<=x&&x<=n&&mn[x]==x;
    }

    ll spf(ll x)const{
        return mn[x];
    }

    ll prime_id(ll x)const{
        return 0<=x&&x<=n?id[x]:-1;
    }

    ll prime_count(ll x)const{
        if(x<1) return 0;
        return pi[min(x,n)];
    }

    vp factor(ll x)const{
        vp r;
        while(x>1){
            ll p=mn[x],e=0;
            while(mn[x]==p) x/=p,e++;
            r.pb({p,e});
        }
        return r;
    }

    vl factor_list(ll x)const{
        vl r;
        while(x>1){
            r.pb(mn[x]);
            x/=mn[x];
        }
        return r;
    }

    vl factor_unique(ll x)const{
        vl r;
        while(x>1){
            ll p=mn[x];
            r.pb(p);
            while(x>1&&mn[x]==p) x/=p;
        }
        return r;
    }

    ll rad(ll x)const{
        ll r=1;
        while(x>1){
            ll p=mn[x];
            r*=p;
            while(x>1&&mn[x]==p) x/=p;
        }
        return r;
    }

    static uint64_t splitmix64(uint64_t x){
        x+=0x9e3779b97f4a7c15ULL;
        x=(x^(x>>30))*0xbf58476d1ce4e5b9ULL;
        x=(x^(x>>27))*0x94d049bb133111ebULL;
        return x^(x>>31);
    }

    ll factor_zobrist(ll x)const{
        uint64_t h=0;
        while(x>1){
            ll p=mn[x];
            h^=splitmix64(p);
            while(x>1&&mn[x]==p) x/=p;
        }
        return (ll)h;
    }

    vl divisors(ll x)const{
        vl r={1};
        for(auto [p,e]:factor(x)){
            ll s=r.size(),q=1;
            rep(k,e){
                q*=p;
                rep(i,s) r.pb(r[i]*q);
            }
        }
        sort(all(r));
        return r;
    }

    ll omega(ll x)const{
        ll r=0;
        while(x>1){
            ll p=mn[x];
            r++;
            while(x>1&&mn[x]==p) x/=p;
        }
        return r;
    }

    ll Omega(ll x)const{
        ll r=0;
        while(x>1){
            x/=mn[x];
            r++;
        }
        return r;
    }

    ll phi(ll x)const{
        ll r=x;
        while(x>1){
            ll p=mn[x];
            r=r/p*(p-1);
            while(x>1&&mn[x]==p) x/=p;
        }
        return r;
    }

    ll mobius(ll x)const{
        ll r=1;
        while(x>1){
            ll p=mn[x];
            x/=p;
            if(x>1&&mn[x]==p) return 0;
            r=-r;
        }
        return r;
    }
};

vmi divisor_zeta(vmi a,const LinearSieve& S){
    ll n=a.size()-1;
    for(ll p:S.primes){
        if(p>n) break;
        for(ll i=1;i*p<=n;i++)
            a[i*p]+=a[i];
    }
    return a;
}

vmi divisor_mobius(vmi a,const LinearSieve& S){
    ll n=a.size()-1;
    for(ll p:S.primes){
        if(p>n) break;
        for(ll i=n/p;i>=1;i--)
            a[i*p]-=a[i];
    }
    return a;
}

vmi multiple_zeta(vmi a,const LinearSieve& S){
    ll n=a.size()-1;
    for(ll p:S.primes){
        if(p>n) break;
        for(ll i=n/p;i>=1;i--)
            a[i]+=a[i*p];
    }
    return a;
}

vmi multiple_mobius(vmi a,const LinearSieve& S){
    ll n=a.size()-1;
    for(ll p:S.primes){
        if(p>n) break;
        for(ll i=1;i*p<=n;i++)
            a[i]-=a[i*p];
    }
    return a;
}

/*
LinearSieve S(N)  N以下を線形篩で前計算 / LinearSieve S(N) / O(N)

bool is_prime(x)        素数か / S.is_prime(x) / 判定 / O(1)
ll spf(x)               最小素因数 / S.spf(x) / 最小素因数取得 / O(1)
ll prime_id(x)          素数の番号、非素数は-1 / S.prime_id(x) / ID取得 / O(1)
ll prime_count(x)       x以下の素数個数 / S.prime_count(x) / 素数計数 / O(1)

vp factor(x)            {{素因数,指数}} / S.factor(x) / 素因数分解 / O(log x)
vl factor_list(x)       重複込み素因数列 / S.factor_list(x) / 12→{2,2,3} / O(log x)
vl factor_unique(x)     重複なし昇順素因数列 / S.factor_unique(x) / 12→{2,3} / O(log x)
ll rad(x)               重複なし素因数の積 / S.rad(x) / 12→6 / O(log x)
ll factor_zobrist(x)    素因数集合のHash / S.factor_zobrist(x) / 指数無視 / O(log x)

vl divisors(x)          約数を昇順列挙 / S.divisors(x) / 約数列挙 / O(τ(x)logτ(x))
ll omega(x)             異なる素因数数 / S.omega(x) / 12→2 / O(log x)
ll Omega(x)             重複込み素因数数 / S.Omega(x) / 12→3 / O(log x)
ll phi(x)               Eulerのφ関数 / S.phi(x) / φ(x) / O(log x)
ll mobius(x)            Möbius関数μ(x) / S.mobius(x) / μ(x) / O(log x)

S.primes                N以下の素数を昇順保持
S.mn[x]                 xの最小素因数
S.id[p]                 素数pの0-indexed番号、非素数は-1
S.pi[x]                 x以下の素数個数

vmi divisor_zeta(a,S)    b[x]=Σ_{d|x}a[d] / divisor_zeta(a,S) / 約数和 / O(N loglogN)
vmi divisor_mobius(a,S)  約数ゼータの逆変換 / divisor_mobius(a,S) / 復元 / O(N loglogN)
vmi multiple_zeta(a,S)   b[x]=Σ_{x|m}a[m] / multiple_zeta(a,S) / 倍数和 / O(N loglogN)
vmi multiple_mobius(a,S) 倍数ゼータの逆変換 / multiple_mobius(a,S) / 復元 / O(N loglogN)

*/

