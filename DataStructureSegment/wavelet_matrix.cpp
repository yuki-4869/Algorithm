//WaveletMatrix

template<class T = long long>
struct WaveletMatrix {
    int n;
    int sigma;
    int log;

    vector<T> values;
    vector<vector<int>> bit_sum;
    vector<int> mid;

    WaveletMatrix() : n(0), sigma(0), log(0) {}

    explicit WaveletMatrix(const vector<T>& a) {
        build(a);
    }

    void build(const vector<T>& a) {
        n = a.size();

        values = a;
        sort(values.begin(), values.end());
        values.erase(
            unique(values.begin(), values.end()),
            values.end()
        );

        sigma = values.size();

        log = 0;
        while ((1LL << log) < max(1, sigma)) {
            log++;
        }

        vector<int> cur(n);

        for (int i = 0; i < n; i++) {
            cur[i] =
                lower_bound(
                    values.begin(),
                    values.end(),
                    a[i]
                ) - values.begin();
        }

        bit_sum.assign(
            log,
            vector<int>(n + 1, 0)
        );

        mid.assign(log, 0);

        for (int level = log - 1; level >= 0; level--) {
            vector<int> zero;
            vector<int> one;

            zero.reserve(n);
            one.reserve(n);

            for (int i = 0; i < n; i++) {
                int bit = (cur[i] >> level) & 1;

                bit_sum[level][i + 1] =
                    bit_sum[level][i] + bit;

                if (bit) {
                    one.push_back(cur[i]);
                } else {
                    zero.push_back(cur[i]);
                }
            }

            mid[level] = zero.size();

            cur.clear();
            cur.reserve(n);

            cur.insert(
                cur.end(),
                zero.begin(),
                zero.end()
            );

            cur.insert(
                cur.end(),
                one.begin(),
                one.end()
            );
        }
    }

    int ones(int level, int l, int r) const {
        return
            bit_sum[level][r] -
            bit_sum[level][l];
    }

    int zeros(int level, int l, int r) const {
        return (r - l) - ones(level, l, r);
    }

    T access(int k) const {
        assert(0 <= k && k < n);

        int x = 0;

        for (int level = log - 1; level >= 0; level--) {
            int bit =
                bit_sum[level][k + 1] -
                bit_sum[level][k];

            int one_before =
                bit_sum[level][k];

            if (bit == 0) {
                k -= one_before;
            } else {
                x |= (1 << level);
                k =
                    mid[level] +
                    one_before;
            }
        }

        return values[x];
    }

    int count(const T& x, int l, int r) const {
        assert(0 <= l && l <= r && r <= n);

        int id =
            lower_bound(
                values.begin(),
                values.end(),
                x
            ) - values.begin();

        if (
            id == sigma ||
            values[id] != x
        ) {
            return 0;
        }

        for (int level = log - 1; level >= 0; level--) {
            int one_l = bit_sum[level][l];
            int one_r = bit_sum[level][r];

            if ((id >> level) & 1) {
                l = mid[level] + one_l;
                r = mid[level] + one_r;
            } else {
                l -= one_l;
                r -= one_r;
            }
        }

        return r - l;
    }

    int range_freq_code(
        int l,
        int r,
        int upper
    ) const {
        if (upper <= 0) return 0;
        if (upper >= sigma) return r - l;

        int ret = 0;

        for (int level = log - 1; level >= 0; level--) {
            int one_l = bit_sum[level][l];
            int one_r = bit_sum[level][r];

            int zero_l = l - one_l;
            int zero_r = r - one_r;

            if ((upper >> level) & 1) {
                ret += zero_r - zero_l;

                l = mid[level] + one_l;
                r = mid[level] + one_r;
            } else {
                l = zero_l;
                r = zero_r;
            }
        }

        return ret;
    }

    int range_freq(
        int l,
        int r,
        const T& upper
    ) const {
        assert(0 <= l && l <= r && r <= n);

        int id =
            lower_bound(
                values.begin(),
                values.end(),
                upper
            ) - values.begin();

        return range_freq_code(l, r, id);
    }

    int range_freq(
        int l,
        int r,
        const T& lower,
        const T& upper
    ) const {
        assert(0 <= l && l <= r && r <= n);

        return
            range_freq(l, r, upper) -
            range_freq(l, r, lower);
    }

    T kth_smallest(
        int l,
        int r,
        int k
    ) const {
        assert(0 <= l && l <= r && r <= n);
        assert(0 <= k && k < r - l);

        int x = 0;

        for (int level = log - 1; level >= 0; level--) {
            int one_l = bit_sum[level][l];
            int one_r = bit_sum[level][r];

            int zero_l = l - one_l;
            int zero_r = r - one_r;

            int zero_count = zero_r - zero_l;

            if (k < zero_count) {
                l = zero_l;
                r = zero_r;
            } else {
                k -= zero_count;

                x |= (1 << level);

                l = mid[level] + one_l;
                r = mid[level] + one_r;
            }
        }

        return values[x];
    }

    T kth_largest(
        int l,
        int r,
        int k
    ) const {
        assert(0 <= l && l <= r && r <= n);
        assert(0 <= k && k < r - l);

        return kth_smallest(
            l,
            r,
            (r - l - 1) - k
        );
    }

    optional<T> prev_value(
        int l,
        int r,
        const T& upper
    ) const {
        assert(0 <= l && l <= r && r <= n);

        int cnt = range_freq(l, r, upper);

        if (cnt == 0) {
            return nullopt;
        }

        return kth_smallest(
            l,
            r,
            cnt - 1
        );
    }

    optional<T> next_value(
        int l,
        int r,
        const T& lower
    ) const {
        assert(0 <= l && l <= r && r <= n);

        int cnt = range_freq(l, r, lower);

        if (cnt == r - l) {
            return nullopt;
        }

        return kth_smallest(
            l,
            r,
            cnt
        );
    }

    int size() const {
        return n;
    }
};

/*
0-indexed
静的配列用
内部で座標圧縮するため負数・long long も使用可能

WaveletMatrix<ll> wm(A);                       // 配列 A から構築 O(N log N)

wm.access(i);                                  // A[i] を取得 O(log N)

wm.count(x,l,r);                               // A[l,r) に値 x が何個あるか O(log N)

wm.range_freq(l,r,upper);                      // A[l,r) で x < upper の要素数 O(log N)
wm.range_freq(l,r,lower,upper);                // A[l,r) で lower <= x < upper の要素数 O(log N)

wm.kth_smallest(l,r,k);                        // A[l,r) の 0-indexed k 番目に小さい値 O(log N)
wm.kth_largest(l,r,k);                         // A[l,r) の 0-indexed k 番目に大きい値 O(log N)

wm.prev_value(l,r,upper);                      // A[l,r) で upper 未満の最大値
                                                // 存在しなければ nullopt O(log N)

wm.next_value(l,r,lower);                      // A[l,r) で lower 以上の最小値
                                                // 存在しなければ nullopt O(log N)

wm.size();                                     // 配列長 O(1)

※ 区間は全て半開区間 [l,r)
※ kth_smallest / kth_largest の k は 0-indexed
※ 静的 Wavelet Matrix なので構築後の点更新はできない
*/