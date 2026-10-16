class FWHT {
    using ll = long long;
    using i128 = __int128_t;

    static bool pow2(int n) { return n > 0 && !(n & (n - 1)); }

    static int ceil_pow2(int n) {
        int s = 1;
        while (s < n) s <<= 1;
        return s;
    }

    static ll inv_mod(ll a, ll mod) {
        ll b = mod;
        i128 x = 1, y = 0;
        while (b) {
            ll q = a / b;
            tie(a, b) = pair{b, a % b};
            tie(x, y) = pair{y, x - (i128)q * y};
        }
        do { if (!(a == 1)) std::abort(); } while (false);
        x %= mod;
        if (x < 0) x += mod;
        return (ll)x;
    }

public:
    template<class T>
    static void transform(vector<T>& a, bool inverse = false) {
        int n = a.size();
        do { if (!(pow2(n))) std::abort(); } while (false);

        for (int len = 1; len < n; len <<= 1)
            for (int i = 0; i < n; i += len << 1)
                for (int j = 0; j < len; j++) {
                    T x = a[i + j], y = a[i + len + j];
                    a[i + j] = x + y;
                    a[i + len + j] = x - y;
                }

        if (inverse) for (auto& x : a) x /= n;
    }

    template<class T>
    static vector<T> convolution(vector<T> a, vector<T> b) {
        if (a.empty() || b.empty()) return {};

        int n = ceil_pow2(max(a.size(), b.size()));
        a.resize(n); b.resize(n);

        transform(a);
        transform(b);
        for (int i = 0; i < n; i++) a[i] *= b[i];
        transform(a, true);

        return a;
    }

    static void transform_mod(vector<ll>& a, ll mod, bool inverse = false) {
        int n = a.size();
        do { if (!(mod >= 2 && pow2(n))) std::abort(); } while (false);

        for (auto& x : a) x = (x % mod + mod) % mod;

        for (int len = 1; len < n; len <<= 1)
            for (int i = 0; i < n; i += len << 1)
                for (int j = 0; j < len; j++) {
                    ll x = a[i + j], y = a[i + len + j];
                    a[i + j] = (i128(x) + y) % mod;
                    a[i + len + j] = (i128(x) - y + mod) % mod;
                }

        if (inverse) {
            do { if (!(gcd((ll)n, mod) == 1)) std::abort(); } while (false);
            ll inv = inv_mod(n, mod);
            for (auto& x : a) x = (i128)x * inv % mod;
        }
    }

    static vector<ll> convolution_mod(vector<ll> a, vector<ll> b, ll mod) {
        if (a.empty() || b.empty()) return {};

        int n = ceil_pow2(max(a.size(), b.size()));
        do { if (!(gcd((ll)n, mod) == 1)) std::abort(); } while (false);

        a.resize(n); b.resize(n);
        transform_mod(a, mod);
        transform_mod(b, mod);

        for (int i = 0; i < n; i++) a[i] = (i128)a[i] * b[i] % mod;
        transform_mod(a, mod, true);

        return a;
    }
};

/*
細かい注意点
C[k]=Σ_{i xor j=k}A[i]B[j]。計算量O(N log N)。
convolutionはmax(|A|,|B|)以上の最小2冪へ0埋め。transform単体は非空の2冪長を渡す。
通常版はlong long/double/modintなどに対応。途中計算が型の範囲に収まること。
mod版の逆変換にはgcd(N,mod)=1が必要。積の中間計算は__int128。

使える関数一覧
auto C = FWHT::convolution(A,B);                // XOR畳み込み
                                                // ll, double, modint 等
                                                // サイズは自動で2冪に拡張

FWHT::transform(A);                             // FWHT
FWHT::transform(A,true);                        // 逆FWHT
                                                // 直接呼ぶ場合 size は2冪

auto C = FWHT::convolution_mod(A,B,mod);        // mod上でXOR畳み込み
FWHT::transform_mod(A,mod);                     // mod上のFWHT
FWHT::transform_mod(A,mod,true);                // mod上の逆FWHT

基本の関数
auto C = FWHT::convolution<ll>({1,2},{3,4});   // {11,10}
auto D = FWHT::convolution_mod({1,2},{3,4},998244353);
*/
