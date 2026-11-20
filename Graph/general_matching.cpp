struct GeneralMatchingResult{
    ll cardinality=0;
    vl mate,edge_ids;
};
GeneralMatchingResult general_matching(ll n,const vp& edges){
    if(n<0 || n>INT_MAX || edges.size()>static_cast<size_t>(LLONG_MAX))std::abort();
    const int N=(int)n;
    vector<vector<int>> g((size_t)N);
    for(auto [u,v]:edges){
        if(u<0 || u>=n || v<0 || v>=n)std::abort();
        if(u!=v){g[(size_t)u].push_back((int)v);g[(size_t)v].push_back((int)u);}
    }
    vector<int> seen((size_t)N,-1);
    for(int v=0;v<N;++v){
        size_t count=0;
        for(int u:g[(size_t)v])if(seen[(size_t)u]!=v){seen[(size_t)u]=v;g[(size_t)v][count++]=u;}
        g[(size_t)v].resize(count);
    }
    vector<int> match((size_t)N,-1),parent((size_t)N),base((size_t)N),queue;
    vector<char> used((size_t)N),flower((size_t)N),path((size_t)N);
    queue.reserve((size_t)N);
    auto lca=[&](int a,int b){
        fill(path.begin(),path.end(),false);
        while(true){
            a=base[(size_t)a];path[(size_t)a]=true;
            if(match[(size_t)a]==-1)break;
            a=parent[(size_t)match[(size_t)a]];
        }
        while(true){
            b=base[(size_t)b];
            if(path[(size_t)b])return b;
            b=parent[(size_t)match[(size_t)b]];
        }
    };
    auto mark_path=[&](int v,int b,int child){
        while(base[(size_t)v]!=b){
            const int m=match[(size_t)v];
            flower[(size_t)base[(size_t)v]]=flower[(size_t)base[(size_t)m]]=true;
            parent[(size_t)v]=child;
            child=m;v=parent[(size_t)m];
        }
    };
    auto augment=[&](int root){
        fill(used.begin(),used.end(),false);
        fill(parent.begin(),parent.end(),-1);
        iota(base.begin(),base.end(),0);
        queue.clear();queue.push_back(root);used[(size_t)root]=true;
        for(size_t head=0;head<queue.size();++head){
            const int v=queue[head];
            for(int u:g[(size_t)v]){
                if(base[(size_t)v]==base[(size_t)u] || match[(size_t)v]==u)continue;
                if(u==root || (match[(size_t)u]!=-1 && parent[(size_t)match[(size_t)u]]!=-1)){
                    const int b=lca(v,u);
                    fill(flower.begin(),flower.end(),false);
                    mark_path(v,b,u);mark_path(u,b,v);
                    for(int i=0;i<N;++i)if(flower[(size_t)base[(size_t)i]]){
                        base[(size_t)i]=b;
                        if(!used[(size_t)i]){used[(size_t)i]=true;queue.push_back(i);}
                    }
                }else if(parent[(size_t)u]==-1){
                    parent[(size_t)u]=v;
                    if(match[(size_t)u]==-1){
                        for(int cur=u;cur!=-1;){
                            const int p=parent[(size_t)cur],next=p==-1?-1:match[(size_t)p];
                            match[(size_t)cur]=p;
                            if(p!=-1)match[(size_t)p]=cur;
                            cur=next;
                        }
                        return true;
                    }
                    const int next=match[(size_t)u];
                    used[(size_t)next]=true;queue.push_back(next);
                }
            }
        }
        return false;
    };
    for(int v=0;v<N;++v)if(match[(size_t)v]==-1 && !g[(size_t)v].empty())augment(v);
    GeneralMatchingResult result;
    result.mate.assign(match.begin(),match.end());
    vector<char> emitted((size_t)N);
    for(size_t id=0;id<edges.size();++id){
        auto [u,v]=edges[id];
        if(u!=v && match[(size_t)u]==v && !emitted[(size_t)u]){
            emitted[(size_t)u]=emitted[(size_t)v]=true;
            result.edge_ids.push_back((ll)id);
        }
    }
    result.cardinality=(ll)result.edge_ids.size();
    return result;
}

/*
細かい注意点
ll,vl,vpは基本テンプレートの型。追加include・ACL・他の個別ライブラリへの依存は不要。
無重み・無向の一般グラフで、端点を共有せずに選べる辺数の最大値を求める。二部性は不要。
0<=N<=INT_MAX、M=edges.size()<=LLONG_MAX。各辺は{u,v}、頂点と辺番号は0-indexed。
自己ループは選べないので無視する。多重辺・逆向きの重複・孤立点・空グラフにも対応。
同じ頂点対の重複を探索前に除き、時間O(N^3+M)・追加領域O(N+M)。N=M=0はO(1)。
Blossom法の縮約・増加路探索は全て反復処理。実用上は数百頂点規模を主な対象とする。
重み付き最適化・最小頂点被覆・増分更新・辺削除には非対応。呼ぶたびに空の対応から計算する。
最大解は一意とは限らず、辺の入力順にも依存。辞書順最小などの最適解間の優先は保証しない。
採用した頂点対に複数の入力辺がある場合、その中で最小の辺番号を返す。
edge_idsは入力辺番号の昇順。edges[id]の端点の向きは入力どおり。入力配列は変更しない。
mate[v]は相手の頂点番号、未対応なら-1。mate[mate[v]]=vで、自己対応はしない。
負のN・Nの上限超過・範囲外の端点はNDEBUGでもメッセージなしで停止する。
必要なメモリを確保できることが前提。返り値は入力と独立した値で、書換後の整合性は自動では保たない。

使える関数一覧
GeneralMatchingResult general_matching(ll n,const vp& edges); // 最大マッチングを復元 O(N^3+M)時間・追加O(N+M)領域
result.cardinality; // ll: 最大マッチングの辺数K O(1)
result.mate;        // vl: 長さNの対応相手、未対応-1 1要素取得O(1)、全走査O(N)
result.edge_ids;    // vl: 長さKの採用辺番号、昇順 1要素取得O(1)、全走査O(K)

基本の関数
vp edges={{0,1},{1,2},{2,0},{2,3}};
auto result=general_matching(4,edges);
assert(result.cardinality==2);
assert((result.mate==vl{1,0,3,2}));
assert((result.edge_ids==vl{0,3}));
for(ll id:result.edge_ids){auto [u,v]=edges[(size_t)id];assert(result.mate[(size_t)u]==v);}
auto parallel=general_matching(2,vp{{0,0},{1,0},{0,1}});
assert(parallel.cardinality==1 && parallel.edge_ids==vl{1});
auto odd=general_matching(3,vp{{0,1},{1,2},{2,0}});
assert(odd.cardinality==1);
auto empty=general_matching(0,vp{});
assert(empty.cardinality==0 && empty.mate.empty() && empty.edge_ids.empty());
*/
