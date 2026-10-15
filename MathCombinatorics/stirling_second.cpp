//第二種スターリング数

struct StirlingSecond {
    using dmint = atcoder::dynamic_modint<-1>; // 実行時指定の動的mint
    int max_n;
    vector<vector<dmint>> S;

    StirlingSecond() : max_n(0) {}
    StirlingSecond(int n, ll MOD) { init(n, MOD); }

    // n: 最大サイズ, MOD: 割り切る素数 P
    void init(int n, ll MOD) {
        dmint::set_mod(MOD); // ACLの動的モジュロを設定
        max_n = n;
        S.assign(max_n + 1, vector<dmint>(max_n + 1, 0));
        S[0][0] = 1;
        
        rep1(i, max_n) {
            rep1(j, i) {
                // 漸化式: S(i, j) = S(i-1, j-1) + j * S(i-1, j)
                S[i][j] = S[i - 1][j - 1] + dmint(j) * S[i - 1][j];
            }
        }
    }

    // 値を取得する
    inline ll get(int n, int k) const {
        if (n < 0 || k < 0 || k > n) return 0;
        return S[n][k].val();
    }
};

/*
StirlingSecond st(N, P);//最大サイズN、MOD P
st.get(i,j); i個のものからj個のグループに分ける方法(mod P)
*/