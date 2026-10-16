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
細かい注意点
0-indexed。to[v]は1回移動先、cost[v]はその移動の集約値。同じ長さの配列を渡す。
全移動先は0..N-1。opは結合則を満たし、eは単位元。非可換な演算にも対応。
kはunsigned long long。64段を固定で走査するため、構築O(64N)、各照会O(64)。

使える関数一覧
BinaryLifting<T> db(to,cost,e,op); // 構築 O(64N)
db.query(v,k);                    // pair<ll,T>: k回後の頂点と集約値 O(64)
db.jump(v,k);                     // ll: k回後の頂点 O(64)
db.prod(v,k);                     // T: k回分の集約値 O(64)

基本の関数
vl to={1,2,0}, cost={2,3,5};
BinaryLifting<ll> db(to,cost,0LL,[](ll a,ll b){return a+b;});
auto [vertex,sum] = db.query(0,4);            // vertex=1, sum=12
auto next = db.jump(0,2);                     // 2
auto value = db.prod(0,2);                    // 5
*/
