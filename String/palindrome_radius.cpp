string S;
  vector<ll> R(sz(S));
  ll I = 0, J = 0;
  while (I < sz(S)) {
    while (I-J >= 0 && I+J < sz(S) && S[I-J] == S[I+J]) ++J;
      R[I] = J;
      int K = 1;
    while (I-K >= 0 && K+R[I-K] < J) R[I+K] = R[I-K], ++K;
    I += K; J -= K;
  }

/*
細かい注意点
関数内に貼る雛形。先頭のSへ対象文字列を設定してから実行する。
R[i]は中心の文字を含む奇数回文半径で、区間は[i-R[i]+1,i+R[i])。
奇数長の回文のみを扱う。偶数長は区切り文字を挟むなどして別途対応する。
時間O(|S|)、追加メモリO(|S|)。

使える関数一覧
S;    // string: 対象文字列（ループを実行する前に設定）
R[i]; // ll: i中心の奇数回文半径（ループ実行後）

基本の関数
string S = "abacaba";                        // 上の先頭宣言をこの行に置換して実行
auto radius = R[3];                          // 実行後は4
auto length = 2*R[3]-1;                      // 回文長7
*/
