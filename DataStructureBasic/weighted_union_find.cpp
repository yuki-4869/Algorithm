//重み付きUnionFind

template<class T = long long>
struct WeightedUnionFind {
    int n;
    vector<int> parent_or_size;
    vector<T> diff_weight;

    WeightedUnionFind() : n(0) {}

    explicit WeightedUnionFind(int n)
        : n(n), parent_or_size(n, -1), diff_weight(n, T{}) {}

    int leader(int a) {
        assert(0 <= a && a < n);

        if (parent_or_size[a] < 0) return a;

        int p = parent_or_size[a];
        int r = leader(p);

        diff_weight[a] += diff_weight[p];

        return parent_or_size[a] = r;
    }

    T weight(int a) {
        assert(0 <= a && a < n);

        leader(a);

        return diff_weight[a];
    }

    int merge(int a, int b, T w) {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);

        w += weight(a);
        w -= weight(b);

        int x = leader(a);
        int y = leader(b);

        if (x == y) return x;

        if (-parent_or_size[x] < -parent_or_size[y]) {
            swap(x, y);
            w = -w;
        }

        parent_or_size[x] += parent_or_size[y];
        parent_or_size[y] = x;
        diff_weight[y] = w;

        return x;
    }

    bool same(int a, int b) {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);

        return leader(a) == leader(b);
    }

    int size(int a) {
        assert(0 <= a && a < n);

        return -parent_or_size[leader(a)];
    }

    T diff(int a, int b) {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);
        assert(same(a, b));

        return weight(b) - weight(a);
    }

    bool consistent(int a, int b, T w) {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);

        if (!same(a, b)) return true;

        return diff(a, b) == w;
    }

    vector<vector<int>> groups() {
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
WeightedUnionFind<ll> uf(N);                     // N 頂点で初期化 O(N)

uf.merge(a,b,w);                                // potential[b]-potential[a]=w として併合 償却 O(alpha(N))
uf.same(a,b);                                   // a,b が同じ連結成分なら true 償却 O(alpha(N))
uf.leader(a);                                   // a の代表元を返す 償却 O(alpha(N))
uf.size(a);                                     // a の属する連結成分のサイズ 償却 O(alpha(N))
uf.groups();                                    // 全連結成分を列挙 O(N alpha(N))

uf.weight(a);                                   // potential[a]-potential[leader(a)] 償却 O(alpha(N))
uf.diff(a,b);                                   // potential[b]-potential[a]、same(a,b) が必要 償却 O(alpha(N))
uf.consistent(a,b,w);                           // potential[b]-potential[a]=w と矛盾しないか判定 償却 O(alpha(N))
*/

// snippet direct test