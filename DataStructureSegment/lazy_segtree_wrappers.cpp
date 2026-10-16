namespace lazy_wrapper_detail {
    inline int checked_size(std::size_t n) {
        if(n>(1u<<29)) std::abort();
        return static_cast<int>(n);
    }
    inline int checked_n(int n) {
        if(n<0) std::abort();
        return checked_size(static_cast<std::size_t>(n));
    }
    inline std::vector<long long> zeros(int n) {
        return std::vector<long long>(static_cast<std::size_t>(checked_n(n)),0);
    }
}

template<class S,S(*op)(S,S),S(*e)(),class F,S(*mp)(F,S),F(*cp)(F,F),F(*id)()>
class LazySegBase : private atcoder::lazy_segtree<S,op,e,F,mp,cp,id> {
    using Tree=atcoder::lazy_segtree<S,op,e,F,mp,cp,id>;
    int n_=0;
    static const std::vector<S>& checked(const std::vector<S>& a) {
        lazy_wrapper_detail::checked_size(a.size()); return a;
    }
    void check_point(int p) const {
        if(p<0 || p>=n_) std::abort();
    }
    void check_range(int l,int r) const {
        if(l<0 || l>r || r>n_) std::abort();
    }
public:
    LazySegBase()=default;
    explicit LazySegBase(int n):Tree(lazy_wrapper_detail::checked_n(n)),n_(n){}
    explicit LazySegBase(const std::vector<S>& a):Tree(checked(a)),n_(static_cast<int>(a.size())){}
    LazySegBase(const LazySegBase&)=default;
    LazySegBase& operator=(const LazySegBase&)=default;
    LazySegBase(LazySegBase&& other) noexcept
        :Tree(std::move(static_cast<Tree&>(other))),n_(std::exchange(other.n_,0)){}
    LazySegBase& operator=(LazySegBase&& other) noexcept {
        if(this!=&other){Tree::operator=(std::move(static_cast<Tree&>(other)));n_=std::exchange(other.n_,0);}
        return *this;
    }
    int size() const {return n_;}
    bool empty() const {return n_==0;}
    void set(int p,S x){check_point(p);Tree::set(p,std::move(x));}
    S get(int p){check_point(p);return Tree::get(p);}
    S prod(int l,int r){check_range(l,r);return l==r?e():Tree::prod(l,r);}
    S all_prod() const {return n_?const_cast<LazySegBase*>(this)->Tree::all_prod():e();}
    void apply(int p,F f){check_point(p);Tree::apply(p,std::move(f));}
    void apply(int l,int r,F f){check_range(l,r);if(l<r)Tree::apply(l,r,std::move(f));}
    template<bool(*g)(S)> int max_right(int l){return max_right(l,g);}
    template<class G> int max_right(int l,G g){
        check_range(l,l);
        if(!g(e()))std::abort();
        return l==n_?n_:Tree::max_right(l,std::move(g));
    }
    template<bool(*g)(S)> int min_left(int r){return min_left(r,g);}
    template<class G> int min_left(int r,G g){
        check_range(r,r);
        if(!g(e()))std::abort();
        return r==0?0:Tree::min_left(r,std::move(g));
    }
};

struct LazyAssignF{long long x=0;bool set=false;};
constexpr long long LSEG_INF=(1LL<<62);

namespace LazyAddMin_impl{
    long long op(long long a,long long b){return std::min(a,b);}
    long long e(){return LSEG_INF;}
    long long mp(long long f,long long x){return x==e()?x:x+f;}
    long long cp(long long f,long long g){return f+g;}
    long long id(){return 0;}
}
struct LazyAddMin:LazySegBase<long long,LazyAddMin_impl::op,LazyAddMin_impl::e,long long,LazyAddMin_impl::mp,LazyAddMin_impl::cp,LazyAddMin_impl::id>{
    using B=LazySegBase<long long,LazyAddMin_impl::op,LazyAddMin_impl::e,long long,LazyAddMin_impl::mp,LazyAddMin_impl::cp,LazyAddMin_impl::id>;
    using B::B;
    LazyAddMin(int n):B(lazy_wrapper_detail::zeros(n)){}
    LazyAddMin()=default;
    void add(int l,int r,long long x){B::apply(l,r,x);}
    long long min(int l,int r){return B::prod(l,r);}
};

namespace LazyAddMax_impl{
    long long op(long long a,long long b){return std::max(a,b);}
    long long e(){return -LSEG_INF;}
    long long mp(long long f,long long x){return x==e()?x:x+f;}
    long long cp(long long f,long long g){return f+g;}
    long long id(){return 0;}
}
struct LazyAddMax:LazySegBase<long long,LazyAddMax_impl::op,LazyAddMax_impl::e,long long,LazyAddMax_impl::mp,LazyAddMax_impl::cp,LazyAddMax_impl::id>{
    using B=LazySegBase<long long,LazyAddMax_impl::op,LazyAddMax_impl::e,long long,LazyAddMax_impl::mp,LazyAddMax_impl::cp,LazyAddMax_impl::id>;
    using B::B;
    LazyAddMax(int n):B(lazy_wrapper_detail::zeros(n)){}
    LazyAddMax()=default;
    void add(int l,int r,long long x){B::apply(l,r,x);}
    long long max(int l,int r){return B::prod(l,r);}
};

