//削除可能なオフラインUnionFind
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
vl val(n,0)//最初の頂点の重み
OfflineDynamicConnectivity odc(n, q, val);
odc.insert(t,u,v) 時刻tに辺u-vをマージ
odc.erase(t,u,v)  時刻tに辺u-vを削除
odc.update_weight(t,u,w) 時刻tに頂点uの重さをwに変更
odc.uf.component_sum(u)　時刻tに頂点uを含む連結成分の重さの合計
odc.build()　　　 クエリを処理
odc.run([&](int t) {
        // 現在時刻 t で、調べたいクエリがあれば処理});
*/