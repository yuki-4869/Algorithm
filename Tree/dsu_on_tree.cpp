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
細かい注意点
0-indexed、無向木。入力はgraph=vector<vector<ll>>。
answer(v)の時点で、根付き木のvの部分木全体が現在のデータ構造に入っている。
addとdelは互いに逆の処理を実装する。各コールバックO(1)なら全体O(N log N)。
内部の再帰を使うため、深い木ではスタック容量に注意する。

使える関数一覧
DSUonTree dsu(g);                 // 木から構築 O(N)
dsu.run(add,del,answer,root=0);    // void: 全部分木クエリを処理 O(N log N)
add(v);                          // 頂点vを追加する利用者定義の処理
del(v);                          // 頂点vを削除する利用者定義の処理
answer(v);                       // vの部分木の答えを保存する利用者定義の処理

基本の関数
graph g={{1,2},{0},{0}};
DSUonTree dsu(g);
ll count=0; vl answers(3);
auto add=[&](ll v){++count;};
auto del=[&](ll v){--count;};
auto answer=[&](ll v){answers[v]=count;};
dsu.run(add,del,answer);                      // {3,1,1}
*/
