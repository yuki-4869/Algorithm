struct OfflineDynamicConnectivity{
    struct RUF{
        vector<ll> parent_,sz;
        vector<pair<ll,ll>> hist;

        RUF(ll n):parent_(n),sz(n,1){iota(parent_.begin(),parent_.end(),0);}

        ll root(ll x){while(x!=parent_[x])x=parent_[x];return x;}
        bool same(ll a,ll b){return root(a)==root(b);}

        void unite(ll a,ll b){
            a=root(a);b=root(b);
            if(a==b)return;
            if(sz[a]<sz[b])swap(a,b);
            hist.push_back({b,sz[a]});
            parent_[b]=a;sz[a]+=sz[b];
        }

        ll snapshot(){return hist.size();}

        void rollback(ll s){
            while(hist.size()>s){
                auto [b,old]=hist.back();hist.pop_back();
                ll a=parent_[b];sz[a]=old;parent_[b]=b;
            }
        }
    };

    struct Query{ll u,v,id;};

    ll n,T,qcnt=0;
    vector<vector<pair<ll,ll>>> seg;
    vector<vector<Query>> qs;
    map<pair<ll,ll>,ll> open;

    OfflineDynamicConnectivity(ll n,ll T):n(n),T(T),seg(4*T),qs(T){}

    pair<ll,ll> norm(ll u,ll v){
        if(u>v)swap(u,v);
        return {u,v};
    }

    void addseg(ll k,ll l,ll r,ll a,ll b,pair<ll,ll> e){
        if(r<=a||b<=l)return;
        if(a<=l&&r<=b){seg[k].push_back(e);return;}
        ll m=(l+r)/2;
        addseg(k*2,l,m,a,b,e);
        addseg(k*2+1,m,r,a,b,e);
    }

    void add(ll t,ll u,ll v){open[norm(u,v)]=t;}

    void erase(ll t,ll u,ll v){
        auto e=norm(u,v);
        addseg(1,0,T,open[e],t,e);
        open.erase(e);
    }

    ll query(ll t,ll u,ll v){
        ll id=qcnt++;
        qs[t].push_back({u,v,id});
        return id;
    }

    void dfs(ll k,ll l,ll r,RUF& uf,vector<ll>& ans){
        ll s=uf.snapshot();
        for(auto [u,v]:seg[k])uf.unite(u,v);

        if(r-l==1){
            for(auto q:qs[l])ans[q.id]=uf.same(q.u,q.v);
        }else{
            ll m=(l+r)/2;
            dfs(k*2,l,m,uf,ans);
            dfs(k*2+1,m,r,uf,ans);
        }
        uf.rollback(s);
    }

    vector<ll> solve(){
        for(auto [e,l]:open)addseg(1,0,T,l,T,e);
        vector<ll> ans(qcnt);
        RUF uf(n);
        dfs(1,0,T,uf,ans);
        return ans;
    }
};

/*
細かい注意点
0-indexed、無向グラフ。時刻は0..T-1。
同じ辺をaddからeraseの間に再addしない。eraseされなかった辺はTまで有効。
全操作を登録してからsolveする。queryの返すIDで答え配列を参照する。
内部はRollback UnionFindと時間セグ木。

使える関数一覧
OfflineDynamicConnectivity dc(N,T); // 構築 O(N+T)
dc.add(t,u,v);                     // void: 時刻tから辺を追加
dc.erase(t,u,v);                   // void: 時刻tで辺を削除
dc.query(t,u,v);                   // ll: 連結判定を登録しIDを返す
dc.solve();                        // vector<ll>: ID順の0/1回答 O((T+Q log T)log N)

基本の関数
OfflineDynamicConnectivity dc(3,3);
dc.add(0,0,1);
auto id = dc.query(1,0,1);
dc.erase(2,0,1);
auto answers = dc.solve();                   // answers[id]==1
*/
