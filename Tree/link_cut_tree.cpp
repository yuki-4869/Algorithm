template<class S,auto op,auto e>
class LinkCutTree{
    struct Node{
        int ch[2]={-1,-1},p=-1,size=1;
        bool reverse=false;
        S value,forward,backward;
        explicit Node(const S&x):value(x),forward(x),backward(x){}
    };
    vector<Node> nodes_;
    vector<int> stack_;
    int vertex_(ll v)const{
        if(v<0||v>=size())std::abort();
        return static_cast<int>(v);
    }
    int count_(int x)const{return x==-1?0:nodes_[x].size;}
    bool auxiliary_root_(int x)const{
        const int p=nodes_[x].p;
        return p==-1||(nodes_[p].ch[0]!=x&&nodes_[p].ch[1]!=x);
    }
    void pull_(int x){
        auto&t=nodes_[x];
        t.size=1+count_(t.ch[0])+count_(t.ch[1]);
        t.forward=op(op(data_(t.ch[0],false),t.value),data_(t.ch[1],false));
        t.backward=op(op(data_(t.ch[1],true),t.value),data_(t.ch[0],true));
    }
    S data_(int x,bool reverse)const{
        return x==-1?e():(reverse?nodes_[x].backward:nodes_[x].forward);
    }
    void reverse_(int x){
        if(x==-1)return;
        auto&t=nodes_[x];
        swap(t.ch[0],t.ch[1]);t.reverse=!t.reverse;
        swap(t.forward,t.backward);
    }
    void push_(int x){
        auto&t=nodes_[x];
        if(t.reverse){reverse_(t.ch[0]);reverse_(t.ch[1]);t.reverse=false;}
    }
    void rotate_(int x){
        const int p=nodes_[x].p,g=nodes_[p].p,d=nodes_[p].ch[1]==x;
        const int middle=nodes_[x].ch[d^1];
        if(!auxiliary_root_(p))nodes_[g].ch[nodes_[g].ch[1]==p]=x;
        nodes_[x].p=g;nodes_[x].ch[d^1]=p;nodes_[p].p=x;
        nodes_[p].ch[d]=middle;
        if(middle!=-1)nodes_[middle].p=p;
        pull_(p);pull_(x);
    }
    void splay_(int x){
        stack_.clear();stack_.push_back(x);
        for(int y=x;!auxiliary_root_(y);){y=nodes_[y].p;stack_.push_back(y);}
        for(auto it=stack_.rbegin();it!=stack_.rend();++it)push_(*it);
        while(!auxiliary_root_(x)){
            const int p=nodes_[x].p,g=nodes_[p].p;
            if(!auxiliary_root_(p))rotate_((nodes_[p].ch[1]==x)==(nodes_[g].ch[1]==p)?p:x);
            rotate_(x);
        }
    }
    int access_(int x){
        int last=-1;
        for(int y=x;y!=-1;y=nodes_[y].p){
            splay_(y);nodes_[y].ch[1]=last;
            if(last!=-1)nodes_[last].p=y;
            pull_(y);last=y;
        }
        splay_(x);return last;
    }
    void make_root_(int x){access_(x);reverse_(x);}
    int find_root_(int x){
        access_(x);push_(x);
        while(nodes_[x].ch[0]!=-1){x=nodes_[x].ch[0];push_(x);}
        splay_(x);return x;
    }
    bool expose_(int u,int v){
        make_root_(u);
        if(find_root_(v)!=u)return false;
        access_(v);return true;
    }
    int select_(int x,int k){
        while(true){
            push_(x);const int left=count_(nodes_[x].ch[0]);
            if(k==left){splay_(x);return x;}
            if(k<left)x=nodes_[x].ch[0];
            else{k-=left+1;x=nodes_[x].ch[1];}
        }
    }
public:
    LinkCutTree()=default;
    explicit LinkCutTree(ll n){
        if(n<0||n>INT_MAX)std::abort();
        nodes_.reserve(static_cast<size_t>(n));
        for(ll i=0;i<n;++i)nodes_.emplace_back(e());
    }
    explicit LinkCutTree(const vector<S>&v){
        if(v.size()>static_cast<size_t>(INT_MAX))std::abort();
        nodes_.reserve(v.size());
        for(const auto&x:v)nodes_.emplace_back(x);
    }
    ll size()const{return static_cast<ll>(nodes_.size());}
    bool same(ll u,ll v){
        const int a=vertex_(u),b=vertex_(v);
        return find_root_(a)==find_root_(b);
    }
    bool link(ll u,ll v){
        const int a=vertex_(u),b=vertex_(v);
        make_root_(a);
        if(find_root_(b)==a)return false;
        nodes_[a].p=b;return true;
    }
    bool cut(ll u,ll v){
        const int a=vertex_(u),b=vertex_(v);
        if(!expose_(a,b)||nodes_[b].ch[0]!=a)return false;
        push_(a);
        if(nodes_[a].ch[1]!=-1)return false;
        nodes_[b].ch[0]=-1;nodes_[a].p=-1;pull_(b);return true;
    }
    ll path_size(ll u,ll v){
        const int a=vertex_(u),b=vertex_(v);
        return expose_(a,b)?nodes_[b].size:0;
    }
    ll dist_edges(ll u,ll v){return path_size(u,v)-1;}
    ll kth(ll u,ll v,ll k){
        const int a=vertex_(u),b=vertex_(v);
        if(!expose_(a,b)||k<0||k>=nodes_[b].size)return -1;
        return select_(b,static_cast<int>(k));
    }
    ll jump(ll u,ll v,ll k){return kth(u,v,k);}
    ll lca(ll root,ll u,ll v){
        const int r=vertex_(root),a=vertex_(u),b=vertex_(v);
        make_root_(r);
        if(find_root_(a)!=r||find_root_(b)!=r)return -1;
        access_(a);return access_(b);
    }
    ll parent(ll root,ll v){
        const int r=vertex_(root),x=vertex_(v);
        if(!expose_(r,x)||r==x)return -1;
        return select_(x,nodes_[x].size-2);
    }
    ll kth_ancestor(ll root,ll v,ll k){return kth(v,root,k);}
    void set(ll v,const S&x){
        const int a=vertex_(v);access_(a);nodes_[a].value=x;pull_(a);
    }
    S get(ll v){const int a=vertex_(v);access_(a);return nodes_[a].value;}
    S prod(ll u,ll v){
        const int a=vertex_(u),b=vertex_(v);
        if(!expose_(a,b))std::abort();
        return nodes_[b].forward;
    }
    template<class G>
    ll max_right(ll u,ll v,G g){
        const int a=vertex_(u),b=vertex_(v);
        if(!g(e())||!expose_(a,b))std::abort();
        if(g(nodes_[b].forward))return nodes_[b].size;
        S sum=e();ll length=0;int x=b;
        while(true){
            push_(x);const int left=nodes_[x].ch[0];
            S next=op(sum,data_(left,false));
            if(!g(next)){x=left;continue;}
            length+=count_(left);sum=op(next,nodes_[x].value);
            if(!g(sum)){splay_(x);return length;}
            ++length;x=nodes_[x].ch[1];
        }
    }
};
namespace lct_monoid{
    inline ll sum(ll a,ll b){return a+b;}
    inline ll zero(){return 0;}
    inline ll minimum(ll a,ll b){return min(a,b);}
    inline ll maximum(ll a,ll b){return max(a,b);}
    inline ll infinity(){return LLONG_MAX;}
    inline ll negative_infinity(){return LLONG_MIN;}
}
using LCTSum=LinkCutTree<ll,lct_monoid::sum,lct_monoid::zero>;
using LCTMin=LinkCutTree<ll,lct_monoid::minimum,lct_monoid::infinity>;
using LCTMax=LinkCutTree<ll,lct_monoid::maximum,lct_monoid::negative_infinity>;

