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

namespace SegSecond_impl{
    struct S{ll first=0,second=0;int count=0;};
    template<bool Maximum>S op(S a,S b){
        S result;
        auto insert=[&](ll x){
            if(!result.count){result.first=x;result.count=1;return;}
            if(x==result.first||(result.count==2&&x==result.second))return;
            if(Maximum?x>result.first:x<result.first){
                result.second=result.first;result.first=x;result.count=2;
            }else if(result.count==1||(Maximum?x>result.second:x<result.second)){
                result.second=x;result.count=2;
            }
        };
        if(a.count)insert(a.first);
        if(a.count==2)insert(a.second);
        if(b.count)insert(b.first);
        if(b.count==2)insert(b.second);
        return result;
    }
    inline S e(){return {};}
    inline vector<S> cv(const vector<ll>&a){
        vector<S> result;result.reserve(a.size());
        for(ll x:a)result.push_back({x,0,1});
        return result;
    }
    inline int checked_n(int n){if(n<0||n>(1<<29))std::abort();return n;}
    template<bool Maximum>
    struct Tree:SegBase<SegSecond_impl::S,op<Maximum>,e>{
        using S=SegSecond_impl::S;
        using Base=SegBase<S,op<Maximum>,e>;
        using Base::Base;using Base::set;
        Tree()=default;
        explicit Tree(int n):Base(checked_n(n)){}
        explicit Tree(const vector<ll>&a):Base(cv(a)){}
        void set(int i,ll x){Base::set(i,{x,0,1});}
    };
}
using SegMaxSecond=SegSecond_impl::Tree<true>;
using SegMinSecond=SegSecond_impl::Tree<false>;

/*
細かい注意点
0-indexed、全区間は[l,r)。opは結合則、eは単位元を満たす。
max_right/min_leftの述語は副作用なし・単調・f(e())==true。
Arg型の同値は最左位置。MaxSubarrayは空部分列を認めない。
SegMinMax::S={mx,mn}、Arg::S={val,id}、MaxSubarray::S={sum,pref,suff,best}。
SegAffine::S={a,b}はf[r-1]∘...∘f[l]、SegBracket::S={sum,mn}。
Second型のS={first,second,count}。重複を除き、Maxは降順、Minは昇順に最大2個保持。
countは保持数0/1/2で、区間内の全種類数ではない。count>=1でfirst、count==2でsecondが有効。
Second型は番兵なしでllの全範囲に対応。空区間・未設定の葉はcount=0、1種類だけならcount=1。
Second型の(n)は未設定の葉をn個用意。全要素0にするならvector<ll>(n)を渡す。N<=2^29。
整数演算がlong longに収まること。codecfではatcoder/segtreeを追加で読み込む。

使える関数一覧
SegMaxSecond seg(v);                        // vector<ll>から異なる上位2つを構築 O(N)
SegMinSecond seg(v);                        // vector<ll>から異なる下位2つを構築 O(N)
SegMaxSecond seg; / SegMinSecond seg;        // 空の木 O(1)
SegMaxSecond seg(n); / SegMinSecond seg(n);  // n個の未設定の葉を構築 O(N)
Type seg(vector<Type::S> v);                // Second型: count=0/1の葉配列から構築 O(N)
seg.set(i,x);                              // Second型: llの値を1個保持する葉に置換 O(log N)
seg.set(i,s);                              // Second型: Sの葉情報に置換、countは0/1 O(log N)
seg.get(i);                                // Second型のS: 葉の情報 O(1)
seg.prod(l,r);                             // Second型のS: {first,second,count} O(log N)
seg.all_prod();                            // Second型のS: 全体の{first,second,count} O(1)
seg.max_right(l,g); / seg.min_left(r,g);    // Second型のSを受け取る述語で境界探索 O(log N)
seg.max_right<g>(l); / seg.min_left<g>(r);  // 同上、関数ポインタ版 O(log N)

SegArgMax seg(v) 区間最大値とその最左位置を管理 O(N)
pair相当 seg.prod(l,r) {最大値,その最左index}を返す O(log N)
void seg.set(i,x) A[i]=xに更新 O(log N)
SegArgMin seg(v) 区間最小値とその最左位置を管理 O(N)
pair相当 seg.prod(l,r) {最小値,その最左index}を返す O(log N)
void seg.set(i,x) A[i]=xに更新 O(log N)

SegMaxSubarray seg(v) 動的な最大部分配列和を管理、空部分列は不可 O(N)
S seg.prod(l,r) {sum,pref,suff,best}を返す O(log N)
void seg.set(i,x) A[i]=xに更新 O(log N)

SegAffine seg(v) 一次関数f(x)=ax+bの列を合成して管理 O(N)
S seg.prod(l,r) f[r-1]∘...∘f[l]を{a,b}で返す O(log N)
mint seg.prod(l,r).eval(x) 区間の合成関数をxに適用 O(log N)

SegBracket seg(s) 括弧列を構築 O(N)
S seg.prod(l,r) [l,r)の{総和,最小prefix和}を返す O(log N)
bool seg.valid(l,r) [l,r)が正しい括弧列か判定 O(log N)
void seg.set(i,c) i番目を'('または')'に更新 O(log N)

void seg.set(i,x) i番目をxに更新 O(log N)
S seg.get(i) i番目の値を返す O(1)
S seg.prod(l,r) [l,r)の集約値を返す O(log N)
S seg.all_prod() 全区間の集約値を返す O(1)
int seg.max_right<f>(l) f(prod(l,r))を満たす最大rを返す O(log N)
int seg.min_left<f>(r) f(prod(l,r))を満たす最小lを返す O(log N)

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
SegMaxSecond top(vl{5,5,3,1});
auto high=top.prod(0,4);                      // {first=5,second=3,count=2}
top.set(2,5);
auto one=top.prod(0,3);                       // {first=5,count=1}。secondは使わない
SegMinSecond bottom(vl{5,5,3,1});
auto low=bottom.prod(0,4);                    // {first=1,second=3,count=2}
*/
