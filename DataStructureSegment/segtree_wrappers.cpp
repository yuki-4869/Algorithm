template<class S,S(*op)(S,S),S(*e)()>
struct SegBase:segtree<S,op,e>{
    using Base=segtree<S,op,e>;
    using Base::Base;
};

namespace SegMax_impl{ll op(ll a,ll b){return max(a,b);}ll e(){return -inf;}}
namespace SegMin_impl{ll op(ll a,ll b){return min(a,b);}ll e(){return inf;}}
namespace SegSum_impl{ll op(ll a,ll b){return a+b;}ll e(){return 0;}}
namespace SegXOR_impl{ll op(ll a,ll b){return a^b;}ll e(){return 0;}}
namespace SegOR_impl{ll op(ll a,ll b){return a|b;}ll e(){return 0;}}
namespace SegAND_impl{ll op(ll a,ll b){return a&b;}ll e(){return ~0LL;}}
namespace SegGCD_impl{ll op(ll a,ll b){return gcd(a,b);}ll e(){return 0;}}

using SegMax=SegBase<ll,SegMax_impl::op,SegMax_impl::e>;
using SegMin=SegBase<ll,SegMin_impl::op,SegMin_impl::e>;
using SegSum=SegBase<ll,SegSum_impl::op,SegSum_impl::e>;
using SegXOR=SegBase<ll,SegXOR_impl::op,SegXOR_impl::e>;
using SegOR=SegBase<ll,SegOR_impl::op,SegOR_impl::e>;
using SegAND=SegBase<ll,SegAND_impl::op,SegAND_impl::e>;
using SegGCD=SegBase<ll,SegGCD_impl::op,SegGCD_impl::e>;

namespace SegMinMax_impl{
    struct S{ll mx,mn;};
    S op(S a,S b){return {max(a.mx,b.mx),min(a.mn,b.mn)};}
    S e(){return {-inf,inf};}
}
struct SegMinMax:SegBase<SegMinMax_impl::S,SegMinMax_impl::op,SegMinMax_impl::e>{
    using S=SegMinMax_impl::S;
    using Base=SegBase<S,SegMinMax_impl::op,SegMinMax_impl::e>;
    using Base::Base; using Base::set;
    static vector<S> cv(const vector<ll>&a){vector<S>v;v.reserve(a.size());for(ll x:a)v.pb({x,x});return v;}
    SegMinMax(const vector<ll>&a):Base(cv(a)){}
    void set(int i,ll x){Base::set(i,{x,x});}
};

namespace SegArgMax_impl{
    struct S{ll val,id;};
    S op(S a,S b){return a.val!=b.val?(a.val>b.val?a:b):(a.id<b.id?a:b);}
    S e(){return {-inf,inf};}
}
struct SegArgMax:SegBase<SegArgMax_impl::S,SegArgMax_impl::op,SegArgMax_impl::e>{
    using S=SegArgMax_impl::S;
    using Base=SegBase<S,SegArgMax_impl::op,SegArgMax_impl::e>;
    using Base::Base; using Base::set;
    static vector<S> cv(const vector<ll>&a){vector<S>v;rep(i,sz(a))v.pb({a[i],i});return v;}
    SegArgMax(const vector<ll>&a):Base(cv(a)){}
    void set(int i,ll x){Base::set(i,{x,i});}
};

namespace SegArgMin_impl{
    struct S{ll val,id;};
    S op(S a,S b){return a.val!=b.val?(a.val<b.val?a:b):(a.id<b.id?a:b);}
    S e(){return {inf,inf};}
}
struct SegArgMin:SegBase<SegArgMin_impl::S,SegArgMin_impl::op,SegArgMin_impl::e>{
    using S=SegArgMin_impl::S;
    using Base=SegBase<S,SegArgMin_impl::op,SegArgMin_impl::e>;
    using Base::Base; using Base::set;
    static vector<S> cv(const vector<ll>&a){vector<S>v;rep(i,sz(a))v.pb({a[i],i});return v;}
    SegArgMin(const vector<ll>&a):Base(cv(a)){}
    void set(int i,ll x){Base::set(i,{x,i});}
};

namespace SegMaxSubarray_impl{
    const ll NEG=-(1LL<<60);
    struct S{ll sum,pref,suff,best;};
    S gen(ll x){return {x,x,x,x};}
    S op(S a,S b){
        return {a.sum+b.sum,max(a.pref,a.sum+b.pref),max(b.suff,b.sum+a.suff),max({a.best,b.best,a.suff+b.pref})};
    }
    S e(){return {0,NEG,NEG,NEG};}
}
struct SegMaxSubarray:SegBase<SegMaxSubarray_impl::S,SegMaxSubarray_impl::op,SegMaxSubarray_impl::e>{
    using S=SegMaxSubarray_impl::S;
    using Base=SegBase<S,SegMaxSubarray_impl::op,SegMaxSubarray_impl::e>;
    using Base::Base; using Base::set;
    static vector<S> cv(const vector<ll>&a){vector<S>v;v.reserve(a.size());for(ll x:a)v.pb(SegMaxSubarray_impl::gen(x));return v;}
    SegMaxSubarray(const vector<ll>&a):Base(cv(a)){}
    void set(int i,ll x){Base::set(i,SegMaxSubarray_impl::gen(x));}
};

