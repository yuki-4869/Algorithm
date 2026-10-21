template<class S,auto op,auto e,int NODES=10000000>
class PersistentSegmentTree{
    static_assert(NODES>=1);
    struct Node{
        S data;
        const Node *left,*right;
        Node(const S&x,const Node*l,const Node*r):data(x),left(l),right(r){}
    };
    struct Arena{
        std::deque<Node> nodes;
        const Node*make(const S&x,const Node*l,const Node*r){
            if(nodes.size()>=static_cast<std::size_t>(NODES))std::abort();
            nodes.emplace_back(x,l,r);
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
    const Node*merge_(const Node*l,const Node*r){return arena_->make(op(data_(l),data_(r)),l,r);}
    const Node*build_(const std::vector<S>&v,ll l,ll r){
        if(r-l==1)return arena_->make(v[static_cast<std::size_t>(l)],nullptr,nullptr);
        const ll m=l+(r-l)/2;
        const Node*left=build_(v,l,m);
        return merge_(left,build_(v,m,r));
    }
    const Node*set_(const Node*t,ll p,const S&x,ll l,ll r){
        if(r-l==1)return arena_->make(x,nullptr,nullptr);
        const ll m=l+(r-l)/2;
        const Node*left=t?t->left:nullptr;
        const Node*right=t?t->right:nullptr;
        if(p<m)left=set_(left,p,x,l,m);
        else right=set_(right,p,x,m,r);
        return merge_(left,right);
    }
    S prod_(const Node*t,ll a,ll b,ll l,ll r)const{
        if(!t||r<=a||b<=l)return e();
        if(a<=l&&r<=b)return t->data;
        const ll m=l+(r-l)/2;
        return op(prod_(t->left,a,b,l,m),prod_(t->right,a,b,m,r));
    }
    template<class G>
    ll max_right_(const Node*t,ll a,ll l,ll r,S&sum,G&g)const{
        if(!t||r<=a)return n_;
        if(a<=l){
            S next=op(sum,t->data);
            if(g(next)){sum=next;return n_;}
            if(r-l==1)return l;
        }
        const ll m=l+(r-l)/2;
        const ll found=max_right_(t->left,a,l,m,sum,g);
        return found!=n_?found:max_right_(t->right,a,m,r,sum,g);
    }
    template<class G>
    ll min_left_(const Node*t,ll b,ll l,ll r,S&sum,G&g)const{
        if(!t||b<=l)return 0;
        if(r<=b){
            S next=op(t->data,sum);
            if(g(next)){sum=next;return 0;}
            if(r-l==1)return r;
        }
        const ll m=l+(r-l)/2;
        const ll found=min_left_(t->right,b,m,r,sum,g);
        return found!=0?found:min_left_(t->left,b,l,m,sum,g);
    }
public:
    PersistentSegmentTree():PersistentSegmentTree(0){}
    explicit PersistentSegmentTree(ll n):n_(n),arena_(std::make_shared<Arena>()){
        if(n<0)std::abort();
    }
    explicit PersistentSegmentTree(const std::vector<S>&v):PersistentSegmentTree(length_(v.size())){
        if(n_)root_=build_(v,0,n_);
    }
    PersistentSegmentTree(const PersistentSegmentTree&)=default;
    PersistentSegmentTree&operator=(const PersistentSegmentTree&)=default;
    PersistentSegmentTree(PersistentSegmentTree&&other)noexcept
        :n_(std::exchange(other.n_,0)),arena_(std::move(other.arena_)),root_(std::exchange(other.root_,nullptr)){}
    PersistentSegmentTree&operator=(PersistentSegmentTree&&other)noexcept{
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
        while(t&&r-l>1){
            const ll m=l+(r-l)/2;
            if(p<m){t=t->left;r=m;}
            else{t=t->right;l=m;}
        }
        return data_(t);
    }
    S prod(ll l,ll r)const{
        if(l<0||l>r||r>n_)std::abort();
        return l==r?e():prod_(root_,l,r,0,n_);
    }
    S all_prod()const{return data_(root_);}
    template<bool(*g)(S)>
    ll max_right(ll l)const{return max_right(l,g);}
    template<class G>
    ll max_right(ll l,G g)const{
        if(l<0||l>n_||!g(e()))std::abort();
        if(l==n_)return n_;
        S sum=e();
        return max_right_(root_,l,0,n_,sum,g);
    }
    template<bool(*g)(S)>
    ll min_left(ll r)const{return min_left(r,g);}
    template<class G>
    ll min_left(ll r,G g)const{
        if(r<0||r>n_||!g(e()))std::abort();
        if(r==0)return 0;
        S sum=e();
        return min_left_(root_,r,0,n_,sum,g);
    }
};

/*
細かい注意点
0-indexed、区間は[l,r)。opとeはACLのsegtreeと同じ指定。opは結合則、e()は左右の単位元。
auto old=seg;で現在の版をO(1)で保存。setはそのオブジェクトだけを更新し、oldは変えない。
コピー・代入は共有、ムーブ後の元オブジェクトは空。全ての共有先の破棄時にノードを解放する。
0<=N<=LLONG_MAX。Nだけの構築は全要素e()を仮想的に保持する。getはACLと異なりO(log(N+1))。
NODES>=1はコピーで共有する全版の合計ノード上限。必要な分だけ確保し、個別には回収しない。
vector構築はN>0なら2*N-1個、setは最大ceil(log2(N))+1個を追加する。
添字・容量・g(e())の違反はNDEBUGでもメッセージなしで停止。gは副作用なしで単調であること。
Sのコピー、op/e/gはO(1)として計算量を表記。共有したSの内部も外部から変更しない。追加include不要。

使える関数一覧
PersistentSegmentTree<S,op,e,NODES> seg;     // 長さ0で構築 O(1)
PersistentSegmentTree<S,op,e,NODES> seg(N);  // 全要素e()で構築 O(1)
PersistentSegmentTree<S,op,e,NODES> seg(v);  // vector<S>から構築 O(N+1)
auto old=seg;                               // コピーで版を保存 O(1)
void seg.set(p,x);                          // a[p]=x、この版のみ更新 償却O(log(N+1))
S seg.get(p);                              // a[p]を返す O(log(N+1))
S seg.prod(l,r);                           // [l,r)を添字順に集約 O(log(N+1))
S seg.all_prod();                          // 全区間の集約を返す O(1)
ll seg.max_right(l,g);                     // g(prod(l,r))が真となる最大r O(log(N+1))
ll seg.min_left(r,g);                      // g(prod(l,r))が真となる最小l O(log(N+1))
ll seg.max_right<g>(l);                    // 関数ポインタ版、同じ動作・計算量
ll seg.min_left<g>(r);                     // 関数ポインタ版、同じ動作・計算量
ll seg.size();                            // 配列長を返す O(1)
int seg.nodes_used();                     // 共有全版の確保済みノード数 O(1)

基本の関数
constexpr auto op=[](ll a,ll b){return a+b;};
constexpr auto e=[](){return 0LL;};
PersistentSegmentTree<ll,op,e,1024> seg(vl{1,2,3});
auto old=seg;
seg.set(1,10);
auto branch=old;
branch.set(2,8);
auto value=seg.get(1);                      // 10
auto sum=seg.prod(0,3);                     // 14
auto old_sum=old.all_prod();                // 6
auto branch_sum=branch.all_prod();          // 11
auto right=seg.max_right(0,[](ll x){return x<=11;}); // 2
auto left=seg.min_left(3,[](ll x){return x<=13;});   // 1
*/
