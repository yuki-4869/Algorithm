namespace ext_euclid {
using ll = long long;
using i128 = __int128_t;

struct Result { ll g,x,y; };

Result extgcd(ll a,ll b){
    i128 r0=a,r1=b,x0=1,x1=0,y0=0,y1=1;
    while(r1){
        i128 q=r0/r1;
        tie(r0,r1)=pair{r1,r0-q*r1};
        tie(x0,x1)=pair{x1,x0-q*x1};
        tie(y0,y1)=pair{y1,y0-q*y1};
    }
    if(r0<0) r0=-r0,x0=-x0,y0=-y0;
    return {(ll)r0,(ll)x0,(ll)y0};
}

pair<ll,ll> bezout(ll a,ll b){
    auto [g,x,y]=extgcd(a,b);
    return {x,y};
}

optional<ll> inverse(ll a,ll mod){
    do { if (!(mod>0)) std::abort(); } while (false);
    auto [g,x,y]=extgcd(a,mod);
    if(g!=1) return nullopt;
    x%=mod;
    if(x<0) x+=mod;
    return x;
}

struct Diophantine { ll x,y,dx,dy,g; };

optional<Diophantine> solve(ll a,ll b,ll c){
    do { if (!(a!=0||b!=0)) std::abort(); } while (false);
    auto [g,x,y]=extgcd(a,b);
    if(c%g) return nullopt;
    i128 k=c/g;
    return Diophantine{(ll)(x*k),(ll)(y*k),b/g,-a/g,g};
}

struct Congruence { ll x,mod; };

optional<Congruence> congruence(ll a,ll b,ll mod){
    do { if (!(mod>0)) std::abort(); } while (false);
    auto [g,x,y]=extgcd(a,mod);
    if(b%g) return nullopt;
    ll m=mod/g;
    i128 z=(i128)x*(b/g)%m;
    if(z<0) z+=m;
    return Congruence{(ll)z,m};
}

optional<ll> divide_mod(ll a,ll b,ll mod){
    auto inv=inverse(b,mod);
    if(!inv) return nullopt;
    i128 x=a%mod;
    if(x<0) x+=mod;
    return (ll)(x*(*inv)%mod);
}
}

/*
細かい注意点
ax+by=gcd(a,b)の係数を求める。返すgは非負。
inverse/divide_mod/congruenceのmodは正。solveはa,bの少なくとも一方が非0。
逆元はgcd(a,mod)=1のときだけ存在。ax+by=cはgcd(a,b)がcを割るときだけ可解。
解・係数・gcdはlong longに収まること。解なしはnulloptで返す。
solveの全解はx=s.x+s.dx*t、y=s.y+s.dy*t。congruenceはx≡s.x (mod s.mod)。

使える関数一覧
auto [g,x,y]=ext_euclid::extgcd(a,b);          // ax+by=gcd(a,b), g>=0
auto [x,y]=ext_euclid::bezout(a,b);             // Bézout係数のみ

auto inv=ext_euclid::inverse(a,mod);            // a^{-1} mod mod、なければ nullopt
auto x=ext_euclid::divide_mod(a,b,mod);         // a/b mod mod、逆元がなければ nullopt

auto s=ext_euclid::solve(a,b,c);                // ax+by=c、解なしなら nullopt
                                                // x=s->x+s->dx*t
                                                // y=s->y+s->dy*t

auto s=ext_euclid::congruence(a,b,m);           // ax≡b (mod m)、解なしなら nullopt
                                                // x≡s->x (mod s->mod)

基本の関数
auto result = ext_euclid::extgcd(30,18);       // g=6, 30*x+18*y=6
auto inverse = ext_euclid::inverse(3,7);      // 5
auto solution = ext_euclid::solve(6,9,3);     // 6x+9y=3の解
*/
