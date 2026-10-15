//回文の半径を求める　O(|S|)
  string S;
  vector<ll> R(sz(S));//それぞれの文字を中心とした回文半径
  ll I = 0, J = 0;
  while (I < sz(S)) {
    while (I-J >= 0 && I+J < sz(S) && S[I-J] == S[I+J]) ++J;
      R[I] = J;
      int K = 1;
    while (I-K >= 0 && K+R[I-K] < J) R[I+K] = R[I-K], ++K;
    I += K; J -= K;
  }