namespace LazySum_impl{
    struct S{long long val,size;};
    S op(S a,S b){return {a.val+b.val,a.size+b.size};}
    S e(){return {0,0};}
    std::vector<S> cv(const std::vector<long long>&a){std::vector<S>v;for(long long x:a)v.push_back({x,1});return v;}
}

namespace LazyAddSum_impl{
    using S=LazySum_impl::S;
    S mp(long long f,S x){return {x.val+f*x.size,x.size};}
    long long cp(long long f,long long g){return f+g;}
    long long id(){return 0;}
}
struct LazyAddSum:LazySegBase<LazySum_impl::S,LazySum_impl::op,LazySum_impl::e,long long,LazyAddSum_impl::mp,LazyAddSum_impl::cp,LazyAddSum_impl::id>{
    using S=LazySum_impl::S;
    using B=LazySegBase<S,LazySum_impl::op,LazySum_impl::e,long long,LazyAddSum_impl::mp,LazyAddSum_impl::cp,LazyAddSum_impl::id>;
    using B::B;using B::set;
    LazyAddSum(int n):B(LazySum_impl::cv(lazy_wrapper_detail::zeros(n))){}
    LazyAddSum(const std::vector<long long>&a):B(LazySum_impl::cv(a)){}
    void set(int i,long long x){B::set(i,{x,1});}
    long long sum(int l,int r){return B::prod(l,r).val;}
    LazyAddSum()=default;
    using B::apply;
    void add(int l,int r,long long x){B::apply(l,r,x);}
};

namespace LazyAssignMin_impl{
    long long op(long long a,long long b){return std::min(a,b);}
    long long e(){return LSEG_INF;}
    long long mp(LazyAssignF f,long long x){return x==e()?x:(f.set?f.x:x);}
    LazyAssignF cp(LazyAssignF f,LazyAssignF g){return f.set?f:g;}
    LazyAssignF id(){return {};}
}
struct LazyAssignMin:LazySegBase<long long,LazyAssignMin_impl::op,LazyAssignMin_impl::e,LazyAssignF,LazyAssignMin_impl::mp,LazyAssignMin_impl::cp,LazyAssignMin_impl::id>{
    using B=LazySegBase<long long,LazyAssignMin_impl::op,LazyAssignMin_impl::e,LazyAssignF,LazyAssignMin_impl::mp,LazyAssignMin_impl::cp,LazyAssignMin_impl::id>;
    using B::B;using B::apply;
    LazyAssignMin(int n):B(lazy_wrapper_detail::zeros(n)){}
    void apply(int l,int r,long long x){B::apply(l,r,{x,true});}
    LazyAssignMin()=default;
    void apply(int i,long long x){B::apply(i,LazyAssignF{x,true});}
    void assign(int l,int r,long long x){apply(l,r,x);}
    long long min(int l,int r){return B::prod(l,r);}
};

namespace LazyAssignMax_impl{
    long long op(long long a,long long b){return std::max(a,b);}
    long long e(){return -LSEG_INF;}
    long long mp(LazyAssignF f,long long x){return x==e()?x:(f.set?f.x:x);}
    LazyAssignF cp(LazyAssignF f,LazyAssignF g){return f.set?f:g;}
    LazyAssignF id(){return {};}
}
struct LazyAssignMax:LazySegBase<long long,LazyAssignMax_impl::op,LazyAssignMax_impl::e,LazyAssignF,LazyAssignMax_impl::mp,LazyAssignMax_impl::cp,LazyAssignMax_impl::id>{
    using B=LazySegBase<long long,LazyAssignMax_impl::op,LazyAssignMax_impl::e,LazyAssignF,LazyAssignMax_impl::mp,LazyAssignMax_impl::cp,LazyAssignMax_impl::id>;
    using B::B;using B::apply;
    LazyAssignMax(int n):B(lazy_wrapper_detail::zeros(n)){}
    void apply(int l,int r,long long x){B::apply(l,r,{x,true});}
    LazyAssignMax()=default;
    void apply(int i,long long x){B::apply(i,LazyAssignF{x,true});}
    void assign(int l,int r,long long x){apply(l,r,x);}
    long long max(int l,int r){return B::prod(l,r);}
};

