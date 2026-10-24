// https://github.com/yuki-4869/Algorithm/blob/main/template.cpp
#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std; using namespace atcoder;
using ll=long long; using LL=__int128; using mint=modint998244353;
using ld = long double;
using pll=pair<ll,ll>; using tu=tuple<ll,ll,ll>; using tu4 = tuple<ll,ll,ll,ll>;using vl=vector<ll>; using vb=vector<bool>; using vvb=vector<vb>; using vvl=vector<vl>; using vs=vector<string>; using vld=vector<long double>; using vc=vector<char>; using vmi=vector<mint>; using vp = vector<pll>; using vt = vector<tu>;using graph = vvl; using Graph = vector<vp>;
template<class T=ll> auto vec2(ll n,ll m,T x=T()){return vector(n,vector<T>(m,x));} template<class T=ll> auto vec3(ll n,ll m,ll l,T x=T()){return vector(n,vector(m,vector<T>(l,x)));}
template<class T=ll> auto vec4(ll a,ll b,ll c,ll d,T x=T()){return vector(a,vector(b,vector(c,vector<T>(d,x))));}
template<class T> using rpq=priority_queue<T,vector<T>,greater<T>>; // 小さい順priority_queue
using inverse_priority_queue=rpq<ll>;
const ll Inf=2147483647LL, Inf9=1000000000LL, inf=9223372036854775807LL, inf18=1000000000000000000LL, mod=998244353LL;
#define all(v) (v).begin(),(v).end()
#define rall(v) (v).rbegin(),(v).rend()
#define Re(n) for(ll i = 0; i<(ll)(n);i++)
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

template<class T> inline ll sz(const T& x){return (ll)x.size();} // サイズ取得
ostream& operator<<(ostream& os,const mint& x){return os<<x.val();} // mint出力
istream& operator>>(istream& is,mint& x){ll v; is>>v; x=mint(v); return is;} // mint入力
template<class T> inline bool chmin(T& a,const T& b){if(a>b){a=b; return true;} return false;} // 最小値更新
template<class T> inline bool chmax(T& a,const T& b){if(a<b){a=b; return true;} return false;} // 最大値更新
template<class T,class U> ll LB(const vector<T>& v,const U& a){return lower_bound(all(v),a)-v.begin();} // lower_boundのindex
template<class T,class U> ll UB(const vector<T>& v,const U& a){return upper_bound(all(v),a)-v.begin();} // upper_boundのindex
template<class T> T vec_min(const vector<T>& v){do { if (!(!v.empty())) std::abort(); } while (false); return *min_element(all(v));} // vectorの最小値
template<class T> T vec_max(const vector<T>& v){do { if (!(!v.empty())) std::abort(); } while (false); return *max_element(all(v));} // vectorの最大値
template<class T> T vec_sum(const vector<T>& v){T res=T(0); for(const auto& x:v) res+=x; return res;} // vectorの総和

template<class T=ll> vector<T> cin_vl(ll n){vector<T> ret(n); rep(i,n) cin>>ret[i]; return ret;} // vector入力、型省略時ll
template<class T=ll> vector<vector<T>> cin_vvl(ll h,ll w){vector<vector<T>> ret(h,vector<T>(w)); rep(i,h) rep(j,w) cin>>ret[i][j]; return ret;} // h*w入力、型省略時ll
template<class T=ll> pair<vector<T>,vector<T>> cin_xy(ll n){vector<T> x(n),y(n); rep(i,n) cin>>x[i]>>y[i]; return {x,y};} // x,y入力、型省略時ll
template<class T=ll> tuple<vector<T>,vector<T>,vector<T>> cin_xyz(ll n){vector<T> x(n),y(n),z(n); rep(i,n) cin>>x[i]>>y[i]>>z[i]; return {x,y,z};} // x,y,z入力、型省略時ll
template<class T=ll> vector<vector<T>> Cin_vl(ll n,ll k){vector<vector<T>> ret(k,vector<T>(n)); rep(i,n) rep(j,k) cin>>ret[j][i]; return ret;} // n行k列入力を列ごと保持、型省略時ll
template<class T=ll> vector<pair<T,T>> cin_pairs(ll n){vector<pair<T,T>> ret(n); rep(i,n) cin>>ret[i].first>>ret[i].second; return ret;} // pair列入力、型省略時ll
template<class T=ll> vector<tuple<T,T,T>> cin_tuples(ll n){vector<tuple<T,T,T>> ret(n); rep(i,n){T a,b,c; cin>>a>>b>>c; ret[i]={a,b,c};} return ret;} // 3要素tuple列入力、型省略時ll

