#include <bits/stdc++.h>

#include <atcoder/modint>
using namespace std; using namespace atcoder;
using ll=long long; using LL=__int128; using mint=modint998244353; using ld=long double;
using pll=pair<ll,ll>; using tu=tuple<ll,ll,ll>; using tu4=tuple<ll,ll,ll,ll>; using vl=vector<ll>; using vb=vector<bool>; using vvb=vector<vb>; using vvl=vector<vl>; using vs=vector<string>; using vld=vector<ld>; using vc=vector<char>; using vmi=vector<mint>; using vp=vector<pll>; using vt=vector<tu>;
using Graph=vector<vector<pair<ll,ll>>>; using graph=vector<vector<ll>>;
template<class T=ll> auto vec2(ll n,ll m,T x=T()){return vector(n,vector<T>(m,x));}
template<class T=ll> auto vec3(ll n,ll m,ll l,T x=T()){return vector(n,vector(m,vector<T>(l,x)));}
template<class T=ll> auto vec4(ll a,ll b,ll c,ll d,T x=T()){return vector(a,vector(b,vector(c,vector<T>(d,x))));}
template<class T> using rpq=priority_queue<T,vector<T>,greater<T>>;
using inverse_priority_queue=rpq<ll>;
const ll Inf=2147483647LL, Inf9=1000000000LL, inf=9223372036854775807LL, inf18=1000000000000000000LL, mod=998244353LL;
#define all(v) (v).begin(),(v).end()
#define rall(v) (v).rbegin(),(v).rend()
#define rep(i,n) for(ll i=0;i<(ll)(n);i++)
#define rrep(i,n) for(ll i=(ll)(n)-1;i>=0;i--)
#define rep1(i,n) for(ll i=1;i<=(ll)(n);i++)
#define rep3(i,s,t) for(ll i=(ll)(s);i<(ll)(t);i++)
#define pb push_back
#define p(s) cout<<(s)<<'\n'
#define p2(s,t) cout<<(s)<<" "<<(t)<<'\n'
#define p3(s,t,u) cout<<(s)<<" "<<(t)<<" "<<(u)<<'\n'
#define p4(s,t,u,v) cout<<(s)<<" "<<(t)<<" "<<(u)<<" "<<(v)<<'\n'
#define pe(s) cout<<(s)<<' '
#define pval(s) cout<<(s).val()<<'\n'
#define Sort(A) sort(all(A))
#define RSort(A) sort(rall(A))
#define p_yes() p("Yes")
#define p_no() p("No")

template<class T> inline ll sz(const T& x){return (ll)x.size();}
ostream& operator<<(ostream& os,const mint& x){return os<<x.val();}
istream& operator>>(istream& is,mint& x){ll v; is>>v; x=mint(v); return is;}
template<class T> inline bool chmin(T& a,const T& b){if(a>b){a=b; return true;} return false;}
template<class T> inline bool chmax(T& a,const T& b){if(a<b){a=b; return true;} return false;}
template<class T,class U> ll LB(const vector<T>& v,const U& a){return lower_bound(all(v),a)-v.begin();}
template<class T,class U> ll UB(const vector<T>& v,const U& a){return upper_bound(all(v),a)-v.begin();}
template<class T> T vec_min(const vector<T>& v){do { if (!(!v.empty())) std::abort(); } while (false); return *min_element(all(v));}
template<class T> T vec_max(const vector<T>& v){do { if (!(!v.empty())) std::abort(); } while (false); return *max_element(all(v));}
template<class T> T vec_sum(const vector<T>& v){T res=T(0); for(const auto& x:v) res+=x; return res;}

template<class T=ll> vector<T> cin_vl(ll n){vector<T> ret(n); rep(i,n) cin>>ret[i]; return ret;}
template<class T=ll> vector<vector<T>> cin_vvl(ll h,ll w){vector<vector<T>> ret(h,vector<T>(w)); rep(i,h) rep(j,w) cin>>ret[i][j]; return ret;}
template<class T=ll> pair<vector<T>,vector<T>> cin_xy(ll n){vector<T> x(n),y(n); rep(i,n) cin>>x[i]>>y[i]; return {x,y};}
template<class T=ll> tuple<vector<T>,vector<T>,vector<T>> cin_xyz(ll n){vector<T> x(n),y(n),z(n); rep(i,n) cin>>x[i]>>y[i]>>z[i]; return {x,y,z};}
template<class T=ll> vector<vector<T>> Cin_vl(ll n,ll k){vector<vector<T>> ret(k,vector<T>(n)); rep(i,n) rep(j,k) cin>>ret[j][i]; return ret;}
template<class T=ll> vector<pair<T,T>> cin_pairs(ll n){vector<pair<T,T>> ret(n); rep(i,n) cin>>ret[i].first>>ret[i].second; return ret;}
template<class T=ll> vector<tuple<T,T,T>> cin_tuples(ll n){vector<tuple<T,T,T>> ret(n); rep(i,n){T a,b,c; cin>>a>>b>>c; ret[i]={a,b,c};} return ret;}

