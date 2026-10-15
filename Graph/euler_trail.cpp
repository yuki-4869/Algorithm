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
vl EulerTrail(n,e,directed=false) オイラー路の頂点列を返す、存在しなければ{} O(N+M)

auto ans=EulerTrail(n,edges); 無向グラフ
auto ans=EulerTrail(n,edges,true); 有向グラフ

e=vector<pair<ll,ll>> で辺{u,v}を渡す
全ての辺をちょうど1回通る頂点列を返す
オイラー閉路の場合も同じ関数で取得可能
辺が全て使えない場合は{}を返す
多重辺に対応
*/