namespace LazyAssignSum_impl{
    using S=LazySum_impl::S;
    S mp(LazyAssignF f,S x){return f.set?S{f.x*x.size,x.size}:x;}
    LazyAssignF cp(LazyAssignF f,LazyAssignF g){return f.set?f:g;}
    LazyAssignF id(){return {};}
}
struct LazyAssignSum:LazySegBase<LazySum_impl::S,LazySum_impl::op,LazySum_impl::e,LazyAssignF,LazyAssignSum_impl::mp,LazyAssignSum_impl::cp,LazyAssignSum_impl::id>{
    using S=LazySum_impl::S;
    using B=LazySegBase<S,LazySum_impl::op,LazySum_impl::e,LazyAssignF,LazyAssignSum_impl::mp,LazyAssignSum_impl::cp,LazyAssignSum_impl::id>;
    using B::B;using B::apply;using B::set;
    LazyAssignSum(int n):B(LazySum_impl::cv(lazy_wrapper_detail::zeros(n))){}
    LazyAssignSum(const std::vector<long long>&a):B(LazySum_impl::cv(a)){}
    void apply(int l,int r,long long x){B::apply(l,r,{x,true});}
    void set(int i,long long x){B::set(i,{x,1});}
    long long sum(int l,int r){return B::prod(l,r).val;}
    LazyAssignSum()=default;
    void apply(int i,long long x){B::apply(i,LazyAssignF{x,true});}
    void assign(int l,int r,long long x){apply(l,r,x);}
};

namespace LazyMinMax_impl{
    struct S{long long mx,mn;};
    S op(S a,S b){return {std::max(a.mx,b.mx),std::min(a.mn,b.mn)};}
    S e(){return {-LSEG_INF,LSEG_INF};}
    std::vector<S> cv(const std::vector<long long>&a){std::vector<S>v;for(long long x:a)v.push_back({x,x});return v;}
}

namespace LazyAddMinMax_impl{
    using S=LazyMinMax_impl::S;
    S mp(long long f,S x){return x.mn==LSEG_INF?x:S{x.mx+f,x.mn+f};}
    long long cp(long long f,long long g){return f+g;}
    long long id(){return 0;}
}
struct LazyAddMinMax:LazySegBase<LazyMinMax_impl::S,LazyMinMax_impl::op,LazyMinMax_impl::e,long long,LazyAddMinMax_impl::mp,LazyAddMinMax_impl::cp,LazyAddMinMax_impl::id>{
    using S=LazyMinMax_impl::S;
    using B=LazySegBase<S,LazyMinMax_impl::op,LazyMinMax_impl::e,long long,LazyAddMinMax_impl::mp,LazyAddMinMax_impl::cp,LazyAddMinMax_impl::id>;
    using B::B;using B::set;
    LazyAddMinMax(int n):B(LazyMinMax_impl::cv(lazy_wrapper_detail::zeros(n))){}
    LazyAddMinMax(const std::vector<long long>&a):B(LazyMinMax_impl::cv(a)){}
    void set(int i,long long x){B::set(i,{x,x});}
    LazyAddMinMax()=default;
    using B::apply;
    void add(int l,int r,long long x){B::apply(l,r,x);}
    long long min(int l,int r){return B::prod(l,r).mn;}
    long long max(int l,int r){return B::prod(l,r).mx;}
};

namespace LazyAssignMinMax_impl{
    using S=LazyMinMax_impl::S;
    S mp(LazyAssignF f,S x){return x.mn==LSEG_INF?x:(f.set?S{f.x,f.x}:x);}
    LazyAssignF cp(LazyAssignF f,LazyAssignF g){return f.set?f:g;}
    LazyAssignF id(){return {};}
}
struct LazyAssignMinMax:LazySegBase<LazyMinMax_impl::S,LazyMinMax_impl::op,LazyMinMax_impl::e,LazyAssignF,LazyAssignMinMax_impl::mp,LazyAssignMinMax_impl::cp,LazyAssignMinMax_impl::id>{
    using S=LazyMinMax_impl::S;
    using B=LazySegBase<S,LazyMinMax_impl::op,LazyMinMax_impl::e,LazyAssignF,LazyAssignMinMax_impl::mp,LazyAssignMinMax_impl::cp,LazyAssignMinMax_impl::id>;
    using B::B;using B::apply;using B::set;
    LazyAssignMinMax(const std::vector<long long>&a):B(LazyMinMax_impl::cv(a)){}
    void apply(int l,int r,long long x){B::apply(l,r,{x,true});}
    void set(int i,long long x){B::set(i,{x,x});}
    LazyAssignMinMax()=default;
    explicit LazyAssignMinMax(int n):LazyAssignMinMax(lazy_wrapper_detail::zeros(n)){}
    void apply(int i,long long x){B::apply(i,LazyAssignF{x,true});}
    void assign(int l,int r,long long x){apply(l,r,x);}
    long long min(int l,int r){return B::prod(l,r).mn;}
    long long max(int l,int r){return B::prod(l,r).mx;}
};

