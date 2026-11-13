struct ClosestPairResult{
    LL squared_distance=0;
    pll indices={-1,-1};
};
ClosestPairResult closest_pair(const vector<pll>&points){
    if(points.size()>static_cast<size_t>(INT_MAX))std::abort();
    for(auto [x,y]:points)if(x<-1000000000000000000LL||x>1000000000000000000LL||y<-1000000000000000000LL||y>1000000000000000000LL)std::abort();
    const ll n=static_cast<ll>(points.size());
    if(n<2)return {};
    struct Point{ll x,y,id;};
    vector<Point> p; p.reserve(n);
    for(ll i=0;i<n;++i)p.push_back({points[i].first,points[i].second,i});
    sort(p.begin(),p.end(),[](const Point&a,const Point&b){return tie(a.x,a.y,a.id)<tie(b.x,b.y,b.id);});
    for(ll i=1;i<n;++i)if(p[i-1].x==p[i].x&&p[i-1].y==p[i].y)return {0,minmax(p[i-1].id,p[i].id)};
    auto distance=[](const Point&a,const Point&b)->LL{const LL x=LL(a.x)-b.x,y=LL(a.y)-b.y;return x*x+y*y;};
    ClosestPairResult result{distance(p[0],p[1]),minmax(p[0].id,p[1].id)};
    auto update=[&](const Point&a,const Point&b){
        const LL d=distance(a,b);
        if(d<result.squared_distance){result.squared_distance=d;result.indices=minmax(a.id,b.id);}
    };
    auto by_y=[](const Point&a,const Point&b){return tie(a.y,a.x,a.id)<tie(b.y,b.x,b.id);};
    vector<Point> buffer(n),strip;strip.reserve(n);
    auto solve=[&](auto&&self,ll l,ll r)->void{
        if(r-l<=3){
            for(ll i=l;i<r;++i)for(ll j=l;j<i;++j)update(p[i],p[j]);
            sort(p.begin()+l,p.begin()+r,by_y);return;
        }
        const ll mid=(l+r)/2,middle_x=p[mid].x;
        self(self,l,mid);self(self,mid,r);
        merge(p.begin()+l,p.begin()+mid,p.begin()+mid,p.begin()+r,buffer.begin()+l,by_y);
        copy(buffer.begin()+l,buffer.begin()+r,p.begin()+l);
        strip.clear();
        for(ll i=l;i<r;++i){
            const LL x=LL(p[i].x)-middle_x;
            if(x*x>=result.squared_distance)continue;
            for(ll j=static_cast<ll>(strip.size())-1;j>=0;--j){
                const LL y=LL(p[i].y)-strip[j].y;
                if(y*y>=result.squared_distance)break;
                update(p[i],strip[j]);
            }
            strip.push_back(p[i]);
        }
    };
    solve(solve,0,n);return result;
}

/*
細かい注意点
ll,LL=__int128,pllは基本テンプレートの型エイリアス。追加include・別ライブラリは不要。
点はpll=(x,y)、各座標の絶対値は10^18以下、N=points.size()はINT_MAX以下。
範囲外の座標・点数はNDEBUGでもメッセージなしで停止。入力は変更せず、結果に入力の参照を残さない。
異なる入力添字i<jを返す。重複座標も別の点として扱い、2点あれば最小二乗距離は0。
N<2ではindices={-1,-1},squared_distance=0。添字が有効かを先に確認する。
最適解が複数あれば1組を返す。添字対の辞書順最小性は保証しない。
距離は平方根を取らず正確な二乗値を返す。最大8*10^36でLLに収まるが、llに収まるとは限らない。
LLは通常のcoutへ直接出力できない。実距離の近似はsqrtl((long double)result.squared_distance)で求める。
浮動小数点・動的な点の追加削除・全最適対の列挙には非対応。再帰の深さはO(log N)。

使える関数一覧
ClosestPairResult closest_pair(points); // 最近点対を取得 O(N log N)時間・追加O(N)領域、N<2ならO(1)
result.indices;                         // pll: 元の入力の0-indexed添字{i,j}、i<j。存在しなければ{-1,-1}、参照O(1)
result.squared_distance;                // LL: 最小ユークリッド距離の二乗。参照O(1)

基本の関数
vector<pll> points={{0,0},{10,10},{3,4},{3,5}};
auto result=closest_pair(points);
assert((result.indices==pll{2,3}));
assert(result.squared_distance==LL(1));
auto duplicate=closest_pair(vector<pll>{{7,-2},{0,0},{7,-2}});
assert((duplicate.indices==pll{0,2}) && duplicate.squared_distance==0);
auto missing=closest_pair(vector<pll>{{1,2}});
assert((missing.indices==pll{-1,-1}));
const ll B=1000000000000000000LL;
auto wide=closest_pair(vector<pll>{{-B,-B},{B,B}});
assert(wide.squared_distance==LL(8)*B*B);
*/
