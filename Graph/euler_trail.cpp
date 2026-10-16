vector<ll> EulerTrail(ll n,vector<pair<ll,ll>> e,bool directed=false){
    ll m=e.size(),s=0;
    vector<vector<pair<ll,ll>>> g(n);
    vector<ll> in(n),out(n),used(m);

    for(ll i=0;i<m;i++){
        auto [a,b]=e[i];
        g[a].push_back({b,i});out[a]++;in[b]++;
        if(!directed)g[b].push_back({a,i}),out[b]++,in[a]++;
    }

    if(directed){
        ll st=0,en=0;
        for(ll v=0;v<n;v++){
            if(out[v]-in[v]==1)s=v,st++;
            else if(in[v]-out[v]==1)en++;
            else if(in[v]!=out[v])return {};
        }
        if(st!=en||st>1)return {};
        if(!st)for(ll v=0;v<n;v++)if(out[v]){s=v;break;}
    }else{
        vector<ll> odd;
        for(ll v=0;v<n;v++)if(out[v]&1)odd.push_back(v);
        if(odd.size()!=0&&odd.size()!=2)return {};
        if(odd.size())s=odd[0];
        else for(ll v=0;v<n;v++)if(out[v]){s=v;break;}
    }

    vector<ll> it(n),st={s},ans;
    while(st.size()){
        ll v=st.back();
        while(it[v]<g[v].size()&&used[g[v][it[v]].second])it[v]++;
        if(it[v]==g[v].size())ans.push_back(v),st.pop_back();
        else{
            auto [u,id]=g[v][it[v]++];
            used[id]=1;st.push_back(u);
        }
    }
    if(ans.size()!=m+1)return {};
    reverse(ans.begin(),ans.end());
    return ans;
}

/*
細かい注意点
0-indexed、N>=1。辺はvector<pair<ll,ll>>。多重辺・自己ループにも対応。
全辺をちょうど1回通る頂点列を返す。存在しない場合は空配列。
有向/無向のオイラー閉路も同じ関数で得られる。

使える関数一覧
EulerTrail(N,edges);       // vector<ll>: 無向オイラー路 O(N+M)
EulerTrail(N,edges,true);  // vector<ll>: 有向オイラー路 O(N+M)

基本の関数
vector<pair<ll,ll>> edges={{0,1},{1,2},{2,0}};
auto path = EulerTrail(3,edges);              // 無向オイラー路
auto directed_path = EulerTrail(3,edges,true);// 有向オイラー路
*/
