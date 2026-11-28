namespace SternBrocot {
using Fraction = std::pair<long long,long long>;
using Path = std::vector<std::pair<char,long long>>;

Path encode(Fraction fraction) {
    auto [num,den] = fraction;
    if (num <= 0 || den <= 0) std::abort();
    Path path;
    while (num != den) {
        if (num < den) {
            const long long count = (den-1)/num;
            path.emplace_back('L',count);
            den -= count*num;
        } else {
            const long long count = (num-1)/den;
            path.emplace_back('R',count);
            num -= count*den;
        }
    }
    return path;
}

namespace detail {
long long narrow(__int128 value) {
    if (value > LLONG_MAX) std::abort();
    return static_cast<long long>(value);
}

std::pair<Fraction,Fraction> boundaries(const Path& path) {
    Fraction lower{0,1}, upper{1,0};
    for (auto [dir,count] : path) {
        if (count <= 0 || (dir != 'L' && dir != 'R')) std::abort();
        auto& bound = dir == 'L' ? upper : lower;
        const auto& other = dir == 'L' ? lower : upper;
        bound = {
            narrow(bound.first+static_cast<__int128>(count)*other.first),
            narrow(bound.second+static_cast<__int128>(count)*other.second)
        };
    }
    return {lower,upper};
}
}

Fraction decode(const Path& path) {
    const auto [lo,hi] = detail::boundaries(path);
    return {detail::narrow(static_cast<__int128>(lo.first)+hi.first),
            detail::narrow(static_cast<__int128>(lo.second)+hi.second)};
}

Fraction lca(Fraction a, Fraction b) {
    const Path first = encode(a), second = encode(b);
    Path prefix;
    for (std::size_t i = 0; i < std::min(first.size(),second.size()); ++i) {
        if (first[i].first != second[i].first) break;
        prefix.emplace_back(first[i].first,std::min(first[i].second,second[i].second));
        if (first[i].second != second[i].second) break;
    }
    return decode(prefix);
}

std::optional<Fraction> ancestor(Fraction fraction, long long depth) {
    if (depth < 0) std::abort();
    Path path = encode(fraction), prefix;
    for (auto [dir,count] : path) {
        if (depth == 0) break;
        const long long used = std::min(depth,count);
        prefix.emplace_back(dir,used);
        depth -= used;
    }
    if (depth != 0) return std::nullopt;
    return decode(prefix);
}

std::pair<Fraction,Fraction> range(Fraction fraction) {
    return detail::boundaries(encode(fraction));
}
}

/*
細かい注意点
分数は{分子,分母}。入力は両方正のlong longで、約分前でも可。復元結果は既約分数。
根は1/1で深さ0。経路は{方向,連続回数}の列。方向はL・R、連続回数は正。
decodeの復元結果がlong longに収まること。rangeは開区間の両端で、0は{0,1}、+∞は{1,0}。
Kは連長圧縮後の経路の長さ。encodeで得るKは分子・分母の最大値の対数以下のオーダー。

使える関数一覧
SternBrocot::encode(f);        // Path: 根からの圧縮経路 O(K)
SternBrocot::decode(path);     // Fraction: 経路の終点 O(K)
SternBrocot::lca(f,g);        // Fraction: 最小共通祖先 O(K_f+K_g)
SternBrocot::ancestor(f,d);   // optional<Fraction>: 深さdの祖先。深すぎればnullopt O(K)
SternBrocot::range(f);        // pair<Fraction,Fraction>: 部分木の開区間 O(K)

基本の関数
SternBrocot::Fraction f{5,3};
auto path=SternBrocot::encode(f);
if(SternBrocot::decode(path)!=f)std::abort();
if(SternBrocot::lca(f,{3,2})!=SternBrocot::Fraction{3,2})std::abort();
*/
