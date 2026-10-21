template<class T,class F,int NODES=10000000>
class PersistentSegmentTree{
    static_assert(NODES>=1);
public:
    struct Node{
        T data;
        Node *l,*r;
        Node(const T&value,Node*left,Node*right):data(value),l(left),r(right){}
    };
private:
    ll n_;
    const F f_;
    const T id_;
    std::deque<Node> pool_;
    Node*nil_=nullptr;

    Node*make_node_(const T&value,Node*left,Node*right){
        if(pool_.size()>=static_cast<std::size_t>(NODES))std::abort();
        pool_.emplace_back(value,left,right);
        return &pool_.back();
    }
    T combine_(const T&left,const T&right)const{return f_(left,right);}
    Node*merge_(Node*left,Node*right){return make_node_(combine_(left->data,right->data),left,right);}
    void check_root_(const Node*root)const{if(root==nullptr)std::abort();}
    Node*build_(const std::vector<T>&v,ll l,ll r){
        if(r-l==1)return make_node_(v[static_cast<std::size_t>(l)],nil_,nil_);
        const ll m=l+(r-l)/2;
        return merge_(build_(v,l,m),build_(v,m,r));
    }
    template<class G>
    Node*update_apply_(Node*root,ll k,const G&g,ll l,ll r){
        if(r-l==1){T value=root->data;return make_node_(g(value),nil_,nil_);}
        const ll m=l+(r-l)/2;
        if(k<m)return merge_(update_apply_(root->l,k,g,l,m),root->r);
        return merge_(root->l,update_apply_(root->r,k,g,m,r));
    }
    T query_(Node*root,ll a,ll b,ll l,ll r)const{
        if(root==nil_||r<=a||b<=l)return id_;
        if(a<=l&&r<=b)return root->data;
        const ll m=l+(r-l)/2;
        return combine_(query_(root->l,a,b,l,m),query_(root->r,a,b,m,r));
    }
    template<class C>
    ll max_right_(Node*root,ll l,ll r,ll ql,T&sum,const C&check)const{
        if(r<=ql)return n_;
        if(ql<=l){
            T next=combine_(sum,root->data);
            if(check(next)){sum=next;return n_;}
            if(r-l==1)return l;
        }
        const ll m=l+(r-l)/2;
        const ll res=max_right_(root->l,l,m,ql,sum,check);
        if(res!=n_)return res;
        return max_right_(root->r,m,r,ql,sum,check);
    }
public:
    PersistentSegmentTree(ll n,const F&f,const T&id):n_(n),f_(f),id_(id){
        if(n<0)std::abort();
        nil_=make_node_(id_,nullptr,nullptr);
        nil_->l=nil_->r=nil_;
    }
    PersistentSegmentTree(const PersistentSegmentTree&)=delete;
    PersistentSegmentTree&operator=(const PersistentSegmentTree&)=delete;
    PersistentSegmentTree(PersistentSegmentTree&&)=delete;
    PersistentSegmentTree&operator=(PersistentSegmentTree&&)=delete;

    ll size()const{return n_;}
    int nodes_used()const{return static_cast<int>(pool_.size());}
    Node*new_tree()const{return nil_;}
    Node*build(const std::vector<T>&v){
        if(static_cast<unsigned long long>(v.size())!=static_cast<unsigned long long>(n_))std::abort();
        return n_==0?nil_:build_(v,0,n_);
    }
    Node*update(Node*root,ll k,const T&x){
        return update_apply(root,k,[&](const T&){return x;});
    }
    template<class G>
    Node*update_apply(Node*root,ll k,const G&g){
        check_root_(root);
        if(k<0||k>=n_)std::abort();
        return update_apply_(root,k,g,0,n_);
    }
    T query(Node*root,ll a,ll b)const{
        check_root_(root);
        if(a<0||a>b||b>n_)std::abort();
        return a==b?id_:query_(root,a,b,0,n_);
    }
    template<class C>
    ll max_right(Node*root,ll l,const C&check)const{
        check_root_(root);
        if(l<0||l>n_||!check(id_))std::abort();
        if(l==n_)return n_;
        T sum=id_;
        return max_right_(root,0,n_,l,sum,check);
    }
};

/*
細かい注意点
0-indexed、区間は[l,r)。0<=N<=LLONG_MAX。fは結合則、IDは左右の単位元。
build(v)はv.size()==N。new_treeは全要素IDの根を追加ノードなしで返す。
update/update_applyは新しい根を返し、古い根を変更しない。返り値を保存すること。
gには旧値の作業用コピーを渡す。参照引数を書き換えるgでも共有ノードは変更しない。
根は同じpstが返したものだけを使い、Nodeのdata/l/rを直接変更しない。
木本体のコピー・ムーブは禁止。根ポインタのコピーは状態の共有O(1)。pst破棄時に全根が無効になる。
NODES>=1は全バージョンを合わせたノード上限。番兵1個を含み、必要な分だけ確保する。
N>0のbuildは2*N-1個、1点更新は最大ceil(log2(N))+1個を追加する。ノードは個別解放しない。
容量・添字・列長・nullptr・述語の単位元検査はNDEBUGでも有効で、違反時はメッセージなしで停止する。
max_rightの述語は副作用なし、check(ID)==true、右端を伸ばすとtrueからfalseに変わる単調性が必要。
Tのコピーとf/g/checkをO(1)とする計算量。整数演算は型に収める。追加includeは不要。

使える関数一覧
PersistentSegmentTree<T,F,NODES> pst(N,f,ID);  // 空の共有根を用意、時間・初期領域O(1)
ll pst.size();                               // 配列長を返す O(1)
int pst.nodes_used();                        // 番兵を含む確保済みノード数を返す O(1)
Node* pst.new_tree();                        // 全要素IDの根を返す O(1)
Node* pst.build(v);                          // 初期列の新しい根を返す O(N+1)
Node* pst.update(root,i,x);                   // 1点代入後の新しい根を返す 償却O(log(N+1))
Node* pst.update_apply(root,i,g);             // 1点にgを適用した新しい根を返す 償却O(log(N+1))
T pst.query(root,l,r);                        // [l,r)を添字順に集約 O(log(N+1))、空区間はO(1)
ll pst.max_right(root,l,check);               // 条件を満たす最大の右端を返す O(log(N+1))

基本の関数
auto op=[](ll a,ll b){return a+b;};
PersistentSegmentTree<ll,decltype(op),1024> pst(3,op,0LL);
auto root0=pst.build(vl{1,2,3});
auto root1=pst.update(root0,1,10);             // root0を残して新しい版を作る
auto root2=pst.update_apply(root0,2,[](ll x){return x+5;});
auto old_sum=pst.query(root0,0,3);             // 6
auto new_sum=pst.query(root1,0,3);             // 14
auto branch_sum=pst.query(root2,0,3);          // 11
auto right=pst.max_right(root1,0,[](ll x){return x<=11;}); // 2
auto blank=pst.new_tree();                    // 全要素0の版
auto used=pst.nodes_used();                   // 確保済みノード数
*/
