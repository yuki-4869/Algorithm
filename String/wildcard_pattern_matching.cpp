std::vector<bool> wildcard_pattern_matching(std::string_view text, std::string_view pattern) {
    if (pattern.size() > text.size()) return {};
    if (pattern.empty()) return std::vector<bool>(text.size()+1,true);
    using Value = atcoder::modint998244353;
    constexpr std::size_t max_transform = 1u<<23;
    if (text.size() > max_transform || pattern.size() > (Value::mod()-1)/625 ||
        text.size()+pattern.size()-1 > max_transform) std::abort();

    auto encode = [](std::string_view s, bool reversed) {
        std::array<std::vector<Value>,3> result;
        for (auto& a : result) a.resize(s.size());
        for (std::size_t i = 0; i < s.size(); ++i) {
            const char c = s[i];
            if (c == '*') continue;
            if (c < 'a' || c > 'z') std::abort();
            const auto j = reversed ? s.size()-1-i : i;
            const int x = c-'a';
            result[0][j] = 1;
            result[1][j] = x;
            result[2][j] = x*x;
        }
        return result;
    };
    auto a = encode(text,false), b = encode(pattern,true);
    auto aa = atcoder::convolution(std::move(a[2]),std::move(b[0]));
    auto ab = atcoder::convolution(std::move(a[1]),std::move(b[1]));
    auto bb = atcoder::convolution(std::move(a[0]),std::move(b[2]));
    std::vector<bool> result(text.size()-pattern.size()+1);
    for (std::size_t i = 0; i < result.size(); ++i) {
        const auto at = i+pattern.size()-1;
        result[i] = aa[at]-2*ab[at]+bb[at] == Value(0);
    }
    return result;
}

/*
細かい注意点
英小文字と*。両側にある*が任意の1文字に一致する。返り値添字iは開始位置。
パターンが空なら全境界が一致、テキストより長いなら空配列。
0<|T|<=|S|の場合、|S|+|T|-1<=2^23、625|T|<998244353。codecfではatcoder/convolutionを追加。

使える関数一覧
wildcard_pattern_matching(text,pattern); // vector<bool>: 開始位置ごとの一致判定
                                         // 時間O((N+M)log(N+M+2))、領域O(N+M)

基本の関数
auto match=wildcard_pattern_matching("a*ca","a*");
if(match!=std::vector<bool>{true,true,false})std::abort();
*/
