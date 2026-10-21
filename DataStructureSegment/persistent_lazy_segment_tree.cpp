template<class S,auto op,auto e,class F,auto mapping,auto composition,auto id,int NODES=10000000>
class PersistentLazySegmentTree{
    static_assert(NODES>=1);
    struct Node{
        S data;
        const Node *left,*right;
        F lazy;
        bool pending;
        Node(const S&x,const Node*l,const Node*r,const F&f,bool p):data(x),left(l),right(r),lazy(f),pending(p){}
    };
    struct Arena{
        std::deque<Node> nodes;
        const Node*make(const S&x,const Node*l,const Node*r,const F&f,bool pending){
            if(nodes.size()>=static_cast<std::size_t>(NODES))std::abort();
            nodes.emplace_back(x,l,r,f,pending);
            return &nodes.back();
        }
    };
    ll n_;
    std::shared_ptr<Arena> arena_;
    const Node*root_=nullptr;
    static ll length_(std::size_t n){
        if(n>static_cast<unsigned long long>(std::numeric_limits<ll>::max()))std::abort();
        return static_cast<ll>(n);
    }
    static S data_(const Node*t){return t?t->data:e();}
    static S read_(const Node*t,const F&carry,bool active){
        return active?mapping(carry,data_(t)):data_(t);
    }
    static void descend_(const Node*t,F&carry,bool&active){
        if(t&&t->pending){
            carry=active?composition(carry,t->lazy):t->lazy;
            active=true;
        }
    }
    const Node*merge_(const Node*l,const Node*r){
        return arena_->make(op(data_(l),data_(r)),l,r,id(),false);
    }
    const Node*build_(const std::vector<S>&v,ll l,ll r){
        if(r-l==1)return arena_->make(v[static_cast<std::size_t>(l)],nullptr,nullptr,id(),false);
        const ll m=l+(r-l)/2;
        const Node*left=build_(v,l,m);
        return merge_(left,build_(v,m,r));
    }
    const Node*apply_(const Node*t,const F&f){
        return arena_->make(mapping(f,data_(t)),t?t->left:nullptr,t?t->right:nullptr,
                            t&&t->pending?composition(f,t->lazy):f,true);
    }
    void children_(const Node*t,const Node*&left,const Node*&right){
        left=t?t->left:nullptr;
        right=t?t->right:nullptr;
        if(t&&t->pending){
            left=apply_(left,t->lazy);
            right=apply_(right,t->lazy);
        }
    }
    const Node*set_(const Node*t,ll p,const S&x,ll l,ll r){
        if(r-l==1)return arena_->make(x,nullptr,nullptr,id(),false);
        const ll m=l+(r-l)/2;
        const Node *left,*right;
        children_(t,left,right);
        if(p<m)left=set_(left,p,x,l,m);
        else right=set_(right,p,x,m,r);
        return merge_(left,right);
    }
    const Node*apply_range_(const Node*t,ll a,ll b,const F&f,ll l,ll r){
        if(a<=l&&r<=b)return apply_(t,f);
        const ll m=l+(r-l)/2;
        const Node *left,*right;
        children_(t,left,right);
        if(a<m)left=apply_range_(left,a,b,f,l,m);
        if(m<b)right=apply_range_(right,a,b,f,m,r);
        return merge_(left,right);
    }
    S prod_(const Node*t,ll a,ll b,ll l,ll r,F carry,bool active)const{
        if(r<=a||b<=l)return e();
        if(a<=l&&r<=b)return read_(t,carry,active);
        descend_(t,carry,active);
        const ll m=l+(r-l)/2;
        return op(prod_(t?t->left:nullptr,a,b,l,m,carry,active),
                  prod_(t?t->right:nullptr,a,b,m,r,carry,active));
    }
    template<class G>
    ll max_right_(const Node*t,ll a,ll l,ll r,S&sum,G&g,F carry,bool active)const{
        if(r<=a)return n_;
        if(a<=l){
            S next=op(sum,read_(t,carry,active));
            if(g(next)){sum=next;return n_;}
            if(r-l==1)return l;
        }
        descend_(t,carry,active);
        const ll m=l+(r-l)/2;
        const ll found=max_right_(t?t->left:nullptr,a,l,m,sum,g,carry,active);
        return found!=n_?found:max_right_(t?t->right:nullptr,a,m,r,sum,g,carry,active);
    }
    template<class G>
    ll min_left_(const Node*t,ll b,ll l,ll r,S&sum,G&g,F carry,bool active)const{
        if(b<=l)return 0;
        if(r<=b){
            S next=op(read_(t,carry,active),sum);
            if(g(next)){sum=next;return 0;}
            if(r-l==1)return r;
        }
        descend_(t,carry,active);
        const ll m=l+(r-l)/2;
        const ll found=min_left_(t?t->right:nullptr,b,m,r,sum,g,carry,active);
        return found!=0?found:min_left_(t?t->left:nullptr,b,l,m,sum,g,carry,active);
    }
public:
    PersistentLazySegmentTree():PersistentLazySegmentTree(0){}
    explicit PersistentLazySegmentTree(ll n):n_(n),arena_(std::make_shared<Arena>()){
        if(n<0)std::abort();
    }
    explicit PersistentLazySegmentTree(const std::vector<S>&v):PersistentLazySegmentTree(length_(v.size())){
        if(n_)root_=build_(v,0,n_);
    }
    PersistentLazySegmentTree(const PersistentLazySegmentTree&)=default;
    PersistentLazySegmentTree&operator=(const PersistentLazySegmentTree&)=default;
    PersistentLazySegmentTree(PersistentLazySegmentTree&&other)noexcept
        :n_(std::exchange(other.n_,0)),arena_(std::move(other.arena_)),root_(std::exchange(other.root_,nullptr)){}
    PersistentLazySegmentTree&operator=(PersistentLazySegmentTree&&other)noexcept{
        if(this!=&other){
            n_=std::exchange(other.n_,0);
            arena_=std::move(other.arena_);
            root_=std::exchange(other.root_,nullptr);
        }
        return *this;
    }
    ll size()const{return n_;}
    int nodes_used()const{return arena_?static_cast<int>(arena_->nodes.size()):0;}
    void set(ll p,S x){
        if(p<0||p>=n_)std::abort();
        root_=set_(root_,p,x,0,n_);
    }
    S get(ll p)const{
        if(p<0||p>=n_)std::abort();
        ll l=0,r=n_;
        const Node*t=root_;
        F carry=id();
        bool active=false;
        while(r-l>1){
            descend_(t,carry,active);
            const ll m=l+(r-l)/2;
            if(p<m){t=t?t->left:nullptr;r=m;}
            else{t=t?t->right:nullptr;l=m;}
        }
        return read_(t,carry,active);
    }
    S prod(ll l,ll r)const{
        if(l<0||l>r||r>n_)std::abort();
        return l==r?e():prod_(root_,l,r,0,n_,id(),false);
    }
    S all_prod()const{return data_(root_);}
    void apply(ll p,F f){
        if(p<0||p>=n_)std::abort();
        root_=apply_range_(root_,p,p+1,f,0,n_);
    }
    void apply(ll l,ll r,F f){
        if(l<0||l>r||r>n_)std::abort();
        if(l!=r)root_=apply_range_(root_,l,r,f,0,n_);
    }
    template<bool(*g)(S)>
    ll max_right(ll l)const{return max_right(l,g);}
    template<class G>
    ll max_right(ll l,G g)const{
        if(l<0||l>n_||!g(e()))std::abort();
        if(l==n_)return n_;
        S sum=e();
        return max_right_(root_,l,0,n_,sum,g,id(),false);
    }
    template<bool(*g)(S)>
    ll min_left(ll r)const{return min_left(r,g);}
    template<class G>
    ll min_left(ll r,G g)const{
        if(r<0||r>n_||!g(e()))std::abort();
        if(r==0)return 0;
        S sum=e();
        return min_left_(root_,r,0,n_,sum,g,id(),false);
    }
};

