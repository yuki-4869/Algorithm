struct point {
    ll x, y;
};

long long cross(const point &a, const point &b) {
    return (long long) a.x * b.y - (long long) a.y * b.x;
}

bool cmp(const point &a, const point &b) {
    if(a.x == 0 && a.y == 0 && b.x == 0 && b.y == 0) return false;

    if(a.x == 0 && a.y == 0) return true;
    if(b.x == 0 && b.y == 0) return false;
    ll ah = (a.y < 0 or (a.y == 0 and a.x < 0));
    ll bh = (b.y < 0 or (b.y == 0 and b.x < 0));
    if (ah != bh) return ah < bh;

    ll c = cross(a, b);
    if (c != 0) return c > 0;
    return a.x * a.x + a.y * a.y < b.x * b.x + b.y * b.y;
}

void argument_sort(vector<point> &points) {
    sort(points.begin(), points.end(), cmp);
}

/*
細かい注意点
原点を先頭、その後は正のx軸から反時計回り。同方向の点は原点から近い順。
外積・距離の二乗・差がlong longに収まる座標で使用する。

使える関数一覧
cross(a,b);             // ll: aとbの外積 O(1)
cmp(a,b);               // bool: 偏角順の比較 O(1)
argument_sort(points);  // void: 点列を偏角ソート O(N log N)

基本の関数
vector<point> points={{0,-1},{-1,0},{0,1},{1,0}};
argument_sort(points);                        // 右・上・左・下の順
*/
