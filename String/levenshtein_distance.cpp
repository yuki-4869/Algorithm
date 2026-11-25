template <class SeqA, class SeqB>
auto levenshtein_distance(const SeqA& a, const SeqB& b)
    -> decltype(a.size(), b.size(), std::size_t{}) {
    if (a.size() < b.size()) return ::levenshtein_distance(b, a);

    const std::size_t n = a.size();
    const std::size_t m = b.size();
    if (m == 0) return n;

    std::vector<std::size_t> dp(m + 1);
    for (std::size_t j = 0; j <= m; ++j) dp[j] = j;

    for (std::size_t i = 0; i < n; ++i) {
        std::size_t diagonal = dp[0];
        dp[0] = i + 1;
        for (std::size_t j = 1; j <= m; ++j) {
            const std::size_t above = dp[j];
            const std::size_t substitution = diagonal + (a[i] == b[j - 1] ? 0 : 1);
            dp[j] = std::min({above + 1, dp[j - 1] + 1, substitution});
            diagonal = above;
        }
    }
    return dp[m];
}

inline std::size_t levenshtein_distance(std::string_view a, std::string_view b) {
    return levenshtein_distance<>(a, b);
}

/*
細かい注意点
・挿入・削除・置換のコストは各1。空の列も渡せる。
・文字列は char 単位、数列は要素単位で比較する。
・時間 O(NM)、追加領域 O(min(N,M))。どちらかが空なら O(1) 時間。

使える関数一覧
levenshtein_distance(a, b) : a から b への編集距離を std::size_t で返す。

基本の関数
assert(levenshtein_distance("kitten", "sitting") == 3);
vl a = {1, 2, 3}, b = {1, 4, 3, 5};
assert(levenshtein_distance(a, b) == 2);
*/
