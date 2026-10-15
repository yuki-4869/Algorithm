struct DSUonTree{
    ll n,timer=0;
    vector<vector<ll>> g;
    vector<ll> sz,heavy,tin,tout,euler;

    DSUonTree(vector<vector<ll>> g,ll root=0):n(g.size()),g(g){
        sz.assign(n,0);heavy.assign(n,-1);
        tin.resize(n);tout.resize(n);euler.resize(n);
        dfs1(root,-1);
    }

    void dfs1(ll v,ll p){
        tin[v]=timer;euler[timer++]=v;sz[v]=1;
        for(ll u:g[v])if(u!=p){
            dfs1(u,v);sz[v]+=sz[u];
            if(heavy[v]==-1||sz[u]>sz[heavy[v]])heavy[v]=u;
        }
        tout[v]=timer;
    }

    template<class Add,class Del,class Ans>
    void dfs2(ll v,ll p,bool keep,Add& add,Del& del,Ans& answer){
        for(ll u:g[v])if(u!=p&&u!=heavy[v])dfs2(u,v,false,add,del,answer);
        if(heavy[v]!=-1)dfs2(heavy[v],v,true,add,del,answer);

        for(ll u:g[v])if(u!=p&&u!=heavy[v])
            for(ll i=tin[u];i<tout[u];i++)add(euler[i]);

        add(v);
        answer(v);

        if(!keep)
            for(ll i=tin[v];i<tout[v];i++)del(euler[i]);
    }

    template<class Add,class Del,class Ans>
    void run(Add add,Del del,Ans answer,ll root=0){
        dfs2(root,-1,true,add,del,answer);
    }
};
/*
void run(add,del,answer,root=0) 全頂点の部分木情報をDSU on Treeで処理する O(NlogN)

add(v) 頂点vを現在のデータ構造に追加
del(v) 頂点vを現在のデータ構造から削除
answer(v) vの部分木全体が入った状態で答えを保存

DSUonTree dsu(g);
dsu.run(add,del,answer);

auto add=[&](ll v){cnt[color[v]]++;};
auto del=[&](ll v){cnt[color[v]]--;};
auto answer=[&](ll v){ans[v]=...;};

answer(v)が呼ばれる時点でvの部分木全体が保持されている
色の種類数・頻度・最大頻度などの部分木クエリ向け
g=vector<vector<ll>>
*/