template<class T> void cout_vl(const vector<T>& a,char sep=' ',char end='\n'){rep(i,sz(a)){if(i) cout<<sep; cout<<a[i];} cout<<end;} // vector出力
template<class T> void Cout_vl(const vector<T>& a,const vector<T>& b){rep(i,sz(a)) cout<<a[i]<<" "<<b[i]<<'\n';} // 2vector縦出力
template<class T> void cout_vvl(const vector<vector<T>>& a){for(const auto& e:a) cout_vl(e);} // 2次元vector出力
template<class T=ll> void Swap(vector<vector<T>>& v,ll& h,ll& w){vector<vector<T>> ret(w,vector<T>(h));rep(i,h) rep(j,w) ret[j][i]=v[i][j];v.swap(ret);swap(h,w);} // h*w行列をw*h行列に変換 hとwを自動でswap

ll ceil_div(ll a,ll b){do { if (!(b!=0)) std::abort(); } while (false); if(b<0) a=-a,b=-b; if(a>=0) return (a+b-1)/b; return -((-a)/b);} // 切り上げ除算
ll floor_div(ll a,ll b){do { if (!(b!=0)) std::abort(); } while (false); if(b<0) a=-a,b=-b; if(a>=0) return a/b; return -((-a+b-1)/b);} // 切り下げ除算
ll isqrt(ll n){ll x=sqrtl(n); while((LL)(x+1)*(x+1)<=n) x++; while((LL)x*x>n) x--; return x;} // floor(sqrt(n))
template<class T> vector<T> sort_unique_vec(vector<T> v){sort(all(v)); v.erase(unique(all(v)),v.end()); return v;} // sortして重複削除した新vector
template<class T> vector<ll> compress(const vector<T>& v){auto xs=sort_unique_vec(v); vector<ll> ret; for(auto& x:v) ret.pb(LB(xs,x)); return ret;} // 座標圧縮後のindex列
template<class T> vector<T> prefix_sum(const vector<T>& v){vector<T> s(sz(v)+1,0); rep(i,sz(v)) s[i+1]=s[i]+v[i]; return s;} // 1次元累積和
template<class T> vector<vector<T>> prefix_sum2(const vector<vector<T>>& a){int h=sz(a),w=sz(a[0]); vector<vector<T>> s(h+1,vector<T>(w+1,0)); rep(i,h)rep(j,w)s[i+1][j+1]=s[i+1][j]+s[i][j+1]-s[i][j]+a[i][j]; return s;} // 2次元累積和
template<class T> T range_sum(const vector<T>& s,ll l,ll r){return s[r]-s[l];} // 累積和から[l,r)の和
template<class T> vector<pair<T,ll>> run_length(const vector<T>& v){vector<pair<T,ll>> ret; for(auto& x:v){if(ret.empty()||ret.back().first!=x) ret.pb({x,1}); else ret.back().second++;} return ret;} // ランレングス圧縮
vector<pair<char,ll>> run_length(const string& s){vector<pair<char,ll>> ret; for(char c:s){if(ret.empty()||ret.back().first!=c) ret.pb({c,1}); else ret.back().second++;} return ret;} // 文字列ランレングス圧縮
template<class T> ll argmin(const vector<T>& v){do { if (!(!v.empty())) std::abort(); } while (false); return min_element(all(v))-v.begin();} // 最小値のindex
template<class T> ll argmax(const vector<T>& v){do { if (!(!v.empty())) std::abort(); } while (false); return max_element(all(v))-v.begin();} // 最大値のindex
template<class T> bool in_vec(const vector<T>& v,const T& x){return binary_search(all(v),x);} // sort済vectorに存在するか

bool even(ll x){return x%2==0;}
bool odd(ll x){return x%2!=0;}
ll grid_id(ll x,ll y,ll w){return x*w+y;} // 座標からGridのID
pll grid_xy(ll id,ll w){return {id/w,id%w};} // GridのIDから座標

