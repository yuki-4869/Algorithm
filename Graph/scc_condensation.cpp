struct SCCResult {
    ll K;
    vl cmp;

    vector<vl> groups;

    graph dag;
    graph rdag;

    vl indeg;
    vl outdeg;

    vb cyclic;

    bool same(ll u, ll v) const {
        return cmp[u] == cmp[v];
    }

    ll size(ll c) const {
        return groups[c].size();
    }

    vl sources() const {
        vl res;
        rep(c,K){
            if(indeg[c] == 0) res.pb(c);
        }
        return res;
    }

    vl sinks() const {
        vl res;
        rep(c,K){
            if(outdeg[c] == 0) res.pb(c);
        }
        return res;
    }

    bool in_cycle(ll v) const {
        return cyclic[cmp[v]];
    }

    bool has_cycle() const {
        for(bool x : cyclic){
            if(x) return true;
        }
        return false;
    }
};

struct WeightedSCCResult : SCCResult {
    vb nonzero_cycle;

    bool has_nonzero_cycle() const {
        for(bool x : nonzero_cycle){
            if(x) return true;
        }
        return false;
    }
};

SCCResult SCCDecomposition(const graph& g){
    ll n = g.size();

    graph rg(n);

    rep(v,n){
        for(auto to : g[v]){
            rg[to].pb(v);
        }
    }

    vl order;
    order.reserve(n);

    vb used(n,false);

    rep(s,n){
        if(used[s]) continue;

        vector<pair<ll,ll>> st;
        st.pb({s,0});

        used[s] = true;

        while(!st.empty()){
            auto &[v,i] = st.back();

            if(i == (ll)g[v].size()){
                order.pb(v);
                st.pop_back();
                continue;
            }

            ll to = g[v][i++];

            if(!used[to]){
                used[to] = true;
                st.pb({to,0});
            }
        }
    }

    vl cmp(n,-1);
    vector<vl> groups;

    ll K = 0;

    for(ll i = n-1; i >= 0; i--){
        ll s = order[i];

        if(cmp[s] != -1) continue;

        groups.pb({});

        vl st = {s};
        cmp[s] = K;

        while(!st.empty()){
            ll v = st.back();
            st.pop_back();

            groups[K].pb(v);

            for(auto to : rg[v]){
                if(cmp[to] == -1){
                    cmp[to] = K;
                    st.pb(to);
                }
            }
        }

        K++;
    }

    graph dag(K), rdag(K);

    vl indeg(K,0);
    vl outdeg(K,0);

    vl seen(K,-1);

    rep(c,K){
        for(auto v : groups[c]){
            for(auto to : g[v]){
                ll d = cmp[to];

                if(c == d) continue;
                if(seen[d] == c) continue;

                seen[d] = c;

                dag[c].pb(d);
                rdag[d].pb(c);

                outdeg[c]++;
                indeg[d]++;
            }
        }
    }

    vb cyclic(K,false);

    rep(c,K){
        if(groups[c].size() >= 2){
            cyclic[c] = true;
        }
    }

    rep(v,n){
        for(auto to : g[v]){
            if(v == to){
                cyclic[cmp[v]] = true;
            }
        }
    }

    return {
        K,
        cmp,
        groups,
        dag,
        rdag,
        indeg,
        outdeg,
        cyclic
    };
}

WeightedSCCResult SCCDecomposition(const Graph& g){
    ll n = g.size();
    graph unweighted(n);

    rep(v,n){
        unweighted[v].reserve(g[v].size());
        for(auto [to,w] : g[v]){
            unweighted[v].pb(to);
        }
    }

    WeightedSCCResult res;
    static_cast<SCCResult&>(res) = SCCDecomposition(unweighted);
    res.nonzero_cycle.assign(res.K,false);

    vector<__int128> potential(n,0);
    vb seen(n,false);

    rep(c,res.K){
        ll root = res.groups[c][0];
        queue<ll> q;
        q.push(root);
        seen[root] = true;

        while(!q.empty()){
            ll v = q.front();
            q.pop();

            for(auto [to,w] : g[v]){
                if(res.cmp[to] != c) continue;
                __int128 next = potential[v] + (__int128)w;

                if(!seen[to]){
                    seen[to] = true;
                    potential[to] = next;
                    q.push(to);
                }
                else if(potential[to] != next){
                    res.nonzero_cycle[c] = true;
                }
            }
        }
    }

    return res;
}

/*
細かい注意点
0-indexed、有向グラフ。無重みはgraph=vector<vector<ll>>。
重み付きはGraph=vector<vector<pair<ll,ll>>>で、辺は{行き先,重み}。重みはllの全範囲。
空グラフ・自己ループ・多重辺に対応。入力は変更しない。構築O(N+M)、メモリO(N+M)。
SCC番号は縮約DAGのトポロジカル順。異なる成分間の辺u->vではcmp[u]<cmp[v]。
縮約DAGの順方向DPは成分番号の昇順、逆方向は降順に処理できる。
cmpは頂点から成分番号、groupsは成分番号から頂点一覧。
重み付き版はWeightedSCCResultを返し、SCCResultの全フィールド・関数も使える。
nonzero_cycle[c]はSCC c に辺重みの総和が0でない有向閉路が存在するか。
正閉路・負閉路の区別はしない。SCC内部の辺だけを使い、__int128のポテンシャルで判定。
dag/rdagは重みを持たない縮約DAG。成分間の多重辺は従来どおり除去する。

使える関数一覧
auto scc = SCCDecomposition(g);  // graphならSCCResult、GraphならWeightedSCCResult O(N+M)

scc.K;                           // 強連結成分の個数
scc.cmp[v];                      // 頂点vが属するSCC番号
scc.same(u,v);                   // u,vが同じSCCか

scc.groups[c];                   // SCC c に属する頂点一覧
scc.size(c);                     // SCC c の頂点数

scc.dag;                         // SCCを縮約したDAG
scc.rdag;                        // 縮約DAGの逆グラフ
scc.indeg[c];                    // SCC c の入次数
scc.outdeg[c];                   // SCC c の出次数

scc.sources();                   // 入次数0のSCC一覧
scc.sinks();                     // 出次数0のSCC一覧

scc.cyclic[c];                   // SCC c が閉路を含むか
scc.in_cycle(v);                 // 頂点vが有向閉路上にあるか
scc.has_cycle();                 // グラフ全体に有向閉路があるか

重み付き版で追加される情報・関数
scc.nonzero_cycle[c];            // SCC c に非0閉路があるか O(1)
scc.has_nonzero_cycle();         // 全体に非0閉路があるか O(K)

基本の関数
graph g={{1},{0,2},{}};
auto scc = SCCDecomposition(g);
auto connected = scc.same(0,1);               // true
auto component = scc.cmp[2];                  // 1
auto sources = scc.sources();                 // 入次数0のSCC一覧
Graph weighted(3);
weighted[0]={{1,2}}; weighted[1]={{0,-1},{2,5}};
auto wscc = SCCDecomposition(weighted);
auto nonzero = wscc.nonzero_cycle[wscc.cmp[0]]; // true: 0 -> 1 -> 0 の重みの和は1
auto any_nonzero = wscc.has_nonzero_cycle();   // true
*/
