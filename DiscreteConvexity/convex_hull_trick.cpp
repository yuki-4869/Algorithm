template<class T = long long, bool MINIMIZE = true>
struct ConvexHullTrick {
    static_assert(is_integral_v<T> && sizeof(T) <= 8);

    using i128 = __int128_t;

    struct Line {
        T a, b;

        Line(T a = 0, T b = 0) : a(a), b(b) {}

        i128 get(T x) const {
            return (i128)a * x + b;
        }
    };

    vector<Line> lines;

    ConvexHullTrick() {}

    Line normalize(T a, T b) const {
        if constexpr (MINIMIZE) {
            return Line(a, b);
        } else {
            return Line(-a, -b);
        }
    }

    bool unnecessary(
        const Line& x,
        const Line& y,
        const Line& z
    ) const {
        i128 left =
            ((i128)y.b - x.b) *
            ((i128)y.a - z.a);

        i128 right =
            ((i128)z.b - y.b) *
            ((i128)x.a - y.a);

        return left >= right;
    }

    void add_line(T a, T b) {
        Line line = normalize(a, b);

        if (!lines.empty()) {
            do { if (!(lines.back().a >= line.a)) std::abort(); } while (false);
        }

        if (!lines.empty() && lines.back().a == line.a) {
            if (lines.back().b <= line.b) {
                return;
            }

            lines.pop_back();
        }

        while (
            lines.size() >= 2 &&
            unnecessary(
                lines[lines.size() - 2],
                lines[lines.size() - 1],
                line
            )
        ) {
            lines.pop_back();
        }

        lines.push_back(line);
    }

    T query(T x) const {
        do { if (!(!lines.empty())) std::abort(); } while (false);

        int left = 0;
        int right = (int)lines.size() - 1;

        while (left < right) {
            int mid = (left + right) / 2;

            if (lines[mid].get(x) <= lines[mid + 1].get(x)) {
                right = mid;
            } else {
                left = mid + 1;
            }
        }

        i128 result = lines[left].get(x);

        if constexpr (!MINIMIZE) {
            result = -result;
        }

        return (T)result;
    }

    int size() const {
        return lines.size();
    }

    bool empty() const {
        return lines.empty();
    }

    void clear() {
        lines.clear();
    }
};

/*
細かい注意点
整数係数・整数x用。最小値版は傾き降順、最大値版(false)は傾き昇順で追加する。
同じ傾きは最小値版なら最小切片、最大値版なら最大切片だけを残す。
非空でqueryを呼ぶ。a*x+bはTの範囲内。内部比較は__int128。
xの照会順序は任意。

使える関数一覧
ConvexHullTrick<ll> cht;                       // 最小値を管理
ConvexHullTrick<ll,false> cht;                 // 最大値を管理

cht.add_line(a,b);                             // y=ax+b を追加 償却 O(1)
cht.query(x);                                  // x における最小値/最大値 O(log N)

cht.size();                                    // 現在保持している有効な直線数 O(1)
cht.empty();                                   // 直線が0本なら true O(1)
cht.clear();                                   // 全直線を削除 O(N)

基本の関数
ConvexHullTrick<ll> cht;
cht.add_line(2,0);
cht.add_line(-1,3);
auto value = cht.query(2);                    // 1 O(log N)
*/
