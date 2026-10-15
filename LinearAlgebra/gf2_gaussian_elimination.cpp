template<size_t W>
struct GF2Ans{
    bool ok;
    ll rank;
    vector<ll> x;
};

template<size_t W>
GF2Ans<W> GF2Gaussian(vector<bitset<W>> a){
    ll n=a.size(),m=W-1,r=0;
    vector<ll> where(m,-1);

    for(ll col=0;col<m&&r<n;col++){
        ll p=r;
        while(p<n&&!a[p][col])p++;
        if(p==n)continue;

        swap(a[p],a[r]);
        for(ll i=0;i<n;i++)if(i!=r&&a[i][col])a[i]^=a[r];
        where[col]=r++;
    }

    for(ll i=0;i<n;i++){
        bool zero=true;
        for(ll j=0;j<m;j++)if(a[i][j])zero=false;
        if(zero&&a[i][m])return {false,r,{}};
    }

    vector<ll> x(m);
    for(ll j=0;j<m;j++)if(where[j]!=-1)x[j]=a[where[j]][m];
    return {true,r,x};
}

/*
GF2Ans<W> GF2Gaussian(a) XOR連立方程式を解き、存在判定・rank・解の1つを返す O(N*M^2/64)

res.ok 解が存在するか
res.rank 係数行列のrank
res.x 解の1つ、自由変数は0

const ll M=100;
vector<bitset<M+1>> a(N);
auto res=GF2Gaussian(a);

a[i][0..M-1] 係数
a[i][M] 右辺
方程式は全てmod2、つまりXORとして扱う
解が複数ある場合も1つだけ返す
*/