//偏角ソート
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