/*
細かい注意点
0-indexedの無向の森。Nは0以上INT_MAX以下。辺はlinkで登録し、閉路は作れない。
頂点番号・個数が不正ならNDEBUGでもメッセージなしで停止。link/cutの失敗はfalseで返す。
path_sizeは両端を含む頂点数。非連結なら0、dist_edgesは-1。kth/jumpは0-indexed。
lca/parent/kth_ancestorは毎回rootを明示する。取得操作も内部の根や補助木を変えるためconstではない。
該当頂点がない場合と必要な頂点が非連結の場合、頂点取得APIは-1を返す。
再帰なし。構築・独立コピーはO(N)、保持領域O(N)。各操作は償却O(log N)、単発ではO(N)の場合がある。
部分木・成分の値の集計や更新、一般グラフの連結性、永続化には非対応。
llは基本テンプレートの型。opは結合的、eは単位元。非可換演算にも対応し、prod(u,v)はuからvの順。
Sと演算結果は設定した型に収まること。各演算・値コピー・述語がO(1)の場合の計算量。
prod/max_rightには連結な両端が必要。違反時はメッセージなしで停止。
max_rightはg(e())=trueかつ接頭部分に対する単調性が必要。返り値は頂点番号でなく頂点数。
LCTSum/Min/MaxのN構築はそれぞれ0/LLONG_MAX/LLONG_MIN。追加include不要。

使える関数一覧
LinkCutTree<S,op,e> lct;                  // 空の森 O(1)
LinkCutTree<S,op,e> lct(N);               // N個の孤立頂点 O(N)
LinkCutTree<S,op,e> lct(v);               // vector<S>の頂点値から構築 O(N)
LCTSum/ LCTMin/ LCTMax lct(N);       // llの総和/最小/最大。vでも構築可 O(N)
lct.size();                         // ll: 頂点数 O(1)
lct.link(u,v);                      // bool: 別成分なら辺を追加、自己辺・閉路はfalse 償却O(log N)
lct.cut(u,v);                       // bool: 存在する辺だけ削除、それ以外false 償却O(log N)
lct.same(u,v);                      // bool: 同じ木に属するか 償却O(log N)
lct.path_size(u,v);                 // ll: パスの頂点数、非連結なら0 償却O(log N)
lct.dist_edges(u,v);                // ll: パスの辺数、非連結なら-1 償却O(log N)
lct.kth(u,v,k);                     // ll: uからvへk番目、なければ-1 償却O(log N)
lct.jump(u,v,k);                    // ll: kthと同じ 償却O(log N)
lct.lca(root,u,v);                  // ll: rootを根とするLCA、非連結なら-1 償却O(log N)
lct.parent(root,v);                 // ll: rootを根とする親、根・非連結なら-1 償却O(log N)
lct.kth_ancestor(root,v,k);          // ll: vのk個上の祖先、なければ-1 償却O(log N)
lct.set(v,x);                       // void: 頂点値を置換 償却O(log N)
lct.get(v);                         // S: 頂点値のコピー 償却O(log N)
lct.prod(u,v);                      // S: uからvへのパス積 償却O(log N)
lct.max_right(u,v,g);               // ll: gを満たす最大接頭部分の頂点数 償却O(log N)

基本の関数
LCTSum lct(vector<ll>{1,2,3,4});
lct.link(0,1);lct.link(1,2);lct.link(1,3);
assert(lct.prod(2,3)==9);
lct.set(1,10);
assert(lct.get(1)==10 && lct.prod(2,3)==17);
auto length=lct.max_right(2,3,[](ll sum){return sum<=13;});
assert(length==2 && lct.kth(2,3,length)==3);
lct.cut(1,2);lct.link(0,2);
assert(lct.prod(2,3)==18);
*/
