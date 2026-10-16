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
細かい注意点
AはN行M列、bはN要素。要素型Tは四則演算と逆元を持つ体を想定する。
modintなどの厳密な演算用。浮動小数点の誤差判定は別途調整が必要。
解なしならok=false。自由変数を0とした解を1つだけ返す。
元のA,bは値渡しでコピーされ、変更されない。

使える関数一覧
GaussianElimination(A,b); // GaussAns<T>: Ax=bを解く O(N*M*min(N,M))
result.ok;               // bool: 解が存在するか
result.rank;             // ll: 係数行列のrank
result.x;                // vector<T>: 自由変数0の解

基本の関数
vector<vector<mint>> A={{1,1},{1,2}};
vector<mint> b={3,5};
auto result = GaussianElimination(A,b);
if(result.ok){auto solution=result.x;}        // {1,2}
*/
