//LiChaoTree

template<class T = long long, bool MINIMIZE = true>
struct LiChaoTree {
    static_assert(is_integral_v<T>);

    struct Line {
        T a, b;

        Line(T a = 0, T b = 0) : a(a), b(b) {}

        T get(T x) const {
            return a * x + b;
        }
    };

    struct Node {
        Line line;
        bool has_line;
        int left, right;

        Node()
            : line(),
              has_line(false),
              left(-1),
              right(-1) {}
    };

    T x_left, x_right;
    vector<Node> nodes;
    int root;

    LiChaoTree() : x_left(0), x_right(-1), root(-1) {}

    LiChaoTree(T x_left, T x_right)
        : x_left(x_left),
          x_right(x_right) {

        assert(x_left <= x_right);

        root = new_node();
    }

    bool better(T a, T b) const {
        if constexpr (MINIMIZE) {
            return a < b;
        } else {
            return a > b;
        }
    }

    T identity() const {
        if constexpr (MINIMIZE) {
            return numeric_limits<T>::max();
        } else {
            return numeric_limits<T>::lowest();
        }
    }

    int new_node() {
        nodes.emplace_back();
        return (int)nodes.size() - 1;
    }

    int make_left(int v) {
        if (nodes[v].left == -1) {
            int u = new_node();
            nodes[v].left = u;
        }

        return nodes[v].left;
    }

    int make_right(int v) {
        if (nodes[v].right == -1) {
            int u = new_node();
            nodes[v].right = u;
        }

        return nodes[v].right;
    }

    void add_line_impl(
        int v,
        T l,
        T r,
        Line line
    ) {
        if (!nodes[v].has_line) {
            nodes[v].line = line;
            nodes[v].has_line = true;
            return;
        }

        T m = midpoint(l, r);

        bool left_better =
            better(line.get(l), nodes[v].line.get(l));

        bool mid_better =
            better(line.get(m), nodes[v].line.get(m));

        if (mid_better) {
            swap(nodes[v].line, line);
        }

        if (l == r) return;

        if (left_better != mid_better) {
            int u = make_left(v);

            add_line_impl(
                u,
                l,
                m,
                line
            );
        } else {
            int u = make_right(v);

            add_line_impl(
                u,
                m + 1,
                r,
                line
            );
        }
    }

    void add_segment_impl(
        int v,
        T l,
        T r,
        T ql,
        T qr,
        Line line
    ) {
        if (qr < l || r < ql) return;

        if (ql <= l && r <= qr) {
            add_line_impl(v, l, r, line);
            return;
        }

        T m = midpoint(l, r);

        if (ql <= m) {
            int u = make_left(v);

            add_segment_impl(
                u,
                l,
                m,
                ql,
                qr,
                line
            );
        }

        if (m < qr) {
            int u = make_right(v);

            add_segment_impl(
                u,
                m + 1,
                r,
                ql,
                qr,
                line
            );
        }
    }

    T query_impl(
        int v,
        T l,
        T r,
        T x
    ) const {
        if (v == -1) {
            return identity();
        }

        T result = identity();

        if (nodes[v].has_line) {
            result = nodes[v].line.get(x);
        }

        if (l == r) return result;

        T m = midpoint(l, r);

        T child_result;

        if (x <= m) {
            child_result = query_impl(
                nodes[v].left,
                l,
                m,
                x
            );
        } else {
            child_result = query_impl(
                nodes[v].right,
                m + 1,
                r,
                x
            );
        }

        if (better(child_result, result)) {
            result = child_result;
        }

        return result;
    }

    void add_line(T a, T b) {
        add_line_impl(
            root,
            x_left,
            x_right,
            Line(a, b)
        );
    }

    void add_line(Line line) {
        add_line_impl(
            root,
            x_left,
            x_right,
            line
        );
    }

    void add_segment(
        T a,
        T b,
        T l,
        T r
    ) {
        if (r < x_left || x_right < l) return;

        l = max(l, x_left);
        r = min(r, x_right);

        if (l > r) return;

        add_segment_impl(
            root,
            x_left,
            x_right,
            l,
            r,
            Line(a, b)
        );
    }

    void add_segment(
        Line line,
        T l,
        T r
    ) {
        add_segment(
            line.a,
            line.b,
            l,
            r
        );
    }

    T query(T x) const {
        assert(x_left <= x && x <= x_right);

        return query_impl(
            root,
            x_left,
            x_right,
            x
        );
    }
};

/*
整数座標用、区間は両端を含む [L,R]

LiChaoTree<ll> cht(L,R);                       // L<=x<=R 上で最小値を管理
LiChaoTree<ll,false> cht(L,R);                 // L<=x<=R 上で最大値を管理

cht.add_line(a,b);                             // y=ax+b を全区間に追加 O(log(R-L+1))

cht.add_segment(a,b,l,r);                      // y=ax+b を l<=x<=r のみに追加
                                                // O(log^2(R-L+1))

cht.query(x);                                  // x における登録済み直線の最小値/最大値
                                                // O(log(R-L+1))

LiChaoTree<ll>::Line f(a,b);                   // 直線 y=ax+b
cht.add_line(f);                               // Line 型でも追加可能
cht.add_segment(f,l,r);                        // 区間への追加も可能

※ MINIMIZE=true が最小値、false が最大値
※ query(x) する x は初期化時の [L,R] 内である必要がある
※ 直線が1本も有効でない x の query() は
   最小値版なら numeric_limits<T>::max()
   最大値版なら numeric_limits<T>::lowest() を返す

※ a*x+b が T の範囲内に収まる必要がある
*/