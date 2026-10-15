//RollbackUnionFind
struct RollbackUnionFind {
    struct History {
        int x;
        int parent_x;
        int y;
        int parent_y;
    };

    int n;
    int component_count;

    vector<int> parent_or_size;
    vector<History> history;

    RollbackUnionFind() : n(0), component_count(0) {}

    explicit RollbackUnionFind(int n)
        : n(n),
          component_count(n),
          parent_or_size(n, -1) {}

    int leader(int a) const {
        assert(0 <= a && a < n);

        while (parent_or_size[a] >= 0) {
            a = parent_or_size[a];
        }

        return a;
    }

    bool merge(int a, int b) {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);

        int x = leader(a);
        int y = leader(b);

        if (x == y) {
            history.push_back({
                -1, 0,
                -1, 0
            });

            return false;
        }

        if (-parent_or_size[x] < -parent_or_size[y]) {
            swap(x, y);
        }

        history.push_back({
            x, parent_or_size[x],
            y, parent_or_size[y]
        });

        parent_or_size[x] += parent_or_size[y];
        parent_or_size[y] = x;

        component_count--;

        return true;
    }

    bool same(int a, int b) const {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);

        return leader(a) == leader(b);
    }

    int size(int a) const {
        assert(0 <= a && a < n);

        return -parent_or_size[leader(a)];
    }

    int components() const {
        return component_count;
    }

    int time() const {
        return history.size();
    }

    void undo() {
        assert(!history.empty());

        auto h = history.back();
        history.pop_back();

        if (h.x == -1) return;

        parent_or_size[h.x] = h.parent_x;
        parent_or_size[h.y] = h.parent_y;

        component_count++;
    }

    void rollback(int t) {
        assert(0 <= t && t <= (int)history.size());

        while ((int)history.size() > t) {
            undo();
        }
    }

    void rollback_all() {
        rollback(0);
    }

    vector<vector<int>> groups() const {
        vector<int> leader_buf(n), group_size(n);

        for (int i = 0; i < n; i++) {
            leader_buf[i] = leader(i);
            group_size[leader_buf[i]]++;
        }

        vector<vector<int>> result(n);

        for (int i = 0; i < n; i++) {
            result[i].reserve(group_size[i]);
        }

        for (int i = 0; i < n; i++) {
            result[leader_buf[i]].push_back(i);
        }

        result.erase(
            remove_if(
                result.begin(),
                result.end(),
                [](const vector<int>& v) {
                    return v.empty();
                }
            ),
            result.end()
        );

        return result;
    }
};

/*
0-indexed

RollbackUnionFind uf(N);                       // N 頂点で初期化 O(N)

uf.merge(a,b);                                 // a,b を併合、実際に併合したなら true O(log N)
uf.same(a,b);                                  // a,b が同じ連結成分なら true O(log N)
uf.leader(a);                                  // a の代表元を返す O(log N)
uf.size(a);                                    // a の属する連結成分のサイズ O(log N)
uf.components();                               // 現在の連結成分数 O(1)
uf.groups();                                   // 全連結成分を列挙 O(N log N)

uf.undo();                                     // 直前の merge() 1回分を取り消す O(1)

int t = uf.time();                             // 現在の状態番号を保存 O(1)
uf.rollback(t);                                // time()==t になるまで巻き戻す O(現在との差)
uf.rollback_all();                             // 初期状態まで巻き戻す O(merge回数)

※ 経路圧縮は行わない
※ merge(a,b) が既に同じ成分だった場合も履歴を1つ積む
   そのため undo() は「直前の merge() 呼び出し」を正確に1回取り消す
*/