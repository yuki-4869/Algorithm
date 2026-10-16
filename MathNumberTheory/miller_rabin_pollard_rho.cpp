struct PollardRho {
    using u64 = uint64_t;
    using u128 = __uint128_t;

    static u64 mod_mul(u64 a, u64 b, u64 mod) {
        return (u128)a * b % mod;
    }

    static u64 mod_pow(u64 a, u64 n, u64 mod) {
        u64 ret = 1;

        while (n) {
            if (n & 1) {
                ret = mod_mul(ret, a, mod);
            }

            a = mod_mul(a, a, mod);
            n >>= 1;
        }

        return ret;
    }

    static bool is_prime(u64 n) {
        if (n < 2) return false;

        for (u64 p : {
            2ULL, 3ULL, 5ULL, 7ULL,
            11ULL, 13ULL, 17ULL, 19ULL,
            23ULL, 29ULL, 31ULL, 37ULL
        }) {
            if (n == p) return true;
            if (n % p == 0) return false;
        }

        u64 d = n - 1;
        int s = 0;

        while ((d & 1) == 0) {
            d >>= 1;
            s++;
        }

        for (u64 a : {
            2ULL,
            325ULL,
            9375ULL,
            28178ULL,
            450775ULL,
            9780504ULL,
            1795265022ULL
        }) {
            if (a % n == 0) continue;

            u64 x = mod_pow(a % n, d, n);

            if (x == 1 || x == n - 1) {
                continue;
            }

            bool composite = true;

            for (int r = 1; r < s; r++) {
                x = mod_mul(x, x, n);

                if (x == n - 1) {
                    composite = false;
                    break;
                }
            }

            if (composite) {
                return false;
            }
        }

        return true;
    }

    static u64 rho(u64 n) {
        if (n % 2 == 0) return 2;
        if (n % 3 == 0) return 3;

        static mt19937_64 rng(
            chrono::steady_clock::now()
                .time_since_epoch()
                .count()
        );

        while (true) {
            u64 c =
                uniform_int_distribution<u64>(1, n - 1)(rng);

            u64 x =
                uniform_int_distribution<u64>(0, n - 1)(rng);

            u64 y = x;
            u64 d = 1;

            auto f = [&](u64 x) -> u64 {
                return (
                    mod_mul(x, x, n) + c
                ) % n;
            };

            while (d == 1) {
                x = f(x);
                y = f(f(y));

                u64 diff =
                    x > y ? x - y : y - x;

                d = gcd(diff, n);
            }

            if (d != n) {
                return d;
            }
        }
    }

    static void factorize_impl(
        u64 n,
        vector<u64>& result
    ) {
        if (n == 1) return;

        if (is_prime(n)) {
            result.push_back(n);
            return;
        }

        u64 d = rho(n);

        factorize_impl(d, result);
        factorize_impl(n / d, result);
    }

    static vector<u64> factorize(u64 n) {
        do { if (!(n >= 1)) std::abort(); } while (false);

        vector<u64> result;

        factorize_impl(n, result);

        sort(result.begin(), result.end());

        return result;
    }

    static vector<pair<u64, int>> factorize_count(u64 n) {
        vector<u64> factors = factorize(n);

        vector<pair<u64, int>> result;

        for (u64 p : factors) {
            if (
                result.empty() ||
                result.back().first != p
            ) {
                result.push_back({p, 1});
            } else {
                result.back().second++;
            }
        }

        return result;
    }
};

/*
細かい注意点
uint64_tの非負整数用。factorizeの入力はn>=1、n=1では空配列。
is_primeは使用基底によりuint64_t全域で決定的。因数探索のPollard Rhoは乱択。
素因数は重複込みの昇順、factorize_countは素因数と指数の昇順。
mod_mulは__uint128_tを使用。mod_mul/mod_powの法は正。

使える関数一覧
PollardRho::is_prime(n);        // bool: 素数判定、固定個数の基底でO(log n)
PollardRho::factorize(n);       // vector<uint64_t>: 素因数分解、乱択
PollardRho::factorize_count(n); // vector<pair<uint64_t,int>>: 素因数と指数
PollardRho::mod_pow(a,b,mod);   // uint64_t: a^b mod mod O(log b)
PollardRho::mod_mul(a,b,mod);   // uint64_t: a*b mod mod O(1)

基本の関数
auto prime = PollardRho::is_prime(1000000007); // true
auto factors = PollardRho::factorize(60);     // {2,2,3,5}
auto counted = PollardRho::factorize_count(60);// {{2,2},{3,1},{5,1}}
*/
