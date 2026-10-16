struct VirtualTreeResult{
    vector<ll> vertex;
    vector<pair<ll,ll>> edge;
    ll root=-1;
};

template<class F>
VirtualTreeResult VirtualTree(vector<ll> v,
    const vector<ll>& tin,const vector<ll>& dep,F lca){
    if(v.empty())return {};
    sort(v.begin(),v.end(),[&](ll a,ll b){return tin[a]<tin[b];});
    v.erase(unique(v.begin(),v.end()),v.end());

    ll m=v.size();
    for(ll i=0;i+1<m;i++)v.push_back(lca(v[i],v[i+1]));

    sort(v.begin(),v.end(),[&](ll a,ll b){return tin[a]<tin[b];});
    v.erase(unique(v.begin(),v.end()),v.end());

    vector<ll> st;
    vector<pair<ll,ll>> edge;

    for(ll x:v){
        while(st.size()&&dep[st.back()]>=dep[x])st.pop_back();
        if(st.size())edge.push_back({st.back(),x});
        st.push_back(x);
    }
    return {v,edge,v[0]};
}

/*
細かい注意点
0-indexed、元の木のDFS順tin・深さdep・LCA関数を渡す。
入力頂点の重複は除去され、必要なLCAが自動で追加される。空入力のrootは-1。
Kは入力頂点数、LCA1回の時間をLとするとO(K log K+K L)。
HLDなどの前計算は別途必要。結果の頂点番号は元の木の番号のまま。

使える関数一覧
VirtualTree(v,tin,dep,lca); // VirtualTreeResult: 圧縮木を返す
tree.vertex;               // vector<ll>: 圧縮木の頂点一覧
tree.edge;                 // vector<pair<ll,ll>>: 親->子の辺列
tree.root;                 // ll: 根、空なら-1

基本の関数
vl selected={1,2}, tin={0,1,2}, depth={0,1,1};
auto lca = [](ll a,ll b){return a==b?a:0LL;};
auto tree = VirtualTree(selected,tin,depth,lca);
auto root = tree.root;                       // 0
auto edges = tree.edge;                      // 親->子の辺列
*/
