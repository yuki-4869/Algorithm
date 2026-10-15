template<class T>
struct GaussAns{
    bool ok;
    ll rank;
    vector<T> x;
};

template<class T>
GaussAns<T> GaussianElimination(vector<vector<T>> a,vector<T> b){
    ll n=a.size(),m=a[0].size(),r=0;
    vector<ll> where(m,-1);
    for(ll i=0;i<n;i++)a[i].push_back(b[i]);

    for(ll col=0;col<m&&r<n;col++){
        ll p=r;
        while(p<n&&a[p][col]==T(0))p++;
        if(p==n)continue;

        swap(a[p],a[r]);
        T z=a[r][col];
        for(ll j=col;j<=m;j++)a[r][j]/=z;

        for(ll i=0;i<n;i++)if(i!=r&&a[i][col]!=T(0)){
            T z=a[i][col];
            for(ll j=col;j<=m;j++)a[i][j]-=z*a[r][j];
        }
        where[col]=r++;
    }

    for(ll i=0;i<n;i++){
        bool zero=true;
        for(ll j=0;j<m;j++)if(a[i][j]!=T(0))zero=false;
        if(zero&&a[i][m]!=T(0))return {false,r,{}};
    }

    vector<T> x(m);
    for(ll j=0;j<m;j++)if(where[j]!=-1)x[j]=a[where[j]][m];
    return {true,r,x};
}

/*

GaussAns<T> GaussianElimination(A,b) Ax=bを解き、存在判定・rank・解の1つを返す O(N*M*min(N,M))

res.ok 解が存在するか
res.rank 行列Aのrank
res.x 解の1つ、自由変数は0

using mint=atcoder::modint998244353;
auto res=GaussianElimination<mint>(A,b);
if(res.ok)auto x=res.x;

A=vector<vector<T>>
b=vector<T>
modintなど四則演算と逆元が使える体を想定
解が複数ある場合も1つだけ返す
*/