namespace LazyAddArgMax_impl{
    struct S{long long val,id;};
    S op(S a,S b){return a.val!=b.val?(a.val>b.val?a:b):(a.id<b.id?a:b);}
    S e(){return {-LSEG_INF,LSEG_INF};}
    S mp(long long f,S x){return x.id==LSEG_INF?x:S{x.val+f,x.id};}
    long long cp(long long f,long long g){return f+g;}
    long long id(){return 0;}
}
struct LazyAddArgMax:LazySegBase<LazyAddArgMax_impl::S,LazyAddArgMax_impl::op,LazyAddArgMax_impl::e,long long,LazyAddArgMax_impl::mp,LazyAddArgMax_impl::cp,LazyAddArgMax_impl::id>{
    using S=LazyAddArgMax_impl::S;
    using B=LazySegBase<S,LazyAddArgMax_impl::op,LazyAddArgMax_impl::e,long long,LazyAddArgMax_impl::mp,LazyAddArgMax_impl::cp,LazyAddArgMax_impl::id>;
    static std::vector<S> cv(const std::vector<long long>&a){std::vector<S>v;for(std::size_t i=0;i<a.size();++i)v.push_back({a[i],static_cast<long long>(i)});return v;}
    LazyAddArgMax(const std::vector<long long>&a):B(cv(a)){}
    void set(int i,long long x){B::set(i,{x,i});}
    LazyAddArgMax()=default;
    explicit LazyAddArgMax(int n):LazyAddArgMax(lazy_wrapper_detail::zeros(n)){}
    using B::apply;
    using B::set;
    void add(int l,int r,long long x){B::apply(l,r,x);}
};

namespace LazyAddArgMin_impl{
    struct S{long long val,id;};
    S op(S a,S b){return a.val!=b.val?(a.val<b.val?a:b):(a.id<b.id?a:b);}
    S e(){return {LSEG_INF,LSEG_INF};}
    S mp(long long f,S x){return x.id==LSEG_INF?x:S{x.val+f,x.id};}
    long long cp(long long f,long long g){return f+g;}
    long long id(){return 0;}
}
struct LazyAddArgMin:LazySegBase<LazyAddArgMin_impl::S,LazyAddArgMin_impl::op,LazyAddArgMin_impl::e,long long,LazyAddArgMin_impl::mp,LazyAddArgMin_impl::cp,LazyAddArgMin_impl::id>{
    using S=LazyAddArgMin_impl::S;
    using B=LazySegBase<S,LazyAddArgMin_impl::op,LazyAddArgMin_impl::e,long long,LazyAddArgMin_impl::mp,LazyAddArgMin_impl::cp,LazyAddArgMin_impl::id>;
    static std::vector<S> cv(const std::vector<long long>&a){std::vector<S>v;for(std::size_t i=0;i<a.size();++i)v.push_back({a[i],static_cast<long long>(i)});return v;}
    LazyAddArgMin(const std::vector<long long>&a):B(cv(a)){}
    void set(int i,long long x){B::set(i,{x,i});}
    LazyAddArgMin()=default;
    explicit LazyAddArgMin(int n):LazyAddArgMin(lazy_wrapper_detail::zeros(n)){}
    using B::apply;
    using B::set;
    void add(int l,int r,long long x){B::apply(l,r,x);}
};

namespace LazyChminMax_impl{
    long long op(long long a,long long b){return std::max(a,b);}
    long long e(){return -LSEG_INF;}
    long long mp(long long f,long long x){return std::min(f,x);}
    long long cp(long long f,long long g){return std::min(f,g);}
    long long id(){return LSEG_INF;}
}
struct LazyChminMax:LazySegBase<long long,LazyChminMax_impl::op,LazyChminMax_impl::e,long long,LazyChminMax_impl::mp,LazyChminMax_impl::cp,LazyChminMax_impl::id>{
    using B=LazySegBase<long long,LazyChminMax_impl::op,LazyChminMax_impl::e,long long,LazyChminMax_impl::mp,LazyChminMax_impl::cp,LazyChminMax_impl::id>;
    using B::B;
    LazyChminMax(int n):B(lazy_wrapper_detail::zeros(n)){}
    LazyChminMax()=default;
    long long max(int l,int r){return B::prod(l,r);}
    void chmin(int l,int r,long long x){B::apply(l,r,x);}
};

namespace LazyChmaxMin_impl{
    long long op(long long a,long long b){return std::min(a,b);}
    long long e(){return LSEG_INF;}
    long long mp(long long f,long long x){return std::max(f,x);}
    long long cp(long long f,long long g){return std::max(f,g);}
    long long id(){return -LSEG_INF;}
}
struct LazyChmaxMin:LazySegBase<long long,LazyChmaxMin_impl::op,LazyChmaxMin_impl::e,long long,LazyChmaxMin_impl::mp,LazyChmaxMin_impl::cp,LazyChmaxMin_impl::id>{
    using B=LazySegBase<long long,LazyChmaxMin_impl::op,LazyChmaxMin_impl::e,long long,LazyChmaxMin_impl::mp,LazyChmaxMin_impl::cp,LazyChmaxMin_impl::id>;
    using B::B;
    LazyChmaxMin(int n):B(lazy_wrapper_detail::zeros(n)){}
    LazyChmaxMin()=default;
    long long min(int l,int r){return B::prod(l,r);}
    void chmax(int l,int r,long long x){B::apply(l,r,x);}
};

