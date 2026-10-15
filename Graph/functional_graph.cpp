//FunctionalGraph

// Functional Graph
struct FunctionalGraph{
  ll n;
  vl to, indeg, cycle_id, cycle_pos, cycle_len, dist, root_cycle, root_pos;
  vb on_cycle;
  vvl rev;

  FunctionalGraph(const vl& _to){ build(_to); }

  void build(const vl& _to){
    to=_to; n=to.size();
    indeg.assign(n,0);
    rev.assign(n,{});

    rep(v,n){
      indeg[to[v]]++;
      rev[to[v]].pb(v);
    }

    queue<ll> q;
    rep(v,n) if(indeg[v]==0) q.push(v);

    while(!q.empty()){
      ll v=q.front(); q.pop();
      if(--indeg[to[v]]==0) q.push(to[v]);
    }

    on_cycle.assign(n,false);
    cycle_id.assign(n,-1);
    cycle_pos.assign(n,-1);
    root_cycle.assign(n,-1);
    root_pos.assign(n,-1);
    dist.assign(n,-1);
    cycle_len.clear();

    ll cid=0;
    rep(s,n){
      if(indeg[s]==0 || cycle_id[s]!=-1) continue;

      ll v=s, pos=0;
      do{
        on_cycle[v]=true;
        cycle_id[v]=cid;
        cycle_pos[v]=pos;
        root_cycle[v]=v;
        root_pos[v]=pos;
        dist[v]=0;
        v=to[v];
        pos++;
      }while(v!=s);

      cycle_len.pb(pos);
      cid++;
    }

    queue<ll> qq;
    rep(v,n) if(on_cycle[v]) qq.push(v);

    while(!qq.empty()){
      ll v=qq.front(); qq.pop();

      for(ll u:rev[v]){
        if(dist[u]!=-1) continue;
        dist[u]=dist[v]+1;
        cycle_id[u]=cycle_id[v];
        root_cycle[u]=root_cycle[v];
        root_pos[u]=root_pos[v];
        qq.push(u);
      }
    }
  }

  // v がサイクル上にあるか
  bool is_cycle(ll v){
    return on_cycle[v];
  }

  // v が最終的に入るサイクルのID
  ll get_cycle_id(ll v){
    return cycle_id[v];
  }

  // v からサイクルまでの距離
  ll get_dist(ll v){
    return dist[v];
  }

  // v から進んで最初に到達するサイクル頂点
  ll get_root(ll v){
    return root_cycle[v];
  }

  // v が入るサイクルの長さ
  ll get_cycle_len(ll v){
    return cycle_len[cycle_id[v]];
  }

  // サイクルに入った位置
  ll get_cycle_pos(ll v){
    return root_pos[v];
  }
};
/*
Functional Graph
0-indexed
各頂点から出る辺がちょうど1本

vector<int> to(n);                             // v -> to[v]
FunctionalGraph fg(to);                        // 構築 O(N log 2^64)

fg.jump(v,k);                                  // vからk回進んだ頂点 O(log k)

fg.in_cycle(v);                                // vがサイクル上ならtrue
fg.cycle_id(v);                                // 最終的に入るサイクル番号
fg.cycle_size(v);                              // 最終的に入るサイクルの長さ
fg.dist_to_cycle(v);                           // サイクルまでの距離
fg.cycle_entry(v);                             // 最初に到達するサイクル頂点

fg.distance(a,b);                              // aからbへの最小移動回数
                                                // 到達不能なら-1
fg.reachable(a,b);                             // aからbへ到達可能か

fg.cycle[id];                                  // id番目のサイクル頂点列
                                                // toの向き順

例:
vector<int> to={1,2,3,4,5,2,1};
FunctionalGraph fg(to);

fg.jump(0,3);                                  // 3
fg.dist_to_cycle(0);                           // 2
fg.cycle_entry(0);                             // 2
fg.in_cycle(2);                                // true
fg.cycle_size(0);                              // 4
fg.distance(0,4);                              // 4
fg.reachable(6,3);                             // true
*/