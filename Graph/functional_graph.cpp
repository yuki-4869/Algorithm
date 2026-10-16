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

  bool is_cycle(ll v){
    return on_cycle[v];
  }

  ll get_cycle_id(ll v){
    return cycle_id[v];
  }

  ll get_dist(ll v){
    return dist[v];
  }

  ll get_root(ll v){
    return root_cycle[v];
  }

  ll get_cycle_len(ll v){
    return cycle_len[cycle_id[v]];
  }

  ll get_cycle_pos(ll v){
    return root_pos[v];
  }
};

/*
細かい注意点
0-indexed。各頂点から出る辺はちょうど1本。入力はvector<ll>、各to[v]は0..N-1。
サイクルとそこへの流入木をO(N)で分解する。移動回数を指定するjump機能は含まない。
get_cycle_pos(v)は最初に到達するサイクル頂点の、サイクル内位置を返す。

使える関数一覧
FunctionalGraph fg(to); // 構築 O(N)
fg.build(to);          // void: 再構築 O(N)
fg.is_cycle(v);        // bool: サイクル上か O(1)
fg.get_cycle_id(v);    // ll: 最終到達サイクルの番号 O(1)
fg.get_dist(v);        // ll: サイクルまでの距離 O(1)
fg.get_root(v);        // ll: 最初に到達するサイクル頂点 O(1)
fg.get_cycle_len(v);   // ll: 最終到達サイクルの長さ O(1)
fg.get_cycle_pos(v);   // ll: 流入先のサイクル内位置 O(1)
fg.to, fg.rev;         // 移動先と逆向き隣接リスト
fg.cycle_id, fg.cycle_pos, fg.cycle_len, fg.dist, fg.root_cycle, fg.root_pos, fg.on_cycle; // 分解結果

基本の関数
FunctionalGraph fg(vl{1,2,3,2});
auto distance = fg.get_dist(0);               // サイクルまで2辺
auto entry = fg.get_root(0);                  // 最初のサイクル頂点2
auto length = fg.get_cycle_len(0);            // サイクル長2
auto on_cycle = fg.is_cycle(2);               // true
*/
