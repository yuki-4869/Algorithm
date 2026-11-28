struct FurthestPairResult {
    int first = -1, second = -1;
    __int128 distance_squared = 0;
};

FurthestPairResult furthest_pair(const std::vector<pll>& points) {
    if (points.size() > static_cast<std::size_t>(INT_MAX)) std::abort();
    if (points.size() < 2) return {};
    const auto hull = convex_hull(points);
    const int n = static_cast<int>(hull.size());
    if (n == 1) return {0,1,0};
    auto side = [&](int i, int j, int k) -> __int128 {
        auto [x,y] = hull[i];
        auto [a,b] = hull[j];
        auto [c,d] = hull[k];
        return (static_cast<__int128>(a)-x)*(static_cast<__int128>(d)-y)
             - (static_cast<__int128>(b)-y)*(static_cast<__int128>(c)-x);
    };
    __int128 best = -1;
    int from = 0, to = 1;
    auto consider = [&](int i, int j) {
        const __int128 dx = static_cast<__int128>(hull[i].first)-hull[j].first;
        const __int128 dy = static_cast<__int128>(hull[i].second)-hull[j].second;
        const __int128 distance = dx*dx+dy*dy;
        if (distance > best) {
            best = distance;
            from = i;
            to = j;
        }
    };
    int far = 1;
    for (int i = 0; i < n; ++i) {
        const int j = (i+1)%n;
        while (side(i,j,(far+1)%n) > side(i,j,far)) far = (far+1)%n;
        consider(i,far);
        consider(j,far);
        const int next = (far+1)%n;
        if (side(i,j,next) == side(i,j,far)) {
            consider(i,next);
            consider(j,next);
        }
    }
    return {
        static_cast<int>(std::find(points.begin(),points.end(),hull[from])-points.begin()),
        static_cast<int>(std::find(points.begin(),points.end(),hull[to])-points.begin()),
        best
    };
}

/*
細かい注意点
整数点。座標の各成分は絶対値10^18以下。元の入力順の0始まり添字を返す。
重複座標にも対応。0点・1点では添字は両方-1、距離二乗0。
先に凸包ライブラリ convex_hull.cpp を貼る。

使える関数一覧
furthest_pair(points);    // FurthestPairResult: 最遠の2点。時間O(N log(N+2))、領域O(N)
result.first;             // int: 元の1点目の添字
result.second;            // int: 元の2点目の添字
result.distance_squared;  // __int128: ユークリッド距離の二乗

基本の関数
auto result=furthest_pair({{0,0},{1,0},{4,3},{0,0}});
if(result.distance_squared!=25 || result.first==result.second)std::abort();
*/
