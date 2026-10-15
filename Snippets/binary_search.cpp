ll ok = , ng = ;

auto check = [&](ll x){

    return ;
};

while(ok-ng>1){
    ll mid = std::midpoint(ok,ng);
    if(check(mid)) ok = mid;
    else ng = mid;
}

