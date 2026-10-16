struct UnionFindUndo {
    vector<int> data;
    vector<long long> ws, sum;
    stack<pair<int, int>> history;
    stack<pair<int, long long>> history_sum, history_ws;
    UnionFindUndo(int sz, const vector<long long>& initial_ws) {
        data.assign(sz, -1); ws = initial_ws; sum = initial_ws;
    }
    int find(int k) { return data[k] < 0 ? k : find(data[k]); }
    bool unite(int x, int y) {
        x = find(x), y = find(y);
        if(x == y) {
            history.emplace(x, data[x]); history.emplace(y, data[y]);
            history_sum.emplace(x, sum[x]); return false;
        }
        if(data[x] > data[y]) swap(x, y);
        history.emplace(y, data[y]); history.emplace(x, data[x]);
        history_sum.emplace(x, sum[x]);
        data[x] += data[y]; data[y] = x; sum[x] += sum[y];
        return true;
    }
    void undo() {
        sum[history_sum.top().first] = history_sum.top().second; history_sum.pop();
        data[history.top().first] = history.top().second; history.pop();
        data[history.top().first] = history.top().second; history.pop();
    }
    void change_weight(int k, long long x) {
        int root = find(k);
        history_ws.emplace(k, ws[k]); history_sum.emplace(root, sum[root]);
        sum[root] += (x - ws[k]); ws[k] = x;
    }
    void undo_change_weight() {
        sum[history_sum.top().first] = history_sum.top().second; history_sum.pop();
        ws[history_ws.top().first] = history_ws.top().second; history_ws.pop();
    }
    long long component_sum(int k) { return sum[find(k)]; }
};
struct OfflineDynamicConnectivity {
    using edge = pair<int, int>;
    UnionFindUndo uf;
    int V, Q, segsz;
    vector<vector<edge>> seg;
    vector<vector<pair<int, long long>>> seg_ws;
    vector<pair<pair<int, int>, edge>> pend;
    map<edge, int> cnt, appear;
    vector<long long> cur_ws;
    vector<int> ws_appear;
    OfflineDynamicConnectivity(int V, int Q, const vector<long long>& initial_ws)
        : uf(V, initial_ws), V(V), Q(Q) {
        segsz = 1; while(segsz < Q) segsz <<= 1;
        seg.resize(2 * segsz - 1); seg_ws.resize(2 * segsz - 1);
        cur_ws = initial_ws; ws_appear.assign(V, 0);
    }
    void insert(int idx, int s, int t) {
        auto e = minmax(s, t); if(cnt[e]++ == 0) appear[e] = idx;
    }
    void erase(int idx, int s, int t) {
        auto e = minmax(s, t); if(--cnt[e] == 0) pend.emplace_back(make_pair(appear[e], idx), e);
    }
    void add_ws(int a, int b, int v, long long w, int k, int l, int r) {
        if(r <= a || b <= l) return;
        if(a <= l && r <= b) { seg_ws[k].emplace_back(v, w); return; }
        add_ws(a, b, v, w, 2 * k + 1, l, (l + r) >> 1);
        add_ws(a, b, v, w, 2 * k + 2, (l + r) >> 1, r);
    }
    void update_weight(int idx, int v, long long w) {
        if(ws_appear[v] < idx) add_ws(ws_appear[v], idx, v, cur_ws[v], 0, 0, segsz);
        cur_ws[v] = w; ws_appear[v] = idx;
    }
    void add(int a, int b, const edge &e, int k, int l, int r) {
        if(r <= a || b <= l) return;
        if(a <= l && r <= b) { seg[k].emplace_back(e); return; }
        add(a, b, e, 2 * k + 1, l, (l + r) >> 1);
        add(a, b, e, 2 * k + 2, (l + r) >> 1, r);
    }
    void build() {
        for(auto &p : cnt) if(p.second > 0) pend.emplace_back(make_pair(appear[p.first], Q), p.first);
        for(auto &s : pend) add(s.first.first, s.first.second, s.second, 0, 0, segsz);
        for(int v = 0; v < V; v++) if(ws_appear[v] < Q) add_ws(ws_appear[v], Q, v, cur_ws[v], 0, 0, segsz);
    }
    void run(const function<void(int)> &f, int k = 0, int l = 0, int r = -1) {
        if(r == -1) r = segsz;
        for(auto& p : seg_ws[k]) uf.change_weight(p.first, p.second);
        for(auto &e : seg[k]) uf.unite(e.first, e.second);
        if(r - l == 1) { if(l < Q) f(l); }
        else { int mid = (l + r) >> 1; run(f, 2 * k + 1, l, mid); run(f, 2 * k + 2, mid, r); }
        for(auto &e : seg[k]) uf.undo();
        for(int i = 0; i < (int)seg_ws[k].size(); i++) uf.undo_change_weight();
    }
};

/*
細かい注意点
0-indexed、無向グラフ。時刻は0..Q-1、初期重み配列はN要素。
辺の追加・削除と重み代入を登録し、build後にrunで各時刻の回答を取る。
同じ辺の重複登録は避ける。成分和と重みの差分がlong longに収まること。
連結判定のみのOfflineDynamicConnectivityと同名クラスなので同時に貼らない。

使える関数一覧
OfflineDynamicConnectivity dc(N,Q,initial_weights); // 初期化
dc.insert(t,u,v);                  // void: 辺の追加を登録
dc.erase(t,u,v);                   // void: 辺の削除を登録
dc.update_weight(t,u,x);           // void: 頂点重みの代入を登録
dc.build();                       // void: 有効期間を時間セグ木へ登録
dc.run(callback);                 // void: 各時刻tでcallback(t)を呼ぶ
dc.uf.find(u);                    // int: 現在時刻の代表元 O(log N)
dc.uf.component_sum(u);           // ll: 現在時刻の連結成分和 O(log N)

基本の関数
OfflineDynamicConnectivity dc(3,3,vl{1,2,4});
dc.insert(0,0,1);
dc.update_weight(1,1,10);
dc.erase(2,0,1);
dc.build();
vl answers(3);
dc.run([&](int t){answers[t]=dc.uf.component_sum(0);}); // {3,11,1}
*/