namespace LazyAffineStats_impl{
    struct S{long long sum,mx,mn,size;};
    struct F{long long a=1,b=0;};

    S op(S x,S y){
        return {x.sum+y.sum,std::max(x.mx,y.mx),std::min(x.mn,y.mn),x.size+y.size};
    }

    S e(){return {0,-LSEG_INF,LSEG_INF,0};}

    S mp(F f,S x){
        if(!x.size)return x;
        long long sum=f.a*x.sum+f.b*x.size;
        if(f.a>=0)return {sum,f.a*x.mx+f.b,f.a*x.mn+f.b,x.size};
        return {sum,f.a*x.mn+f.b,f.a*x.mx+f.b,x.size};
    }

    F cp(F f,F g){return {f.a*g.a,f.a*g.b+f.b};}
    F id(){return {};}

    std::vector<S> cv(const std::vector<long long>&a){
        std::vector<S>v;
        for(long long x:a)v.push_back({x,x,x,1});
        return v;
    }
}

struct LazyAffineStats:LazySegBase<LazyAffineStats_impl::S,LazyAffineStats_impl::op,LazyAffineStats_impl::e,LazyAffineStats_impl::F,LazyAffineStats_impl::mp,LazyAffineStats_impl::cp,LazyAffineStats_impl::id>{
    using S=LazyAffineStats_impl::S;
    using F=LazyAffineStats_impl::F;
    using B=LazySegBase<S,LazyAffineStats_impl::op,LazyAffineStats_impl::e,F,LazyAffineStats_impl::mp,LazyAffineStats_impl::cp,LazyAffineStats_impl::id>;

    LazyAffineStats(int n):B(LazyAffineStats_impl::cv(lazy_wrapper_detail::zeros(n))){}
    LazyAffineStats(const std::vector<long long>&a):B(LazyAffineStats_impl::cv(a)){}

    void affine(int l,int r,long long a,long long b){B::apply(l,r,{a,b});}
    void add(int l,int r,long long x){affine(l,r,1,x);}
    void mul(int l,int r,long long x){affine(l,r,x,0);}
    void assign(int l,int r,long long x){affine(l,r,0,x);}
    void set(int i,long long x){B::set(i,{x,x,x,1});}
    long long sum(int l,int r){return B::prod(l,r).sum;}
    long long max(int l,int r){return B::prod(l,r).mx;}
    long long min(int l,int r){return B::prod(l,r).mn;}
    LazyAffineStats()=default;
    using B::apply;

    using B::set;
};

template<class Mint>
struct LazyAffineSumOps {
    struct S{Mint val;long long size;};
    struct F{Mint a=1,b=0;};
    static S op(S x,S y){return {x.val+y.val,x.size+y.size};}
    static S e(){return {0,0};}
    static S mp(F f,S x){return {f.a*x.val+f.b*x.size,x.size};}
    static F cp(F f,F g){return {f.a*g.a,f.a*g.b+f.b};}
    static F id(){return {};}
    template<class T> static std::vector<S> cv(const std::vector<T>& a){
        std::vector<S> result;result.reserve(a.size());
        for(const auto& x:a)result.push_back({Mint(x),1});
        return result;
    }
};
template<class Mint>
struct LazyAffineSumT:LazySegBase<typename LazyAffineSumOps<Mint>::S,LazyAffineSumOps<Mint>::op,LazyAffineSumOps<Mint>::e,typename LazyAffineSumOps<Mint>::F,LazyAffineSumOps<Mint>::mp,LazyAffineSumOps<Mint>::cp,LazyAffineSumOps<Mint>::id>{
    using Ops=LazyAffineSumOps<Mint>;
    using S=typename Ops::S;
    using F=typename Ops::F;
    using B=LazySegBase<S,Ops::op,Ops::e,F,Ops::mp,Ops::cp,Ops::id>;
    using B::apply;
    using B::set;
    LazyAffineSumT()=default;
    explicit LazyAffineSumT(int n):B(Ops::cv(lazy_wrapper_detail::zeros(n))){}
    template<class T> explicit LazyAffineSumT(const std::vector<T>& a):B(Ops::cv(a)){}
    void affine(int l,int r,Mint a,Mint b){B::apply(l,r,{a,b});}
    void add(int l,int r,Mint x){affine(l,r,1,x);}
    void mul(int l,int r,Mint x){affine(l,r,x,0);}
    void assign(int l,int r,Mint x){affine(l,r,0,x);}
    void set(int i,Mint x){B::set(i,{x,1});}
    Mint sum(int l,int r){return B::prod(l,r).val;}
};
using LazyAffineSum=LazyAffineSumT<atcoder::modint998244353>;
namespace LazyAffineSum_impl {
    using Ops=LazyAffineSumOps<atcoder::modint998244353>;
    using S=Ops::S;using F=Ops::F;
    inline S op(S a,S b){return Ops::op(a,b);}
    inline S e(){return Ops::e();}
    inline S mp(F f,S x){return Ops::mp(f,x);}
    inline F cp(F f,F g){return Ops::cp(f,g);}
    inline F id(){return Ops::id();}
}