template<class T> void cout_vl(const vector<T>& a,char sep=' ',char end='\n'){rep(i,sz(a)){if(i) cout<<sep; cout<<a[i];} cout<<end;}
template<class T> void Cout_vl(const vector<T>& a,const vector<T>& b){rep(i,sz(a)) cout<<a[i]<<" "<<b[i]<<'\n';}
template<class T> void cout_vvl(const vector<vector<T>>& a){for(const auto& e:a) cout_vl(e);}

ll ceil_div(ll a,ll b){do { if (!(b!=0)) std::abort(); } while (false); if(b<0) a=-a,b=-b; if(a>=0) return (a+b-1)/b; return -((-a)/b);}
ll floor_div(ll a,ll b){do { if (!(b!=0)) std::abort(); } while (false); if(b<0) a=-a,b=-b; if(a>=0) return a/b; return -((-a+b-1)/b);}
ll isqrt(ll n){ll x=sqrtl(n); while((LL)(x+1)*(x+1)<=n) x++; while((LL)x*x>n) x--; return x;}
template<class T> vector<T> sort_unique_vec(vector<T> v){sort(all(v)); v.erase(unique(all(v)),v.end()); return v;}
template<class T> vector<ll> compress(const vector<T>& v){auto xs=sort_unique_vec(v); vector<ll> ret; for(auto& x:v) ret.pb(LB(xs,x)); return ret;}
template<class T> vector<T> prefix_sum(const vector<T>& v){vector<T> s(sz(v)+1,0); rep(i,sz(v)) s[i+1]=s[i]+v[i]; return s;}
template<class T> T range_sum(const vector<T>& s,ll l,ll r){return s[r]-s[l];}
template<class T> vector<pair<T,ll>> run_length(const vector<T>& v){vector<pair<T,ll>> ret; for(auto& x:v){if(ret.empty()||ret.back().first!=x) ret.pb({x,1}); else ret.back().second++;} return ret;}
vector<pair<char,ll>> run_length(const string& s){vector<pair<char,ll>> ret; for(char c:s){if(ret.empty()||ret.back().first!=c) ret.pb({c,1}); else ret.back().second++;} return ret;}
template<class T> ll argmin(const vector<T>& v){do { if (!(!v.empty())) std::abort(); } while (false); return min_element(all(v))-v.begin();}
template<class T> ll argmax(const vector<T>& v){do { if (!(!v.empty())) std::abort(); } while (false); return max_element(all(v))-v.begin();}
template<class T> bool in_vec(const vector<T>& v,const T& x){return binary_search(all(v),x);}

bool even(ll x){return x%2==0;}
bool odd(ll x){return x%2!=0;}
ll grid_id(ll x,ll y,ll w){return x*w+y;}
pll grid_xy(ll id,ll w){return {id/w,id%w};}

ll gcd(ll a,ll b){if(b==0) return abs(a); return gcd(b,a%b);}
ll lcm(ll a,ll b){return a/gcd(a,b)*b;}
ll Exp(ll x,ll y){ll cnt=-1; while(x>0) x/=y,cnt++; return cnt;}
ll powmod(ll x,ll k,ll m){if(k==0) return 1%m; ll r=powmod(x,k/2,m); r=(LL)r*r%m; if(k&1) r=(LL)r*x%m; return r;}
template<class T> T square(T x){return x*x;}
template<class T> void Uni_erase(vector<T>& v){sort(all(v)); v.erase(unique(all(v)),v.end());}
inline bool OutIn(ll x,ll n){return 0<=x&&x<n;}
inline bool OutIn(ll x,ll y,ll h,ll w){return 0<=x&&x<h&&0<=y&&y<w;}
inline bool OutIn(ll x,ll y,ll z,ll a,ll b,ll c){return 0<=x&&x<a&&0<=y&&y<b&&0<=z&&z<c;}
mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
ll Random(ll l,ll r){uniform_int_distribution<ll> dist(l,r-1); return dist(rng);}
void YesNo(bool x){x?p_yes():p_no();}
vl vl_n(ll n){vl ret(n); rep(i,n) ret[i]=i; return ret;}
vl vl_N(ll n){vl ret(n); rep(i,n) ret[i]=i+1; return ret;}

vl dx={0,1,0,-1}, dy={1,0,-1,0};
vl dx8={1,1,0,-1,-1,-1,0,1}, dy8={0,1,1,1,0,-1,-1,-1};

vector<mint> fac,finv;
void factorial(ll N=1000000LL){ll M=N+100; fac.assign(M+1,1); finv.assign(M+1,1); rep1(i,M) fac[i]=fac[i-1]*i; finv[M]=fac[M].inv(); for(ll i=M;i>=1;i--) finv[i-1]=finv[i]*i;}
void ensure_factorial(ll N){if((ll)fac.size()>N) return; factorial(N);}
mint nCr(ll N,ll r){if(N<0||r<0||N<r) return 0; ensure_factorial(N); return fac[N]*finv[r]*finv[N-r];}
mint nPr(ll N,ll r){if(N<0||r<0||N<r) return 0; ensure_factorial(N); return fac[N]*finv[N-r];}

