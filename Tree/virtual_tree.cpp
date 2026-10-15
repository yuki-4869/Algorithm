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
VirtualTreeResult VirtualTree(v,tin,dep,lca) 指定頂点と必要なLCAだけからなる圧縮木を返す O(KlogK)

vt.vertex Virtual Treeに含まれる頂点一覧
vt.edge 親→子の辺一覧
vt.root Virtual Treeの根、空なら-1

auto vt=VirtualTree(v,tin,depth,[&](ll a,ll b){return lca(a,b);});
for(auto [p,c]:vt.edge){}

v 元の木から抽出したい頂点
tin DFS順
dep 各頂点の深さ
lca(a,b) LCAを返す関数
Kは入力頂点数
元の頂点に加えて必要なLCAが自動追加される
*/