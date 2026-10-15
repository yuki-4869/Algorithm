// 二項係数の累積和
mint binom_prefix_sum(ll n,ll k){
    if(k<0) return 0;
    k=min(k,n);
    mint ret=0;
    rep(i,k+1) ret+=nCr(n,i);
    return ret;
}

struct BinomPrefixState{
    ll n,k;
    mint sum;
    static inline const mint inv2=mint(2).inv();

    BinomPrefixState(ll n_,ll k_):n(n_),k(k_),sum(binom_prefix_sum(n_,k_)){}

    mint value() const{return sum;}

    // F(n,k) -> F(n-1,k)
    void down_keep(){
        assert(n>=1);
        sum=(sum+nCr(n-1,k))*inv2;
        n--;
    }

    // F(n,k) -> F(n-1,k-1)
    void down_decrease(){
        assert(n>=1);
        sum=(sum-nCr(n-1,k))*inv2;
        n--; k--;
    }

    // F(n,k) -> F(n+1,k)
    void up_keep(){
        sum=sum*2-nCr(n,k);
        n++;
    }

    // F(n,k) -> F(n+1,k+1)
    void up_increase(){
        sum=sum*2+nCr(n,k+1);
        n++; k++;
    }

    // F(n,k) -> F(n+dn,k+dk)
    void shift(ll dn,ll dk){
        if(dn>=0){
            assert(0<=dk&&dk<=dn);
            rep(i,dk) up_increase();
            rep(i,dn-dk) up_keep();
        }else{
            ll d=-dn;
            assert(-d<=dk&&dk<=0);
            rep(i,-dk) down_decrease();
            rep(i,d+dk) down_keep();
        }
    }

    // 状態を変更せずF(n+dn,k+dk)を取得
    mint shifted_value(ll dn,ll dk) const{
        auto tmp=*this;
        tmp.shift(dn,dk);
        return tmp.value();
    }
};

/*
BinomPrefixState state(N,K); // Σ_{i=0}^K C(N,i)で初期化 O(K)
state.shift(dn,dk);          // F(N,K)->F(N+dn,K+dk) O(|dn|)
state.shifted_value(dn,dk);  // 状態を変えず遷移先の値を取得 O(|dn|)
state.value();               // 現在の値を取得 O(1)
*/