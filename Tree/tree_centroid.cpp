struct CentroidInfo {
    vl centroids,parent,subtree,max_component;
    ll balance=0;

    ll one() const {
        return centroids.empty() ? -1 : centroids[0];
    }

    bool is_centroid(ll v) const {
        return 0<=v&&v<(ll)max_component.size()&&max_component[v]==balance;
    }
};

inline ll centroid_to(ll v){
    return v;
}

template<class T,class U>
ll centroid_to(const pair<T,U>& e){
    return (ll)e.first;
}

template<class G>
CentroidInfo centroid_info_impl(const G& g,const vl& w){
    ll n=g.size();
    CentroidInfo res;
    if(n==0) return res;

    do { if (!((ll)w.size()==n)) std::abort(); } while (false);

    res.parent.assign(n,-2);
    res.subtree.assign(n,0);
    res.max_component.assign(n,0);

    vl order;
    order.reserve(n);

    stack<ll> st;
    st.push(0);
    res.parent[0]=-1;

    while(!st.empty()){
        ll v=st.top();
        st.pop();
        order.pb(v);

        for(const auto& e:g[v]){
            ll to=centroid_to(e);
            if(res.parent[to]!=-2) continue;
            res.parent[to]=v;
            st.push(to);
        }
    }

    do { if (!((ll)order.size()==n)) std::abort(); } while (false);

    for(ll v:order) res.subtree[v]=w[v];

    for(ll i=n-1;i>0;i--){
        ll v=order[i];
        res.subtree[res.parent[v]]+=res.subtree[v];
    }

    ll total=res.subtree[0];
    res.balance=total;

    rep(v,n){
        ll mx=total-res.subtree[v];

        for(const auto& e:g[v]){
            ll to=centroid_to(e);
            if(res.parent[to]==v) chmax(mx,res.subtree[to]);
        }

        res.max_component[v]=mx;

        if(mx<res.balance){
            res.balance=mx;
            res.centroids.clear();
        }
        if(mx==res.balance) res.centroids.pb(v);
    }

    return res;
}

template<class G>
CentroidInfo centroid_info(const G& g){
    return centroid_info_impl(g,vl(g.size(),1));
}

template<class G>
CentroidInfo centroid_info(const G& g,const vl& w){
    return centroid_info_impl(g,w);
}

template<class G>
vl centroid(const G& g){
    return centroid_info(g).centroids;
}

template<class G>
vl centroid(const G& g,const vl& w){
    return centroid_info(g,w).centroids;
}

template<class G>
ll centroid_one(const G& g){
    return centroid_info(g).one();
}

template<class G>
ll centroid_one(const G& g,const vl& w){
    return centroid_info(g,w).one();
}

/*
細かい注意点
0-indexed、連結な無向木。graphとGraphの両方に対応。
Graphの辺重みは重心計算には使わない。頂点重み付き版のw[v]は非負。
空の木ではcentroids={}、one()=-1。頂点重みが全て0の場合は全頂点が重心になり得る。
ci.parentは根0の親、ci.subtreeは部分木の頂点数/重み。

使える関数一覧
vl centroid(g) 木gの重心を全て返す、通常1個または2個 O(N)
ll centroid_one(g) 木gの重心を1つ返す、空なら-1 O(N)
CentroidInfo centroid_info(g) 重心・親・部分木サイズ・削除後最大成分サイズを全て返す O(N)

vl centroid(g,w) 頂点重みwに対する重心を全て返す O(N)
ll centroid_one(g,w) 頂点重みwに対する重心を1つ返す、空なら-1 O(N)
CentroidInfo centroid_info(g,w) 頂点重み付きの詳細情報を返す O(N)

ci.centroids 重心の頂点番号一覧
ci.parent[v] 根0とした親、根は-1
ci.subtree[v] vの部分木サイズ/部分木重み
ci.max_component[v] v削除後にできる最大連結成分のサイズ/重み
ci.balance 重心を削除したときの最大連結成分サイズ/重みの最小値
ci.one() 重心を1つ返す、存在しなければ-1 O(1)
ci.is_centroid(v) vが重心か判定 O(1)

基本の関数
graph g={{1},{0,2},{1}};
auto info = centroid_info(g);
auto vertex = info.one();                    // 1
auto all = centroid(g);                      // {1}
*/