namespace LazyFlipSum_impl{
    struct S{long long one,size;};
    S op(S a,S b){return {a.one+b.one,a.size+b.size};}
    S e(){return {0,0};}
    S mp(bool f,S x){return f?S{x.size-x.one,x.size}:x;}
    bool cp(bool f,bool g){return f^g;}
    bool id(){return false;}
}

struct LazyFlipSum:LazySegBase<LazyFlipSum_impl::S,LazyFlipSum_impl::op,LazyFlipSum_impl::e,bool,LazyFlipSum_impl::mp,LazyFlipSum_impl::cp,LazyFlipSum_impl::id>{
    using S=LazyFlipSum_impl::S;
    using B=LazySegBase<S,LazyFlipSum_impl::op,LazyFlipSum_impl::e,bool,LazyFlipSum_impl::mp,LazyFlipSum_impl::cp,LazyFlipSum_impl::id>;

    static std::vector<S> cv(const std::vector<long long>&a){std::vector<S>v;for(long long x:a)v.push_back({!!x,1});return v;}
    LazyFlipSum(int n):B(cv(lazy_wrapper_detail::zeros(n))){}
    LazyFlipSum(const std::vector<long long>&a):B(cv(a)){}

    void flip(int l,int r){B::apply(l,r,true);}
    void set(int i,long long x){B::set(i,{!!x,1});}
    long long ones(int l,int r){return B::prod(l,r).one;}
    LazyFlipSum()=default;
    using B::apply;

    using B::set;
};

namespace LazyFlipInv_impl{
    struct S{long long zero,one,inv;};

    S op(S a,S b){
        return {a.zero+b.zero,a.one+b.one,a.inv+b.inv+a.one*b.zero};
    }

    S e(){return {0,0,0};}

    S mp(bool f,S x){
        if(!f)return x;
        return {x.one,x.zero,x.zero*x.one-x.inv};
    }

    bool cp(bool f,bool g){return f^g;}
    bool id(){return false;}
}

struct LazyFlipInv:LazySegBase<LazyFlipInv_impl::S,LazyFlipInv_impl::op,LazyFlipInv_impl::e,bool,LazyFlipInv_impl::mp,LazyFlipInv_impl::cp,LazyFlipInv_impl::id>{
    using S=LazyFlipInv_impl::S;
    using B=LazySegBase<S,LazyFlipInv_impl::op,LazyFlipInv_impl::e,bool,LazyFlipInv_impl::mp,LazyFlipInv_impl::cp,LazyFlipInv_impl::id>;

    static std::vector<S> cv(const std::vector<long long>&a){
        std::vector<S>v;
        for(long long x:a)v.push_back(x?S{0,1,0}:S{1,0,0});
        return v;
    }

    LazyFlipInv(int n):B(cv(lazy_wrapper_detail::zeros(n))){}
    LazyFlipInv(const std::vector<long long>&a):B(cv(a)){}

    void flip(int l,int r){B::apply(l,r,true);}
    void set(int i,long long x){B::set(i,x?S{0,1,0}:S{1,0,0});}
    long long inversions(int l,int r){return B::prod(l,r).inv;}
    LazyFlipInv()=default;
    using B::apply;

    using B::set;
};

namespace LazyAPAddSum_impl{
    struct S{long long sum,size,idxsum;};
    struct F{long long a=0,b=0;};

    S op(S x,S y){return {x.sum+y.sum,x.size+y.size,x.idxsum+y.idxsum};}
    S e(){return {0,0,0};}
    S mp(F f,S x){return {x.sum+f.a*x.idxsum+f.b*x.size,x.size,x.idxsum};}
    F cp(F f,F g){return {f.a+g.a,f.b+g.b};}
    F id(){return {};}
}

struct LazyAPAddSum:LazySegBase<LazyAPAddSum_impl::S,LazyAPAddSum_impl::op,LazyAPAddSum_impl::e,LazyAPAddSum_impl::F,LazyAPAddSum_impl::mp,LazyAPAddSum_impl::cp,LazyAPAddSum_impl::id>{
    using S=LazyAPAddSum_impl::S;
    using F=LazyAPAddSum_impl::F;
    using B=LazySegBase<S,LazyAPAddSum_impl::op,LazyAPAddSum_impl::e,F,LazyAPAddSum_impl::mp,LazyAPAddSum_impl::cp,LazyAPAddSum_impl::id>;

