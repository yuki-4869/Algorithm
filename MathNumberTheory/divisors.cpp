vector<ll> get_divisors(ll n) {
    vector<ll> d;
    for (ll i = 1; i * i <= abs(n); i++) {
        if (n % i == 0) {
            d.push_back(i);
            if (i * i != n) d.push_back(n / i);
            if (n < 0) {
                d.push_back(-i);
                if (i * i != n) d.push_back(-n / i);
            }
        }
    }
    sort(d.begin(), d.end());
    return d;
}