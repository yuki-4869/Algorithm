struct TreeDiameter{
    ll n = 0;
    ll diameter = 0;
    ll s = -1, t = -1;
    ll radius = 0;

    vl path;
    vl dist_s, dist_t;
    vl ecc;
    vl farthest;
    vl centers;
    vl diameter_pos;

private:
    tuple<ll,vl,vl> dist_from(const graph& g, ll start){
        vl dist(n, -1);
        vl par(n, -1);
        queue<ll> q;

        dist[start] = 0;
        q.push(start);

        while(!q.empty()){
            ll v = q.front();
            q.pop();

            for(auto nv : g[v]){
                if(dist[nv] != -1) continue;
                dist[nv] = dist[v] + 1;
                par[nv] = v;
                q.push(nv);
            }
        }

        ll far = start;

        rep(v,n){
            if(dist[v] > dist[far]) far = v;
        }

        return {far, dist, par};
    }

public:
    TreeDiameter(){}

    TreeDiameter(const graph& g){
        build(g);
    }

    void build(const graph& g){
        n = g.size();
        do { if (!(n >= 1)) std::abort(); } while (false);

        auto [a, d0, p0] = dist_from(g, 0);
        auto [b, da, par] = dist_from(g, a);
        auto [dummy, db, p1] = dist_from(g, b);

        s = a;
        t = b;

        dist_s = move(da);
        dist_t = move(db);

        diameter = dist_s[t];

        path.clear();

        for(ll v = t; v != -1; v = par[v]){
            path.pb(v);
        }

        reverse(all(path));

        diameter_pos.assign(n, -1);

        rep(i,(ll)path.size()){
            diameter_pos[path[i]] = i;
        }

        ecc.resize(n);
        farthest.resize(n);

        rep(v,n){
            if(dist_s[v] >= dist_t[v]){
                ecc[v] = dist_s[v];
                farthest[v] = s;
            }
            else{
                ecc[v] = dist_t[v];
                farthest[v] = t;
            }
        }

        radius = *min_element(all(ecc));

        centers.clear();

        rep(v,n){
            if(ecc[v] == radius){
                centers.pb(v);
            }
        }
    }

    ll get_farthest_dist(ll v) const{
        return ecc[v];
    }

    ll get_farthest_vertex(ll v) const{
        return farthest[v];
    }

    vl get_centers() const{
        return centers;
    }

    bool on_diameter(ll v) const{
        return diameter_pos[v] != -1;
    }
};

struct WeightedTreeDiameter{
    ll n = 0;
    ll diameter = 0;
    ll s = -1, t = -1;
    ll radius = 0;

    vl path;
    vl dist_s, dist_t;
    vl ecc;
    vl farthest;
    vl centers;
    vl diameter_pos;

private:
    tuple<ll,vl,vl> dist_from(const Graph& g, ll start){
        vl dist(n, -1);
        vl par(n, -1);
        stack<ll> st;

        dist[start] = 0;
        st.push(start);

        while(!st.empty()){
            ll v = st.top();
            st.pop();

            for(auto [nv,w] : g[v]){
                if(dist[nv] != -1) continue;
                dist[nv] = dist[v] + w;
                par[nv] = v;
                st.push(nv);
            }
        }

        ll far = start;

        rep(v,n){
            if(dist[v] > dist[far]) far = v;
        }

        return {far, dist, par};
    }

public:
    WeightedTreeDiameter(){}

    WeightedTreeDiameter(const Graph& g){
        build(g);
    }

    void build(const Graph& g){
        n = g.size();
        do { if (!(n >= 1)) std::abort(); } while (false);

        auto [a, d0, p0] = dist_from(g, 0);
        auto [b, da, par] = dist_from(g, a);
        auto [dummy, db, p1] = dist_from(g, b);

        s = a;
        t = b;

        dist_s = move(da);
        dist_t = move(db);

        diameter = dist_s[t];

        path.clear();

        for(ll v = t; v != -1; v = par[v]){
            path.pb(v);
        }

        reverse(all(path));

        diameter_pos.assign(n, -1);

        rep(i,(ll)path.size()){
            diameter_pos[path[i]] = i;
        }

        ecc.resize(n);
        farthest.resize(n);

        rep(v,n){
            if(dist_s[v] >= dist_t[v]){
                ecc[v] = dist_s[v];
                farthest[v] = s;
            }
            else{
                ecc[v] = dist_t[v];
                farthest[v] = t;
            }
        }

        radius = *min_element(all(ecc));

        centers.clear();

        rep(v,n){
            if(ecc[v] == radius){
                centers.pb(v);
            }
        }
    }

    ll get_farthest_dist(ll v) const{
        return ecc[v];
    }

    ll get_farthest_vertex(ll v) const{
        return farthest[v];
    }

    vl get_centers() const{
        return centers;
    }

    bool on_diameter(ll v) const{
        return diameter_pos[v] != -1;
    }
};

/*
細かい注意点
0-indexed、非空の連結な無向木。
無重みはgraph=vector<vector<ll>>、重み付きはGraph=vector<vector<pair<ll,ll>>>。
重み付き辺は{行き先,重み}、重みは非負、距離はlong longに収まること。
radius/centersは頂点を中心とする半径/中心。直径経路は同長のうち1つ。
get_centers()は中心の頂点番号を昇順のvlとしてコピーして返す。未構築なら空。
無重みの中心は1個または2個。重み0の辺がある木では3個以上の場合もある。
構築・再構築O(N)、メモリO(N)。get_centers()は中心の個数Kに対してO(K)。
それ以外の結果フィールド・取得関数はO(1)。

使える関数一覧
TreeDiameter td(g);           // graph = vector<vector<ll>>
WeightedTreeDiameter wtd(g);  // Graph = vector<vector<pair<ll,ll>>>

td.diameter;                  // 直径
td.s, td.t;                   // 直径の両端
td.path;                      // s -> t の直径上の頂点列
td.dist_s[v];                 // s から v までの距離
td.dist_t[v];                 // t から v までの距離
td.ecc[v];                    // v から最遠点までの距離
td.farthest[v];               // v から最遠な頂点の1つ
td.radius;                    // 頂点を中心とした木の半径
td.centers;                   // 中心となる頂点
td.diameter_pos[v];           // 直径上での位置、直径外なら -1
td.on_diameter(v);            // v が今回求めた直径上なら true
td.get_farthest_dist(v);      // v から最遠点までの距離
td.get_farthest_vertex(v);    // v から最遠な頂点の1つ
td.get_centers();             // 中心の頂点番号を昇順のvlで返す O(K)
wtd.get_centers();            // 重み付き木の中心もすべて返す O(K)

td.build(g);                  // 再構築 O(N)
wtd.build(g);                 // 再構築 O(N)

基本の関数
graph g={{1},{0,2},{1}};
TreeDiameter td(g);
auto diameter = td.diameter;                 // 2
auto path = td.path;                         // 直径上の頂点列
auto distance = td.get_farthest_dist(1);      // 1
auto centers = td.get_centers();             // {1}
*/
