ll ok = , ng = ;

auto check = [&](ll x){
    return ;
};

while(ok-ng>1){
    ll mid = std::midpoint(ok,ng);
    if(check(mid)) ok = mid;
    else ng = mid;
}

/*
細かい注意点
関数内に貼る雛形。空欄のok/ngとcheckの判定を問題に合わせて記入する。
ok>ng、check(ok)==true、check(ng)==false。判定は境界に関して単調であること。
ok-ngがlong longに収まること。std::midpointを使うためC++20以降。
繰り返し回数O(log(ok-ng))。実際の計算量はこれにcheckの時間を掛ける。

使える関数一覧
check(x); // bool: 利用者が定義する単調な判定
ok;       // 実行後は条件を満たす最小値

基本の関数
ll ok=100, ng=-1;
auto check=[](ll x){return x>=37;};
while(ok-ng>1){
    ll mid=std::midpoint(ok,ng);
    if(check(mid)) ok=mid;
    else ng=mid;
}                                            // ok==37
*/
