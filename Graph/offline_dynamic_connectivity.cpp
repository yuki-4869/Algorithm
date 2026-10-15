struct OfflineDynamicConnectivity{
    struct RUF{
        vector<ll> p,sz;
        vector<pair<ll,ll>> hist;

        RUF(ll n):p(n),sz(n,1){iota(p.begin(),p.end(),0);}

        ll root(ll x){while(x!=p[x])x=p[x];return x;}
        bool same(ll a,ll b){return root(a)==root(b);}

        void unite(ll a,ll b){
            a=root(a);b=root(b);
            if(a==b)return;
            if(sz[a]<sz[b])swap(a,b);
            hist.push_back({b,sz[a]});
            p[b]=a;sz[a]+=sz[b];
        }

        ll snapshot(){return hist.size();}

        void rollback(ll s){
            while(hist.size()>s){
                auto [b,old]=hist.back();hist.pop_back();
                ll a=p[b];sz[a]=old;p[b]=b;
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
void add(t,u,v) 時刻tから辺(u,v)を追加 O(1)
void erase(t,u,v) 時刻tで辺(u,v)を削除 O(logT)
ll query(t,u,v) 時刻tでu,vが連結か調べるクエリを追加しIDを返す O(1)
vl solve() 全queryの連結判定をID順に返す O((Q+M)logQlogN)

OfflineDynamicConnectivity dc(n,T);
dc.add(0,u,v);
ll id=dc.query(1,a,b);
dc.erase(2,u,v);
auto ans=dc.solve();
bool connected=ans[id];

同じ辺はaddしてからeraseするまで再addしない前提
eraseされなかった辺はTまで存在するとして処理
queryの返り値IDでsolve()の答えを参照する
内部でRollback UnionFind+時間セグ木を使用
無向グラフ用
*/