namespace SegAffine_impl{
    struct S{
        mint a,b;
        mint eval(mint x)const{return a*x+b;}
    };
    S op(S f,S g){return {g.a*f.a,g.a*f.b+g.b};}
    S e(){return {1,0};}
}
struct SegAffine:SegBase<SegAffine_impl::S,SegAffine_impl::op,SegAffine_impl::e>{
    using S=SegAffine_impl::S;
    using Base=SegBase<S,SegAffine_impl::op,SegAffine_impl::e>;
    using Base::Base;
};

namespace SegBracket_impl{
    struct S{ll sum,mn;};
    S gen(char c){return c=='('?S{1,0}:S{-1,-1};}
    S op(S a,S b){return {a.sum+b.sum,min(a.mn,a.sum+b.mn)};}
    S e(){return {0,0};}
}
struct SegBracket:SegBase<SegBracket_impl::S,SegBracket_impl::op,SegBracket_impl::e>{
    using S=SegBracket_impl::S;
    using Base=SegBase<S,SegBracket_impl::op,SegBracket_impl::e>;
    using Base::Base; using Base::set;
    static vector<S> cv(const string&s){vector<S>v;v.reserve(s.size());for(char c:s)v.pb(SegBracket_impl::gen(c));return v;}
    SegBracket(const string&s):Base(cv(s)){}
    void set(int i,char c){Base::set(i,SegBracket_impl::gen(c));}
    bool valid(int l,int r){auto x=Base::prod(l,r);return x.sum==0&&x.mn>=0;}
};

/*
細かい注意点
0-indexed、全区間は[l,r)。opは結合則、eは単位元を満たす。
max_right/min_leftの述語は副作用なし・単調・f(e())==true。
Arg型の同値は最左位置。MaxSubarrayは空部分列を認めない。
SegMinMax::S={mx,mn}、Arg::S={val,id}、MaxSubarray::S={sum,pref,suff,best}。
SegAffine::S={a,b}はf[r-1]∘...∘f[l]、SegBracket::S={sum,mn}。
整数演算がlong longに収まること。codecfではatcoder/segtreeを追加で読み込む。

使える関数一覧
SegArgMax seg(v) 区間最大値とその最左位置を管理 O(N)
pair相当 seg.prod(l,r) {最大値,その最左index}を返す O(logN)
void seg.set(i,x) A[i]=xに更新 O(logN)
SegArgMin seg(v) 区間最小値とその最左位置を管理 O(N)
pair相当 seg.prod(l,r) {最小値,その最左index}を返す O(logN)
void seg.set(i,x) A[i]=xに更新 O(logN)

SegMaxSubarray seg(v) 動的な最大部分配列和を管理、空部分列は不可 O(N)
S seg.prod(l,r) {sum,pref,suff,best}を返す O(logN)
void seg.set(i,x) A[i]=xに更新 O(logN)

SegAffine seg(v) 一次関数f(x)=ax+bの列を合成して管理 O(N)
S seg.prod(l,r) f[r-1]∘...∘f[l]を{a,b}で返す O(logN)
mint seg.prod(l,r).eval(x) 区間の合成関数をxに適用 O(logN)

SegBracket seg(s) 括弧列を構築 O(N)
S seg.prod(l,r) [l,r)の{総和,最小prefix和}を返す O(logN)
bool seg.valid(l,r) [l,r)が正しい括弧列か判定 O(logN)
void seg.set(i,c) i番目を'('または')'に更新 O(logN)

void seg.set(i,x) i番目をxに更新 O(logN)
S seg.get(i) i番目の値を返す O(1)
S seg.prod(l,r) [l,r)の集約値を返す O(logN)
S seg.all_prod() 全区間の集約値を返す O(1)
int seg.max_right<f>(l) f(prod(l,r))を満たす最大rを返す O(logN)
int seg.min_left<f>(r) f(prod(l,r))を満たす最小lを返す O(logN)

SegMax seg(v) 区間最大値を管理 O(N)
SegMin seg(v) 区間最小値を管理 O(N)
SegSum seg(v) 区間和を管理 O(N)
SegXOR seg(v) 区間XORを管理 O(N)
SegOR seg(v)  区間ORを管理 O(N)
SegAND seg(v) 区間ANDを管理 O(N)
SegGCD seg(v) 区間GCDを管理 O(N)
SegMinMax seg(v) 区間最大値・最小値を同時管理 O(N)

基本の関数
SegSum seg(vl{1,2,3});                        // 構築 O(N)
seg.set(1,10);                                // 点代入 O(log N)
auto sum = seg.prod(0,3);                     // 14 O(log N)
auto value = seg.get(1);                      // 10 O(1)
*/
