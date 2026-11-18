ll discrete_log(ll a,ll b,ll m){
    if(m<=0)std::abort();
    a%=m;b%=m;
    if(a<0)a+=m;
    if(b<0)b+=m;
    if(m==1 || b==1)return 0;
    if(a==0)return b==0?1:-1;
    if(a==1)return -1;
    auto mul=[&](ll x,ll y){return (ll)(LL(x)*y%m);};
    ll offset=0,k=1;
    while(true){
        ll g=std::gcd(a,m);
        if(g==1)break;
        if(b==k)return offset;
        if(b%g!=0)return -1;
        b/=g;m/=g;++offset;
        k=mul(k,a/g);
    }
    if(b==k)return offset;
    a%=m;
    ll width=(ll)sqrtl((long double)m);
    while(LL(width)*width<m)++width;
    while(width>0 && LL(width-1)*(width-1)>=m)--width;
    vector<pll> baby;
    baby.reserve((size_t)width);
    ll value=b;
    for(ll j=0;j<width;++j){baby.push_back({value,j});value=mul(value,a);}
    sort(baby.begin(),baby.end(),[](const pll& x,const pll& y){return x.first!=y.first?x.first<y.first:x.second>y.second;});
    baby.erase(unique(baby.begin(),baby.end(),[](const pll& x,const pll& y){return x.first==y.first;}),baby.end());
    ll step=1,base=a;
    for(ll e=width;e>0;e>>=1){if(e&1)step=mul(step,base);base=mul(base,base);}
    value=k;
    for(ll i=1;i<=width;++i){
        value=mul(value,step);
        auto it=lower_bound(baby.begin(),baby.end(),value,[](const pll& x,ll y){return x.first<y;});
        if(it!=baby.end() && it->first==value)return (ll)(LL(i)*width-it->second+offset);
    }
    return -1;
}

/*
細かい注意点
ll,LL=__int128,pllは基本テンプレートの型。追加include・ACL・個別ライブラリへの依存は不要。
1<=m<=LLONG_MAX。a,bはllの範囲全体に対応し、関数内で[0,m)へ正規化する。
a^x≡b (mod m)を満たす最小の非負整数xを返す。解なしなら-1。入力は変更しない。
x=0ではa=0でもa^0=1と定義。m=1では常に0。指数0も探索対象。
非互いに素なa,m・合成数mod・b=0にも対応。m<=0はNDEBUGでもメッセージなしで停止。
Mは元の法、Rは最大公約数を取り除いた後の法、W=ceil(sqrt(R))。
時間O(sqrt(M)log M)、追加領域O(sqrt(M))。M<=1の時間・領域はO(1)。
実際のBSGS部分はO(W log W)時間・O(W)領域。重複を除く前にW組を確保する。
Mが10^18規模では一般に実用的でない。必要なメモリ・実行時間を確保できることが前提。
積・平方根の補正・指数の組立は128bitで計算。浮動小数点の丸めによる誤答はない。
乱数やハッシュ表は使わず、ソートと二分探索で最小解を決定する。
複数クエリの前計算共有、全ての指数の列挙、離散対数の加法性は提供しない。

使える関数一覧
ll discrete_log(ll a,ll b,ll m); // 最小x>=0（a^x≡b mod m）、解なし-1 / O(sqrt(M)log M)時間・O(sqrt(M))領域

基本の関数
assert(discrete_log(2,8,13)==3);
assert(discrete_log(2,4,8)==2);
assert(discrete_log(2,0,8)==3);
assert(discrete_log(2,3,8)==-1);
assert(discrete_log(4,4,5)==1);
assert(discrete_log(-2,-8,13)==3);
assert(discrete_log(0,1,7)==0);
assert(discrete_log(0,0,7)==1);
assert(discrete_log(0,2,7)==-1);
assert(discrete_log(123,456,1)==0);
*/
