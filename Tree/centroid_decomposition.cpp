class CentroidDecomposition{
    ll n_=0;
    vl parent_,depth_;
    vector<vector<pll>> ancestors_;
    void check_vertex(ll v)const{if(v<0||v>=n_)std::abort();}
public:
    CentroidDecomposition()=default;
    explicit CentroidDecomposition(const graph&g){build(g);}
    void build(const graph&g){
        if(g.size()>static_cast<size_t>(INT_MAX))std::abort();
        const ll n=static_cast<ll>(g.size());
        ll degree=0;
        for(ll v=0;v<n;++v){
            if(g[v].size()>static_cast<size_t>(n-1))std::abort();
            degree+=static_cast<ll>(g[v].size());
            for(ll u:g[v])if(u<0||u>=n||u==v)std::abort();
        }
        if(n>0&&degree!=2*(n-1))std::abort();
        vl tree_parent(n,-2),order;
        if(n>0){order.push_back(0);tree_parent[0]=-1;}
        for(size_t i=0;i<order.size();++i){
            const ll v=order[i];
            ll reverse_edges=0;
            for(ll u:g[v]){
                if(u==tree_parent[v]){++reverse_edges;continue;}
                if(tree_parent[u]!=-2)std::abort();
                tree_parent[u]=v;order.push_back(u);
            }
            if(reverse_edges!=(v==0?0:1))std::abort();
        }
        if(order.size()!=g.size())std::abort();
        n_=n;parent_=vl(n,-1);depth_=vl(n,0);ancestors_=vector<vector<pll>>(n);
        if(n==0)return;
        vl subtree(n);
        vector<char> removed(n);
        vector<tuple<ll,ll,ll>> tasks{{0,-1,0}};
        while(!tasks.empty()){
            auto [start,above,level]=tasks.back();tasks.pop_back();
            order.clear();order.push_back(start);tree_parent[start]=-1;
            for(size_t i=0;i<order.size();++i){
                const ll v=order[i];
                for(ll u:g[v])if(!removed[u]&&u!=tree_parent[v]){
                    tree_parent[u]=v;order.push_back(u);
                }
            }
            const ll component_size=static_cast<ll>(order.size());
            ll center=-1;
            for(auto it=order.rbegin();it!=order.rend();++it){
                const ll v=*it;
                subtree[v]=1;
                ll largest=0;
                for(ll u:g[v])if(!removed[u]&&tree_parent[u]==v){
                    subtree[v]+=subtree[u];largest=max(largest,subtree[u]);
                }
                largest=max(largest,component_size-subtree[v]);
                if(largest<=component_size/2)center=v;
            }
            parent_[center]=above;depth_[center]=level;
            vector<tuple<ll,ll,ll>> walk{{center,-1,0}};
            while(!walk.empty()){
                auto [v,p,d]=walk.back();walk.pop_back();
                ancestors_[v].emplace_back(center,d);
                for(ll u:g[v])if(!removed[u]&&u!=p)walk.emplace_back(u,v,d+1);
            }
            removed[center]=true;
            for(ll u:g[center])if(!removed[u])tasks.emplace_back(u,center,level+1);
        }
    }
    ll size()const{return n_;}
    ll parent(ll v)const{check_vertex(v);return parent_[v];}
    ll depth(ll v)const{check_vertex(v);return depth_[v];}
    const vector<pll>&ancestors(ll v)const{check_vertex(v);return ancestors_[v];}
};

/*
細かい注意点
ll,vl,pll,graphは基本テンプレートの型エイリアス。追加include・別ライブラリは不要。
0-indexedの重みなし無向木。両方向の辺を1回ずつ登録する。NはINT_MAX以下。空の木も可。
不正な木・頂点番号はNDEBUGでもメッセージなしで停止。入力の隣接リストは変更・保持しない。
頂点数を基準に成分を半分以下へ分割する重心分解。既存のcentroid_infoによる重心1回の計算とは別の処理。
parent/depthは重心木の親・深さ。元の木の親・深さではない。根のparentは-1、depthは0。
ancestors(v)は{祖先重心,元の木の辺数距離}を重心木の根からvまでの順で返す。最後は{v,0}。
重心が2個ならどちらを選ぶかは未規定。隣接リストの並び順で分解結果が変わり得る。
ancestorsの返り値はconst参照。build・代入・移動・破棄後は使わず、必要ならautoでコピーする。
動的な辺の追加削除・重み付き辺・森・頂点重み付き分解には非対応。再帰は使わない。

使える関数一覧
CentroidDecomposition cd;             // 空の状態を構築 O(1)
CentroidDecomposition cd(g);          // 木gを重心分解 O(N log N)時間・領域、空ならO(1)
cd.build(g);                          // void: 再構築 O(N log N)時間・領域、空ならO(1)。旧データ破棄の時間は別途
cd.size();                            // ll: 元の木の頂点数 O(1)
cd.parent(v);                         // ll: 重心木での親、根は-1 O(1)
cd.depth(v);                          // ll: 重心木での深さ O(1)
cd.ancestors(v);                      // const vector<pll>&: 祖先重心と元の木での距離、取得O(1)・全走査O(log N)

基本の関数
graph g={{1},{0,2,3},{1},{1,4},{3}};
CentroidDecomposition cd(g);
assert(cd.size()==5 && cd.parent(1)==-1 && cd.depth(1)==0);
assert((cd.ancestors(4).front()==pll{1,2}));
assert((cd.ancestors(4).back()==pll{4,0}));
vl best(cd.size(),inf);
auto mark=[&](ll v){for(auto [c,d]:cd.ancestors(v))best[c]=min(best[c],d);};
auto nearest=[&](ll v){ll answer=inf;for(auto [c,d]:cd.ancestors(v))if(best[c]!=inf)answer=min(answer,best[c]+d);return answer==inf?-1:answer;};
assert(nearest(2)==-1);
mark(0);mark(4);                          // 頂点を追加、1回O(log N)
assert(nearest(2)==2 && nearest(3)==1);   // 最も近い追加済み頂点までの距離、1回O(log N)
*/