    static std::vector<S> cv(const std::vector<long long>&a){
        std::vector<S>v;
        for(std::size_t i=0;i<a.size();++i)v.push_back({a[i],1,static_cast<long long>(i)});
        return v;
    }

    LazyAPAddSum(int n):B(cv(lazy_wrapper_detail::zeros(n))){}
    LazyAPAddSum(const std::vector<long long>&a):B(cv(a)){}

    void add_linear(int l,int r,long long a,long long b){B::apply(l,r,{a,b});}
    void add_ap(int l,int r,long long a,long long step){add_linear(l,r,step,a-step*l);}
    void set(int i,long long x){B::set(i,{x,1,i});}
    long long sum(int l,int r){return B::prod(l,r).sum;}
    LazyAPAddSum()=default;
    using B::apply;

    using B::set;
};

/*
細かい注意点
0-indexed、区間は[l,r)。0<=i<N、0<=l<=r<=N。N<=2^29。
Sは集約型、Fは更新型。get/prodは整数値そのものではなくSを返す型もある。
区間和・遅延タグ・中間演算はlong longに収まること。
最小・最大・位置付き型の実要素は-LSEG_INFより大きくLSEG_INFより小さく保つ。
空区間のprodは単位元、applyは何もしない。minはLSEG_INF、maxは-LSEG_INF、和は0。
Arg型の空区間のidはLSEG_INF、Sum/Stats/AP型のsizeは0。
max_right/min_leftの述語は副作用なし・単調・g(e())==true。
写像の合成cp(f,g)はgの後にf。代入写像はLazyAssignF{x,true}、{}は何もしない。
具体型の(n)は0初期化、汎用LazySegBase(n)は単位元で初期化する。
Flip型は初期値とset値を0/非0から0/1へ正規化する。
範囲外や負の長さ、不正な述語など検出した前提違反はメッセージなしで停止。
保持メモリO(N)。codecfではatcoder/lazysegtreeを追加で読み込む。

使える関数一覧
// 型の選択（Type seg(n) / Type seg(v)の構築はO(N)、seg(n)は0初期化）
LazyAddMin seg(v);                             // 区間加算 + 最小値。S=long long、F=加算値
LazyAddMax seg(v);                             // 区間加算 + 最大値。S=long long、F=加算値
LazyAddSum seg(v);                             // 区間加算 + 和。S={val,size}、F=加算値
LazyAssignMin seg(v);                          // 区間代入 + 最小値。S=long long、F=LazyAssignF
LazyAssignMax seg(v);                          // 区間代入 + 最大値。S=long long、F=LazyAssignF
LazyAssignSum seg(v);                          // 区間代入 + 和。S={val,size}、F=LazyAssignF
LazyAddMinMax seg(v);                          // 区間加算 + 最大最小。S={mx,mn}、F=加算値
LazyAssignMinMax seg(v);                       // 区間代入 + 最大最小。S={mx,mn}、F=LazyAssignF
LazyAddArgMin seg(v);                          // 区間加算 + 最小値と最左位置。S={val,id}、F=加算値
LazyAddArgMax seg(v);                          // 区間加算 + 最大値と最左位置。S={val,id}、F=加算値
LazyChminMax seg(v);                           // 区間chmin + 最大値。S=long long、F=上限値
LazyChmaxMin seg(v);                           // 区間chmax + 最小値。S=long long、F=下限値
LazyAffineStats seg(v);                        // 整数一次変換 + 和・最大最小。S={sum,mx,mn,size}、F={a,b}
LazyAffineSum seg(v);                          // 法998244353の一次変換 + 和。S={val,size}、F={a,b}
LazyAffineSumT<Mint> seg(v);                   // 指定したACL modint型の一次変換 + 和。S={val,size}、F={a,b}
LazyFlipSum seg(v);                            // 0/1反転 + 1の個数。S={one,size}、F=bool
LazyFlipInv seg(v);                            // 0/1反転 + 転倒数。S={zero,one,inv}、F=bool
LazyAPAddSum seg(v);                           // 等差数列加算 + 和。S={sum,size,idxsum}、F={a,b}

// 構築・コピー・ムーブ（Typeは上記のクラス。otherも同じ型）
Type seg;                                      // 返り値なし: 長さ0の木を構築 O(1)
Type seg(n);                                   // 返り値なし: 長さn、全要素0で構築 O(N)
Type seg(v);                                   // 返り値なし: vから構築 O(N)
Type seg(other);                               // 返り値なし: 独立なコピーを構築 O(N_other)
Type seg(std::move(other));                    // 返り値なし: 木を移し、otherを長さ0にする O(1)
seg = other;                                   // Type&: segへの参照。独立なコピーで上書き O(N+N_other)
seg = std::move(other);                        // Type&: segへの参照。木を移し、otherを長さ0にする O(N+1)

// 全型共通API（SとFの内容は上表。Sを直接setする場合は葉のメタデータも正しく設定）
seg.size();                                    // int: 配列長Nを返す O(1)
seg.empty();                                   // bool: 配列長0ならtrue O(1)
seg.set(i,s);                                  // void: 点iの集約情報をS=sに変更 O(log(N+1))
seg.get(i);                                    // S: 点iの情報を返す（必ずしも整数そのものではない） O(log(N+1))
seg.prod(l,r);                                 // S: [l,r)の集約情報を返す O(log(N+1))
seg.all_prod();                                // S: 配列全体の集約情報を返す O(1)
seg.apply(i,f);                                // void: 点iに更新F=fを作用 O(log(N+1))
seg.apply(l,r,f);                              // void: [l,r)に更新F=fを作用 O(log(N+1))
seg.max_right(l,g);                            // int: g(prod(l,r))を保つ最大rを返す O(log(N+1))
seg.min_left(r,g);                             // int: g(prod(l,r))を保つ最小lを返す O(log(N+1))
seg.max_right<g>(l);                           // int: 関数ポインタ版のmax_right O(log(N+1))
seg.min_left<g>(r);                            // int: 関数ポインタ版のmin_left O(log(N+1))

// 値だけを渡すset（集約型Sのメタデータを自動設定）
seg.set(i,x);                                  // void: 点iを値xに変更 O(log(N+1))
// 全ての具体型で利用可能。法付き型はMint、その他はlong longを渡す。

// 加算・代入型の便利API（型が対応する操作だけを利用）
seg.add(l,r,x);                                // void: Add系・Affine系の[l,r)にx加算 O(log(N+1))
seg.assign(l,r,x);                             // void: Assign系・Affine系の[l,r)をxに代入 O(log(N+1))
seg.apply(i,x);                                // void: Assign系の点iを値xに代入 O(log(N+1))
seg.apply(l,r,x);                              // void: Assign系の[l,r)を値xに代入 O(log(N+1))
// 生の代入写像はLazyAssignF{x,true}。LazyAssignF{}は何もしない写像。
seg.chmin(l,r,x);                              // void: LazyChminMaxでA[i]=min(A[i],x) O(log(N+1))
seg.chmax(l,r,x);                              // void: LazyChmaxMinでA[i]=max(A[i],x) O(log(N+1))
seg.min(l,r);                                  // long long: Min/MinMax/Stats型の区間最小値 O(log(N+1))
seg.max(l,r);                                  // long long: Max/MinMax/Stats型の区間最大値 O(log(N+1))
seg.sum(l,r);                                  // long longまたはMint: AddSum/AssignSum/Affine/Stats/AP型の区間和 O(log(N+1))

// 一次変換（LazyAffineStats / LazyAffineSum / LazyAffineSumT<Mint>）
seg.affine(l,r,a,b);                           // void: A[i]=a*A[i]+bを適用 O(log(N+1))
seg.mul(l,r,x);                                // void: A[i]をx倍 O(log(N+1))
// addはa=1、assignはa=0の一次変換。整数Stats型は負のaにも対応。

// 二進列（LazyFlipSum / LazyFlipInv。初期値・setの値は0/非0で0/1に正規化）
seg.flip(l,r);                                 // void: [l,r)の0/1を反転 O(log(N+1))
seg.ones(l,r);                                 // long long: LazyFlipSumで区間の1の個数を返す O(log(N+1))
seg.inversions(l,r);                           // long long: LazyFlipInvでi<jかつ(A[i],A[j])=(1,0)の組数 O(log(N+1))

// 等差数列加算（LazyAPAddSum）
seg.add_linear(l,r,a,b);                       // void: A[i]+=a*i+b（iは配列全体の添字） O(log(N+1))
seg.add_ap(l,r,a,d);                           // void: A[l+j]+=a+d*j（jは0始まり） O(log(N+1))

// 公開している葉配列への変換ヘルパー（必要な場合のみ使用）
LazyAddArgMin::cv(v);                          // vector<S>: {値,元の添字}へ変換 O(N)
LazyAddArgMax::cv(v);                          // vector<S>: {値,元の添字}へ変換 O(N)
LazyFlipSum::cv(v);                            // vector<S>: {0/1の値,1}へ変換 O(N)
LazyFlipInv::cv(v);                            // vector<S>: {zero,one,0}へ変換 O(N)
LazyAPAddSum::cv(v);                           // vector<S>: {値,1,元の添字}へ変換 O(N)

基本の関数
LazyAddSum seg(vector<ll>{1,2,3});             // 構築 O(N)
seg.apply(0,2,5);                             // [0,2)へ5加算 O(log N)
auto result = seg.prod(0,3);                  // S={val=16,size=3} O(log N)
seg.set(1,10);                                // 点代入 O(log N)
auto value = seg.get(1);                      // S={val=10,size=1} O(log N)
*/
