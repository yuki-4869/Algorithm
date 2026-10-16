struct StirlingSecond {
    using dmint = atcoder::dynamic_modint<-1>;
    int max_n;
    vector<vector<dmint>> S;

    StirlingSecond() : max_n(0) {}
    StirlingSecond(int n, ll MOD) { init(n, MOD); }

    void init(int n, ll MOD) {
        dmint::set_mod(MOD);
        max_n = n;
        S.assign(max_n + 1, vector<dmint>(max_n + 1, 0));
        S[0][0] = 1;

        rep1(i, max_n) {
            rep1(j, i) {
                S[i][j] = S[i - 1][j - 1] + dmint(j) * S[i - 1][j];
            }
        }
    }

    inline ll get(int n, int k) const {
        if (n < 0 || k < 0 || k > n) return 0;
        return S[n][k].val();
    }
};

/*
細かい注意点
第二種Stirling数S(i,j)。区別できるi個を、空でない区別しないj組へ分ける方法数。
Nまで前計算し、法Pで返す。N>=0、1<=P<=INT_MAX、照会のiは0<=i<=N。
dynamic_modint<-1>の法を共有するため、異なる法で作った表を同時に使わない。
構築時間・メモリO(N^2)。

使える関数一覧
StirlingSecond st(N,P); // Nまで法Pで前計算 O(N^2)
st.init(N,P);          // void: 同じ条件で再構築 O(N^2)
st.get(i,j);           // ll: S(i,j) mod P O(1)

基本の関数
StirlingSecond st(5,998244353);
auto value = st.get(3,2);                     // 3
auto empty = st.get(0,0);                     // 1
*/
