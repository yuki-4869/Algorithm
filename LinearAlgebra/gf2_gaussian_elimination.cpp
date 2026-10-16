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
細かい注意点
GF(2)上の連立方程式。bitset<W>の下位W-1ビットが係数、最上位ビットが右辺。
Wはコンパイル時定数でW>=1。変数数M=W-1。入力は値渡しで変更されない。
解なしならok=false、自由変数は0。rankは右辺を除く係数行列のrank。

使える関数一覧
GF2Gaussian(a);      // GF2Ans<W>: XOR連立方程式を解く
result.ok;          // bool: 解が存在するか
result.rank;        // ll: 係数行列のrank
result.x;           // vector<ll>: 自由変数0の0/1解

基本の関数
vector<bitset<3>> equations={bitset<3>(0b111),bitset<3>(0b010)};
auto result = GF2Gaussian(equations);
if(result.ok){auto solution=result.x;}        // {1,0}
*/
