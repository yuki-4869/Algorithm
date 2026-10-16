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

    void down_keep(){
        do { if (!(n>=1)) std::abort(); } while (false);
        sum=(sum+nCr(n-1,k))*inv2;
        n--;
    }

    void down_decrease(){
        do { if (!(n>=1)) std::abort(); } while (false);
        sum=(sum-nCr(n-1,k))*inv2;
        n--; k--;
    }

    void up_keep(){
        sum=sum*2-nCr(n,k);
        n++;
    }

    void up_increase(){
        sum=sum*2+nCr(n,k+1);
        n++; k++;
    }

    void shift(ll dn,ll dk){
        if(dn>=0){
            do { if (!(0<=dk&&dk<=dn)) std::abort(); } while (false);
            rep(i,dk) up_increase();
            rep(i,dn-dk) up_keep();
        }else{
            ll d=-dn;
            do { if (!(-d<=dk&&dk<=0)) std::abort(); } while (false);
            rep(i,-dk) down_decrease();
            rep(i,d+dk) down_keep();
        }
    }

    mint shifted_value(ll dn,ll dk) const{
        auto tmp=*this;
        tmp.shift(dn,dk);
        return tmp.value();
    }
};

/*
細かい注意点
F(N,K)=Σ_{i=0}^K C(N,i)。N>=0。mintとnCrを基本テンプレートから使用する。
最大Nまでfactorialで先に前計算すれば、各1段の遷移O(1)、shiftはO(|dn|)。
dn>=0なら0<=dk<=dn、dn<0ならdn<=dk<=0。遷移後もN>=0。
down系はN>=1でのみ使用する。K<0の累積和は0、K>NはNまでの和。

使える関数一覧
binom_prefix_sum(N,K);       // mint: 累積和 O(max(0,min(N,K)+1))、前計算後
BinomPrefixState state(N,K); // 同じ累積和で初期化
state.value();              // mint: 現在値 O(1)
state.down_keep();          // void: (N,K)->(N-1,K) O(1)
state.down_decrease();      // void: (N,K)->(N-1,K-1) O(1)
state.up_keep();            // void: (N,K)->(N+1,K) O(1)
state.up_increase();        // void: (N,K)->(N+1,K+1) O(1)
state.shift(dn,dk);         // void: 状態を移動 O(|dn|)
state.shifted_value(dn,dk); // mint: 状態を変えず遷移先の値 O(|dn|)

基本の関数
factorial(10);
BinomPrefixState state(4,2);                  // 11
state.shift(1,1);                             // N=5,K=3
auto value = state.value();                  // 26
auto next = state.shifted_value(-1,-1);       // 状態を変えず11
*/
