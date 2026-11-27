struct KruskalResult{
    LL weight=0;
    ll components=0;
    vl edge_ids;
};

KruskalResult kruskal(ll n,const vector<tu>& edges){
    if(n<0||n>INT_MAX||edges.size()>static_cast<size_t>(LLONG_MAX))std::abort();
    vl order;
    order.reserve(edges.size());
    for(size_t id=0;id<edges.size();++id){
        const auto& [u,v,w]=edges[id];
        if(u<0||u>=n||v<0||v>=n)std::abort();
        if(u!=v)order.push_back(static_cast<ll>(id));
    }
    sort(order.begin(),order.end(),[&](ll a,ll b){
        return tie(get<2>(edges[a]),a)<tie(get<2>(edges[b]),b);
    });
    atcoder::dsu uf(static_cast<int>(n));
    KruskalResult result{0,n,{}};
    for(ll id:order){
        const auto& [u,v,w]=edges[id];
        const int a=uf.leader(static_cast<int>(u)),b=uf.leader(static_cast<int>(v));
        if(a==b)continue;
        uf.merge(a,b);
        result.weight+=w;
        --result.components;
        result.edge_ids.push_back(id);
    }
    return result;
}

/*
細かい注意点
無向辺をtu{u,v,w}で渡す。頂点・辺番号は0-indexed。重みwはllの全範囲。
0<=n<=INT_MAX、edges.size()<=LLONG_MAX、各端点は[0,n)。違反時はメッセージなしで停止。
連結でなければ各連結成分の最小全域木を合わせた森を返す。孤立点も1成分。自己ループを除外し、多重辺は別々に扱う。
同じ重みでは元の入力の辺番号順に試す。入力は変更しない。合計はLLで返す。
基本テンプレートのll・LL・vl・tuとatcoder::dsuを使う。ACLを個別に読むテンプレートではatcoder/dsuが必要。

使える関数一覧
KruskalResult kruskal(n,edges); // 最小全域森、O(n+m log(m+1))時間・O(n+m)追加領域
result.weight;                 // LL: 森の重み合計、参照O(1)
result.components;             // ll: 孤立点を含む連結成分数、参照O(1)
result.edge_ids;               // vl: 採用順の入力辺番号、参照O(1)、全走査O(n-components)

基本の関数
vector<tu> edges={{0,1,4},{0,1,-2},{1,2,3},{0,2,3},{3,3,-9}};
auto forest=kruskal(4,edges);
assert(forest.weight==1 && forest.components==2);
assert((forest.edge_ids==vl{1,2}));
*/
