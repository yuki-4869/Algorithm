// https://github.com/yuki-4869/Algorithm/blob/main/Templates/codeforces.cpp
#include <bits/stdc++.h>
#include <atcoder/modint>
using namespace std; using namespace atcoder;
using ll=long long; using LL=__int128; using mint=modint998244353;
using ld = long double;
using pll=pair<ll,ll>; using tu=tuple<ll,ll,ll>; using tu4 = tuple<ll,ll,ll,ll>;using vl=vector<ll>; using vb=vector<bool>; using vvb=vector<vb>; using vvl=vector<vl>; using vs=vector<string>; using vld=vector<long double>; using vc=vector<char>; using vmi=vector<mint>; using vp = vector<pll>; using vt = vector<tu>;using graph = vvl; using Graph = vector<vp>;
template<class T=ll> auto vec2(ll n,ll m,T x=T()){return vector(n,vector<T>(m,x));} template<class T=ll> auto vec3(ll n,ll m,ll l,T x=T()){return vector(n,vector(m,vector<T>(l,x)));}
template<class T=ll> auto vec4(ll a,ll b,ll c,ll d,T x=T()){return vector(a,vector(b,vector(c,vector<T>(d,x))));}
template<class T> using rpq=priority_queue<T,vector<T>,greater<T>>;
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
template<class T=ll> void Swap(vector<vector<T>>& v,ll& h,ll& w){vector<vector<T>> ret(w,vector<T>(h));rep(i,h) rep(j,w) ret[j][i]=v[i][j];v.swap(ret);swap(h,w);}

ll ceil_div(ll a,ll b){do { if (!(b!=0)) std::abort(); } while (false); if(b<0) a=-a,b=-b; if(a>=0) return (a+b-1)/b; return -((-a)/b);}
ll floor_div(ll a,ll b){do { if (!(b!=0)) std::abort(); } while (false); if(b<0) a=-a,b=-b; if(a>=0) return a/b; return -((-a+b-1)/b);}
ll isqrt(ll n){ll x=sqrtl(n); while((LL)(x+1)*(x+1)<=n) x++; while((LL)x*x>n) x--; return x;}
template<class T> vector<T> sort_unique_vec(vector<T> v){sort(all(v)); v.erase(unique(all(v)),v.end()); return v;}
template<class T> vector<ll> compress(const vector<T>& v){auto xs=sort_unique_vec(v); vector<ll> ret; for(auto& x:v) ret.pb(LB(xs,x)); return ret;}
template<class T> vector<T> prefix_sum(const vector<T>& v){vector<T> s(sz(v)+1,0); rep(i,sz(v)) s[i+1]=s[i]+v[i]; return s;}
template<class T> vector<vector<T>> prefix_sum2(const vector<vector<T>>& a){int h=sz(a),w=sz(a[0]); vector<vector<T>> s(h+1,vector<T>(w+1,0)); rep(i,h)rep(j,w)s[i+1][j+1]=s[i+1][j]+s[i][j+1]-s[i][j]+a[i][j]; return s;}
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
vl dijkstra(const graph& g,const vl& starts){ vl dist(g.size(),inf); queue<ll> q; for(ll s:starts) if(dist[s]==inf){dist[s]=0; q.push(s);} while(!q.empty()){ ll v=q.front(); q.pop(); for(ll to:g[v]) if(dist[to]==inf){dist[to]=dist[v]+1; q.push(to);} } return dist; }
vl dijkstra(const graph& g,ll s){return dijkstra(g,vl{s});}
vl dijkstra(const graph& g,initializer_list<ll> starts){return dijkstra(g,vl(starts));}
vl dijkstra(const Graph& g,const vl& starts){ vl dist(g.size(),inf); rpq<pll> q; for(ll s:starts) if(dist[s]==inf){dist[s]=0; q.push({0,s});} while(!q.empty()){ auto [d,v]=q.top(); q.pop(); if(d!=dist[v]) continue; for(auto [to,w]:g[v]){ if(w>=inf-d) continue; ll nd=d+w; if(nd<dist[to]){dist[to]=nd; q.push({nd,to});} } } return dist; }
vl dijkstra(const Graph& g,ll s){return dijkstra(g,vl{s});}
vl dijkstra(const Graph& g,initializer_list<ll> starts){return dijkstra(g,vl(starts));}

const vl Pow2={1LL,2LL,4LL,8LL,16LL,32LL,64LL,128LL,256LL,512LL,1024LL,2048LL,4096LL,8192LL,16384LL,32768LL,65536LL,131072LL,262144LL,524288LL,1048576LL,2097152LL,4194304LL,8388608LL,16777216LL,33554432LL,67108864LL,134217728LL,268435456LL,536870912LL,1073741824LL,2147483648LL,4294967296LL,8589934592LL,17179869184LL,34359738368LL,68719476736LL,137438953472LL,274877906944LL,549755813888LL,1099511627776LL,2199023255552LL,4398046511104LL,8796093022208LL,17592186044416LL,35184372088832LL,70368744177664LL,140737488355328LL,281474976710656LL,562949953421312LL,1125899906842624LL,2251799813685248LL,4503599627370496LL,9007199254740992LL,18014398509481984LL,36028797018963968LL,72057594037927936LL,144115188075855872LL,288230376151711744LL,576460752303423488LL,1152921504606846976LL};

vl vl_N(ll n){vl ret(n); rep(i,n) ret[i]=i+1; return ret;}

vl dx={0,1,0,-1}, dy={1,0,-1,0};
vl dx8={1,1,0,-1,-1,-1,0,1}, dy8={0,1,1,1,0,-1,-1,-1};

vector<mint> fac,finv,pow2;
void factorial(ll N=1000000LL){ll M=N+100; fac.assign(M+1,1); finv.assign(M+1,1); pow2.assign(M+1,1); rep1(i,M) fac[i]=fac[i-1]*i; finv[M]=fac[M].inv(); for(ll i=M;i>=1;i--) finv[i-1]=finv[i]*i; rep1(i,M) pow2[i]=pow2[i-1]*2;}
void ensure_factorial(ll N){if((ll)fac.size()>N) return; factorial(N);}
mint nCr(ll N,ll r){if(N<0||r<0||N<r) return 0; ensure_factorial(N); return fac[N]*finv[r]*finv[N-r];}
mint nPr(ll N,ll r){if(N<0||r<0||N<r) return 0; ensure_factorial(N); return fac[N]*finv[N-r];}

void solve();
signed main(){
    ios::sync_with_stdio(false); cin.tie(nullptr);
    ll t;cin >> t;rep(i,t)solve();
}
void solve(){
	