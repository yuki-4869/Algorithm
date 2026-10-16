template<class T = long long, bool MINIMIZE = true>
struct DequeCHT {
    static_assert(is_integral_v<T> && sizeof(T) <= 8);

    using i128 = __int128_t;

    struct Line {
        i128 a, b;

        Line(i128 a = 0, i128 b = 0) : a(a), b(b) {}

        i128 get(T x) const {
            return a * (i128)x + b;
        }
    };

    deque<Line> lines;

    bool has_last_slope = false;
    i128 last_slope = 0;

    bool has_last_x = false;
    T last_x{};

    DequeCHT() {}

    Line normalize(T a, T b) const {
        if constexpr (MINIMIZE) {
            return Line((i128)a, (i128)b);
        } else {
            return Line(-(i128)a, -(i128)b);
        }
    }

    bool unnecessary(
        const Line& x,
        const Line& y,
        const Line& z
    ) const {
        i128 left =
            (y.b - x.b) *
            (y.a - z.a);

        i128 right =
            (z.b - y.b) *
            (x.a - y.a);

        return left >= right;
    }

    void add_line(T a, T b) {
        Line line = normalize(a, b);

        if (has_last_slope) {
            do { if (!(last_slope >= line.a)) std::abort(); } while (false);
        }

        last_slope = line.a;
        has_last_slope = true;

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

    T query(T x) {
        do { if (!(!lines.empty())) std::abort(); } while (false);

        if (has_last_x) {
            do { if (!(last_x <= x)) std::abort(); } while (false);
        }

        last_x = x;
        has_last_x = true;

        while (
            lines.size() >= 2 &&
            lines[1].get(x) <= lines[0].get(x)
        ) {
            lines.pop_front();
        }

        i128 result = lines.front().get(x);

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

        has_last_slope = false;
        last_slope = 0;

        has_last_x = false;
        last_x = T{};
    }
};

/*
細かい注意点
整数係数・整数x用。最小値版は傾き降順、最大値版(false)は傾き昇順で追加する。
同じ傾きは最小値版なら最小切片、最大値版なら最大切片だけを残す。
非空でqueryを呼ぶ。a*x+bはTの範囲内。内部比較は__int128。
xも単調増加で与える。add/queryは交互に呼んでよい。

使える関数一覧
DequeCHT<ll> cht;                              // 最小値を管理
DequeCHT<ll,false> cht;                        // 最大値を管理

cht.add_line(a,b);                             // y=ax+b を追加 償却 O(1)
cht.query(x);                                  // x における最小値/最大値 償却 O(1)

cht.size();                                    // 現在 hull に残っている直線数 O(1)
cht.empty();                                   // 直線が0本なら true O(1)
cht.clear();                                   // 全直線を削除 O(N)

基本の関数
DequeCHT<ll> cht;
cht.add_line(2,0);
cht.add_line(-1,3);
auto value = cht.query(2);                    // 1 償却O(1)
*/
