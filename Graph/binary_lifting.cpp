template<class T>
struct BinaryLifting{
    ll LOG=64,n; T e;
    vector<vector<ll>> to;
    vector<vector<T>> dat;
    function<T(T,T)> op;

    BinaryLifting(vector<ll> f,vector<T> a,T e,function<T(T,T)> op)
        :n(f.size()),e(e),op(op){
        to.assign(LOG,vector<ll>(n));
        dat.assign(LOG,vector<T>(n));
        to[0]=f;dat[0]=a;

        for(ll k=1;k<LOG;k++)for(ll v=0;v<n;v++){
            to[k][v]=to[k-1][to[k-1][v]];
            dat[k][v]=op(dat[k-1][v],dat[k-1][to[k-1][v]]);
        }
    }

    pair<ll,T> query(ll v,unsigned long long k){
        T res=e;
        for(ll i=0;i<LOG;i++)if(k>>i&1){
            res=op(res,dat[i][v]);
            v=to[i][v];
        }
        return {v,res};
    }
    ll jump(ll v,unsigned long long k){return query(v,k).first;}
    T prod(ll v,unsigned long long k){return query(v,k).second;}
};

/*
pair<ll,T> query(v,k) k回移動後の頂点と途中の集約値を返す O(logk)
ll jump(v,k) vからk回移動した頂点を返す O(logk)
T prod(v,k) vからk回分の値をopで集約して返す O(logk)

BinaryLifting<ll> db(to,cost,0LL,[](ll a,ll b){return a+b;});
auto [v,sum]=db.query(s,k);
ll v=db.jump(s,k);
ll sum=db.prod(s,k);

to[v] vから1回移動した頂点
cost[v] vから1回移動するときに集約する値
e opの単位元
opは結合則を満たす演算を指定
sum/min/max/gcdなどに対応
初期構築 O(NlogK)
*/