void solve();
signed main(){
    ios::sync_with_stdio(false); cin.tie(nullptr);
    ll t;cin >> t;rep(i,t)solve();
}
void solve(){
}

/*
細かい注意点
modintのみのテンプレート用の基本テンプレート。bits/stdc++.hとatcoder/modintを読み込む。
ライブラリ本体はmain/solveの外、呼び出し例はmain/solveの中へ貼る。
整数演算の中間値は各型に収まること。vec_min/max/argmin/maxは非空、除算のbは非0。
LB/UB/in_vecはsort済み、Randomはl<r、Expはx>0かつy>=2。
階乗の前計算範囲は法998244353未満。nCr/nPrは引数が範囲外なら0。
前提違反を検出するチェックはメッセージなしで停止する。
ll/LL/mintなどの型別名と、graph/Graph、各マクロを定義する。
ACLのsegtree/lazy_segtreeを使う場合は対応ヘッダーを追加する。

使える関数一覧
vec2(n,m,x), vec3(n,m,l,x), vec4(a,b,c,d,x); // 多次元vector生成 O(要素数)
sz(v);                                     // ll: 要素数 O(1)
chmin(a,b), chmax(a,b);                    // bool: 更新したか O(1)
LB(v,x), UB(v,x);                         // ll: sort済み列の挿入位置 O(log N)
vec_min(v), vec_max(v), vec_sum(v);        // T: 最小・最大・和 O(N)
cin_vl<T>(n);                             // vector<T>: 入力 O(n)
cin_vvl<T>(h,w);                         // vector<vector<T>>: 入力 O(hw)
cin_xy<T>(n), cin_xyz<T>(n);               // pair/tuple<vector<T>,...>: 列ごと入力 O(n)
Cin_vl<T>(n,k);                           // vector<vector<T>>: n行k列を列ごと保持 O(nk)
cin_pairs<T>(n), cin_tuples<T>(n);         // pair/tuple列入力 O(n)
cout_vl(v,sep=' ',end='\n');               // void: 列出力 O(N)
Cout_vl(a,b), cout_vvl(a);                 // void: 2列/多次元列出力 O(要素数)
Swap(v,h,w);                             // void: 行列転置しh,wも交換 O(hw)
ceil_div(a,b), floor_div(a,b);            // ll: 切り上げ/切り下げ除算 O(1)
isqrt(n);                                // ll: floor(sqrt(n))
sort_unique_vec(v);                      // vector<T>: sort・重複除去 O(N log N)
compress(v);                             // vector<ll>: 座標圧縮 O(N log N)
prefix_sum(v), prefix_sum2(a);            // 累積和 O(要素数)
range_sum(s,l,r);                        // T: [l,r)の累積和差 O(1)
run_length(v);                           // vector<pair<T,ll>>: ランレングス圧縮 O(N)
argmin(v), argmax(v);                     // ll: 最小/最大位置 O(N)
in_vec(v,x);                             // bool: sort済み列の存在判定 O(log N)
even(x), odd(x);                         // bool: 偶数/奇数 O(1)
grid_id(x,y,w), grid_xy(id,w);            // ll / pair<ll,ll>: 座標とIDの変換 O(1)
gcd(a,b), lcm(a,b);                      // ll: 最大公約数/最小公倍数 O(log max(|a|,|b|))
Exp(x,y);                                // ll: 正数xのy進桁数-1 O(log_y x)
powmod(x,k,m);                           // ll: x^k mod m O(log k)
square(x);                               // T: 二乗 O(1)
Uni_erase(v);                            // void: sort・重複除去 O(N log N)
OutIn(x,n), OutIn(x,y,h,w), OutIn(x,y,z,a,b,c); // bool: 範囲内判定 O(1)
Random(l,r);                             // ll: [l,r)の乱数 O(1)
YesNo(flag);                             // void: Yes/Noを出力
vl_n(n), vl_N(n);                        // vector<ll>: 0..n-1 / 1..n O(n)
factorial(N=1000000);                     // void: N+100まで階乗・逆階乗・2の冪 O(N)
ensure_factorial(N);                     // void: 階乗表不足時に再構築 O(N)
nCr(N,r), nPr(N,r);                      // mint: 二項係数/順列数、前計算後O(1)
all(v), rall(v), rep(i,n), rrep(i,n), rep1(i,n), rep3(i,s,t), Re(n); // 範囲・反復マクロ
p(x), p2(a,b), p3(a,b,c), p4(a,b,c,d), pe(x), pval(x), p_yes(), p_no(); // 出力マクロ
rpq<T>, inverse_priority_queue;           // 小さい順priority_queue
dx,dy,dx8,dy8;                           // 4近傍/8近傍の方向配列

基本の関数
vl A={3,1,3};
auto sum = vec_sum(A);                        // 7
auto compressed = compress(A);               // {1,0,1}
factorial(10);
auto combinations = nCr(5,2);                // 10
*/