/*
細かい注意点
0-indexed、区間は[l,r)。型・演算の指定と操作名はACLのlazy_segtreeに対応する。
opは結合則、e()は左右の単位元、mapping(id(),x)=x、mapping(f,op(x,y))=op(mapping(f,x),mapping(f,y))。
composition(f,g)はgを適用した後にfを適用する合成。Fの等値比較やデフォルト構築は不要。
auto old=seg;で版をO(1)保存。set/applyはそのオブジェクトだけを更新し、oldは変えない。
読み取りはconstで、遅延伝播によるノードの変更・確保を行わない。mapping(f,e())!=e()にも対応する。
0<=N<=LLONG_MAX。Nだけの構築はACL同様、全要素e()。区間和の長さはvector<S>(N,{0,1})等で初期化する。
NODES>=1は共有全版のノード上限。必要な分だけ確保し、最後の共有先の破棄時に全て解放する。
vector構築はN>0なら2*N-1個。各set/applyはO(log(N+1))個を追加し、個別には回収しない。
コピー・代入は共有、ムーブ後の元は空。添字・容量・g(e())違反はNDEBUGでもメッセージなしで停止。
gは副作用なしで単調であること。S/Fのコピーと演算・述語をO(1)として計算量を表記。追加include不要。

使える関数一覧
PersistentLazySegmentTree<S,op,e,F,mapping,composition,id,NODES> seg;    // 長さ0で構築 O(1)
PersistentLazySegmentTree<S,op,e,F,mapping,composition,id,NODES> seg(N); // 全要素e()で構築 O(1)
PersistentLazySegmentTree<S,op,e,F,mapping,composition,id,NODES> seg(v); // vector<S>から構築 O(N+1)
auto old=seg;                              // コピーで版を保存 O(1)
void seg.set(p,x);                         // a[p]=x、この版のみ更新 償却O(log(N+1))
S seg.get(p);                              // a[p]を返す O(log(N+1))
S seg.prod(l,r);                           // [l,r)を添字順に集約 O(log(N+1))
S seg.all_prod();                          // 全区間の集約を返す O(1)
void seg.apply(p,f);                       // a[p]=mapping(f,a[p]) 償却O(log(N+1))
void seg.apply(l,r,f);                     // [l,r)の各要素にfを適用 償却O(log(N+1))
ll seg.max_right(l,g);                     // g(prod(l,r))が真となる最大r O(log(N+1))
ll seg.min_left(r,g);                      // g(prod(l,r))が真となる最小l O(log(N+1))
ll seg.max_right<g>(l);                    // 関数ポインタ版、同じ動作・計算量
ll seg.min_left<g>(r);                     // 関数ポインタ版、同じ動作・計算量
ll seg.size();                            // 配列長を返す O(1)
int seg.nodes_used();                     // 共有全版の確保済みノード数 O(1)

基本の関数
struct S{ll sum,len;};
struct F{ll a,b;};
constexpr auto op=[](S x,S y){return S{x.sum+y.sum,x.len+y.len};};
constexpr auto e=[](){return S{0,0};};
constexpr auto mapping=[](F f,S x){return S{f.a*x.sum+f.b*x.len,x.len};};
constexpr auto composition=[](F f,F g){return F{f.a*g.a,f.a*g.b+f.b};};
constexpr auto id=[](){return F{1,0};};
PersistentLazySegmentTree<S,op,e,F,mapping,composition,id,4096> seg(std::vector<S>{{1,1},{2,1},{3,1}});
auto old=seg;
seg.apply(0,2,F{1,5});                      // [1,2,3] -> [6,7,3]
seg.apply(2,F{2,0});                        // [6,7,6]
auto branch=old;
branch.set(1,S{10,1});                      // [1,10,3]
auto sum=seg.prod(0,3).sum;                 // 19
auto old_sum=old.all_prod().sum;            // 6
auto branch_sum=branch.all_prod().sum;      // 14
auto right=seg.max_right(0,[](S x){return x.sum<=13;}); // 2
auto left=seg.min_left(3,[](S x){return x.sum<=13;});   // 1
*/