ll gcd(ll a,ll b){if(b==0) return abs(a); return gcd(b,a%b);} // 最大公約数
ll lcm(ll a,ll b){return a/gcd(a,b)*b;} // 最小公倍数
ll Exp(ll x,ll y){ll cnt=-1; while(x>0) x/=y,cnt++; return cnt;} // y進桁数-1
ll powmod(ll x,ll k,ll m){if(k==0) return 1%m; ll r=powmod(x,k/2,m); r=(LL)r*r%m; if(k&1) r=(LL)r*x%m; return r;} // mod累乗
template<class T> T square(T x){return x*x;} // 二乗
template<class T> void Uni_erase(vector<T>& v){sort(all(v)); v.erase(unique(all(v)),v.end());} // sortして重複削除
inline bool OutIn(ll x,ll n){return 0<=x&&x<n;}
inline bool OutIn(ll x,ll y,ll h,ll w){return 0<=x&&x<h&&0<=y&&y<w;}
inline bool OutIn(ll x,ll y,ll z,ll a,ll b,ll c){return 0<=x&&x<a&&0<=y&&y<b&&0<=z&&z<c;} // グリッド範囲内判定
mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
ll Random(ll l,ll r){uniform_int_distribution<ll> dist(l,r-1); return dist(rng);} // [l,r)乱数
void YesNo(bool x){x?p_yes():p_no();} // Yes/No出力
vl vl_n(ll n){vl ret(n); rep(i,n) ret[i]=i; return ret;} // 0..n-1のvector生成
vl dijkstra(const graph& g,const vl& starts){ vl dist(g.size(),inf); queue<ll> q; for(ll s:starts) if(dist[s]==inf){dist[s]=0; q.push(s);} while(!q.empty()){ ll v=q.front(); q.pop(); for(ll to:g[v]) if(dist[to]==inf){dist[to]=dist[v]+1; q.push(to);} } return dist; }
vl dijkstra(const graph& g,ll s){return dijkstra(g,vl{s});}
vl dijkstra(const graph& g,initializer_list<ll> starts){return dijkstra(g,vl(starts));}
vl dijkstra(const Graph& g,const vl& starts){ vl dist(g.size(),inf); rpq<pll> q; for(ll s:starts) if(dist[s]==inf){dist[s]=0; q.push({0,s});} while(!q.empty()){ auto [d,v]=q.top(); q.pop(); if(d!=dist[v]) continue; for(auto [to,w]:g[v]){ if(w>=inf-d) continue; ll nd=d+w; if(nd<dist[to]){dist[to]=nd; q.push({nd,to});} } } return dist; }
vl dijkstra(const Graph& g,ll s){return dijkstra(g,vl{s});}
vl dijkstra(const Graph& g,initializer_list<ll> starts){return dijkstra(g,vl(starts));}

const vl Pow2={1LL,2LL,4LL,8LL,16LL,32LL,64LL,128LL,256LL,512LL,1024LL,2048LL,4096LL,8192LL,16384LL,32768LL,65536LL,131072LL,262144LL,524288LL,1048576LL,2097152LL,4194304LL,8388608LL,16777216LL,33554432LL,67108864LL,134217728LL,268435456LL,536870912LL,1073741824LL,2147483648LL,4294967296LL,8589934592LL,17179869184LL,34359738368LL,68719476736LL,137438953472LL,274877906944LL,549755813888LL,1099511627776LL,2199023255552LL,4398046511104LL,8796093022208LL,17592186044416LL,35184372088832LL,70368744177664LL,140737488355328LL,281474976710656LL,562949953421312LL,1125899906842624LL,2251799813685248LL,4503599627370496LL,9007199254740992LL,18014398509481984LL,36028797018963968LL,72057594037927936LL,144115188075855872LL,288230376151711744LL,576460752303423488LL,1152921504606846976LL};

vl vl_N(ll n){vl ret(n); rep(i,n) ret[i]=i+1; return ret;} // 1..nのvector生成

vl dx={0,1,0,-1}, dy={1,0,-1,0};
vl dx8={1,1,0,-1,-1,-1,0,1}, dy8={0,1,1,1,0,-1,-1,-1}; // 8近傍

vector<mint> fac,finv,pow2;
void factorial(ll N=1000000LL){ll M=N+100; fac.assign(M+1,1); finv.assign(M+1,1); pow2.assign(M+1,1); rep1(i,M) fac[i]=fac[i-1]*i; finv[M]=fac[M].inv(); for(ll i=M;i>=1;i--) finv[i-1]=finv[i]*i; rep1(i,M) pow2[i]=pow2[i-1]*2;} // N+100まで階乗・逆階乗・2の累乗前計算
void ensure_factorial(ll N){if((ll)fac.size()>N) return; factorial(N);} // 階乗表の不足分確保
mint nCr(ll N,ll r){if(N<0||r<0||N<r) return 0; ensure_factorial(N); return fac[N]*finv[r]*finv[N-r];} // 二項係数
mint nPr(ll N,ll r){if(N<0||r<0||N<r) return 0; ensure_factorial(N); return fac[N]*finv[N-r];} // 順列数

signed main(){
    ios::sync_with_stdio(false); cin.tie(nullptr);
	