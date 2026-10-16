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

/*
細かい注意点
正のnの正の約数を昇順で返す用途で使用する。n=0は約数列挙の対象外。
ループ内のi*iがlong longに収まる値域で使用する。
負数の約数列挙には使用しない。

使える関数一覧
get_divisors(n); // vector<ll>: 正の約数を昇順で列挙 O(sqrt(n)+D log D)、Dは約数個数

基本の関数
auto divisors = get_divisors(12);             // {1,2,3,4,6,12}
*/
