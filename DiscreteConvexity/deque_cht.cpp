//DequeCHT

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
            assert(last_slope >= line.a);
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
        assert(!lines.empty());

        if (has_last_x) {
            assert(last_x <= x);
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
整数係数・整数 x 用
x のクエリ順も単調である場合の高速 CHT

DequeCHT<ll> cht;                              // 最小値を管理
DequeCHT<ll,false> cht;                        // 最大値を管理

cht.add_line(a,b);                             // y=ax+b を追加 償却 O(1)
cht.query(x);                                  // x における最小値/最大値 償却 O(1)

cht.size();                                    // 現在 hull に残っている直線数 O(1)
cht.empty();                                   // 直線が0本なら true O(1)
cht.clear();                                   // 全直線を削除 O(N)

最小値版:
    傾き a を単調減少で追加する必要がある
    a1 >= a2 >= a3 >= ...

最大値版:
    傾き a を単調増加で追加する必要がある
    a1 <= a2 <= a3 <= ...

query(x):
    x を単調増加で与える必要がある
    x1 <= x2 <= x3 <= ...

同じ傾きの直線を追加した場合、
最小値版では切片が小さい方、
最大値版では切片が大きい方だけを残す

※ add_line と query は交互に呼び出してもよい
※ 各直線は deque から高々1回削除されるので全体で線形時間
※ a*x+b の最終結果が T の範囲内に収まる必要がある
※ 交点比較・a*x+b の内部計算には __int128 を使用
*/