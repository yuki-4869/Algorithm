
struct DynamicRollingHash {
    using ll=long long; using ull=unsigned long long; using u128=__uint128_t;
    static constexpr ull MOD=(1ULL<<61)-1;
    struct Hash{ array<ull,2> f{},r{},pw{1,1}; long long len=0; };

private:
    struct Params{ array<ull,2> base,salt; };
    static ull sm(ull x){ x+=0x9e3779b97f4a7c15ULL; x=(x^(x>>30))*0xbf58476d1ce4e5b9ULL; x=(x^(x>>27))*0x94d049bb133111ebULL; return x^(x>>31); }
    static ull seed(ull tag){ ull x=chrono::steady_clock::now().time_since_epoch().count()^(ull)(uintptr_t)&x^tag; random_device rd; return sm(x^((ull)rd()<<32)^rd()); }
    static ull hrand(){ static ull s=seed(0x243f6a8885a308d3ULL); return sm(s+=0x9e3779b97f4a7c15ULL); }
    static ull trand(){ static ull s=seed(0x13198a2e03707344ULL); return sm(s+=0x9e3779b97f4a7c15ULL); }
    static const Params& par(){
        static Params p=[](){
            Params z; constexpr ull LO=1000000007ULL,HI=MOD-1000000007ULL;
            for(int k=0;k<2;k++){ do z.base[k]=LO+hrand()%(HI-LO+1); while(z.base[k]<=1||z.base[k]>=MOD-1); z.salt[k]=hrand(); }
            while(z.base[0]==z.base[1]) z.base[1]=LO+hrand()%(HI-LO+1);
            return z;
        }(); return p;
    }
    static ull add(ull a,ull b){ a+=b; return a>=MOD?a-MOD:a; }
    static ull mul(ull a,ull b){ u128 z=(u128)a*b; ull x=(ull)(z&MOD)+(ull)(z>>61); x=(x&MOD)+(x>>61); return x>=MOD?x-MOD:x; }
    static ull enc(ll x,int k){ return sm((ull)x^par().salt[k])%(MOD-1)+1; }

public:
    static Hash e(){ return {}; }
    static Hash single(ll x){ Hash h; h.len=1; for(int k=0;k<2;k++) h.f[k]=h.r[k]=enc(x,k),h.pw[k]=par().base[k]; return h; }
    static Hash concat(const Hash&a,const Hash&b){
        Hash c; c.len=a.len+b.len;
        for(int k=0;k<2;k++) c.f[k]=add(mul(a.f[k],b.pw[k]),b.f[k]),c.r[k]=add(mul(b.r[k],a.pw[k]),a.r[k]),c.pw[k]=mul(a.pw[k],b.pw[k]);
        return c;
    }
    static Hash concat_all(initializer_list<Hash> v){ Hash r; for(auto&h:v) r=concat(r,h); return r; }
    static Hash concat_all(const vector<Hash>&v){ Hash r; for(auto&h:v) r=concat(r,h); return r; }
    static Hash reversed(Hash h){ swap(h.f,h.r); return h; }
    static Hash repeat(Hash h,long long k){ assert(k>=0); Hash r; while(k){ if(k&1) r=concat(r,h); h=concat(h,h); k>>=1; } return r; }
    static bool same(const Hash&a,const Hash&b){ return a.len==b.len&&a.f==b.f; }
    static bool same_reverse(const Hash&a,const Hash&b){ return a.len==b.len&&a.f==b.r; }

private:
    struct Node{
        ll val,assign_val=0; ull pri; Node *l=nullptr,*r=nullptr; int sz=1; bool rev=false,has_assign=false; Hash own,h;
        Node(ll x):val(x),pri(trand()),own(single(x)),h(own){}
    };
    Node* root=nullptr;
    static int sz(Node*t){ return t?t->sz:0; }
    static Hash hs(Node*t){ return t?t->h:e(); }
    static void apply_assign(Node*t,ll x){ if(!t)return; t->val=x;t->own=single(x);t->h=repeat(t->own,t->sz);t->has_assign=true;t->assign_val=x;t->rev=false; }
    static void apply_rev(Node*t){ if(!t)return; swap(t->l,t->r);swap(t->h.f,t->h.r);t->rev^=1; }
    static void push(Node*t){
        if(!t)return;
        if(t->has_assign) apply_assign(t->l,t->assign_val),apply_assign(t->r,t->assign_val),t->has_assign=false;
        if(t->rev) apply_rev(t->l),apply_rev(t->r),t->rev=false;
    }
    static void pull(Node*t){ if(t) t->sz=1+sz(t->l)+sz(t->r),t->h=concat(concat(hs(t->l),t->own),hs(t->r)); }
    static Node* merge(Node*a,Node*b){
        if(!a)return b; if(!b)return a;
        if(a->pri>b->pri){ push(a);a->r=merge(a->r,b);pull(a);return a; }
        push(b);b->l=merge(a,b->l);pull(b);return b;
    }
    static void split(Node*t,int k,Node*&a,Node*&b){
        if(!t){a=b=nullptr;return;} push(t);
        if(sz(t->l)>=k) split(t->l,k,a,t->l),pull(t),b=t;
        else split(t->r,k-sz(t->l)-1,t->r,b),pull(t),a=t;
    }
    static void split3(Node*t,int l,int r,Node*&a,Node*&b,Node*&c){ Node*x; split(t,l,a,x); split(x,r-l,b,c); }
    static void destroy(Node*t){ if(!t)return; destroy(t->l);destroy(t->r);delete t; }
    static void rebuild(Node*t){ if(!t)return; rebuild(t->l);rebuild(t->r);pull(t); }
    static Node* build_nodes(const vector<ll>&a){
        if(a.empty())return nullptr; vector<Node*> st; st.reserve(a.size());
        for(ll x:a){ Node*cur=new Node(x),*last=nullptr; while(!st.empty()&&st.back()->pri<cur->pri) last=st.back(),st.pop_back(); cur->l=last; if(!st.empty())st.back()->r=cur; st.push_back(cur); }
        Node*rt=st.front(); rebuild(rt); return rt;
    }
    static ll get_rec(Node*t,int i){ push(t); int s=sz(t->l); return i<s?get_rec(t->l,i):i==s?t->val:get_rec(t->r,i-s-1); }
    static void set_rec(Node*t,int i,ll x){ push(t); int s=sz(t->l); if(i<s)set_rec(t->l,i,x); else if(i==s)t->val=x,t->own=single(x); else set_rec(t->r,i-s-1,x); pull(t); }
    static void dump(Node*t,vector<ll>&v){ if(!t)return; push(t);dump(t->l,v);v.push_back(t->val);dump(t->r,v); }
    static vector<ll> vals(const string&s){ vector<ll>v(s.size()); for(int i=0;i<(int)s.size();i++)v[i]=(unsigned char)s[i]; return v; }
    template<class T> static vector<ll> vals(const vector<T>&a){ vector<ll>v(a.size()); for(int i=0;i<(int)a.size();i++)v[i]=(ll)a[i]; return v; }
    void insert_nodes(int p,Node*m){ Node *a,*b; split(root,p,a,b); root=merge(merge(a,m),b); }
    void replace_nodes(int l,int r,Node*m){ Node *a,*b,*c; split3(root,l,r,a,b,c); destroy(b); root=merge(merge(a,m),c); }

public:
    DynamicRollingHash()=default;
    explicit DynamicRollingHash(const string&s):root(build_nodes(vals(s))){}
    template<class T> explicit DynamicRollingHash(const vector<T>&a):root(build_nodes(vals(a))){}
    DynamicRollingHash(const DynamicRollingHash&)=delete;
    DynamicRollingHash& operator=(const DynamicRollingHash&)=delete;
    DynamicRollingHash(DynamicRollingHash&&o)noexcept:root(o.root){o.root=nullptr;}
    DynamicRollingHash& operator=(DynamicRollingHash&&o)noexcept{ if(this!=&o){destroy(root);root=o.root;o.root=nullptr;} return *this; }
    ~DynamicRollingHash(){destroy(root);}

    int size()const{return sz(root);} bool empty()const{return !root;}
    void clear(){destroy(root);root=nullptr;}
    void assign(const string&s){clear();root=build_nodes(vals(s));}
    template<class T> void assign(const vector<T>&a){clear();root=build_nodes(vals(a));}
    ll get(int i){assert(0<=i&&i<size());return get_rec(root,i);}
    void set(int i,ll x){assert(0<=i&&i<size());set_rec(root,i,x);} void set(int i,char c){set(i,(ll)(unsigned char)c);}

    void insert(int p,ll x){assert(0<=p&&p<=size());insert_nodes(p,new Node(x));}
    void insert(int p,char c){insert(p,(ll)(unsigned char)c);}
    void insert(int p,const string&s){assert(0<=p&&p<=size());insert_nodes(p,build_nodes(vals(s)));}
    template<class T> void insert(int p,const vector<T>&v){assert(0<=p&&p<=size());insert_nodes(p,build_nodes(vals(v)));}
    void push_front(ll x){insert(0,x);} void push_back(ll x){insert(size(),x);}
    void erase(int p){erase(p,p+1);}
    void erase(int l,int r){assert(0<=l&&l<=r&&r<=size());replace_nodes(l,r,nullptr);}
    void replace(int l,int r,const string&s){assert(0<=l&&l<=r&&r<=size());replace_nodes(l,r,build_nodes(vals(s)));}
    template<class T> void replace(int l,int r,const vector<T>&v){assert(0<=l&&l<=r&&r<=size());replace_nodes(l,r,build_nodes(vals(v)));}

    void range_assign(int l,int r,ll x){assert(0<=l&&l<=r&&r<=size());Node *a,*b,*c;split3(root,l,r,a,b,c);apply_assign(b,x);root=merge(merge(a,b),c);}
    void range_assign(int l,int r,char c){range_assign(l,r,(ll)(unsigned char)c);}
    void reverse(int l,int r){assert(0<=l&&l<=r&&r<=size());Node *a,*b,*c;split3(root,l,r,a,b,c);apply_rev(b);root=merge(merge(a,b),c);}
    void rotate(int l,int r,long long k){
        assert(0<=l&&l<=r&&r<=size()); int n=r-l; if(n<=1)return; k%=n;if(k<0)k+=n;if(!k)return;
        Node *a,*b,*c,*x,*y; split3(root,l,r,a,b,c); split(b,n-(int)k,x,y); root=merge(merge(a,merge(y,x)),c);
    }
    void cut_and_paste(int l,int r,int p){
        assert(0<=l&&l<=r&&r<=size()); int n=r-l; assert(0<=p&&p<=size()-n);
        Node *a,*m,*c; split3(root,l,r,a,m,c); root=merge(a,c); Node*b;split(root,p,a,b);root=merge(merge(a,m),b);
    }
    void swap_ranges(int l1,int r1,int l2,int r2){
        assert(0<=l1&&l1<=r1&&r1<=l2&&l2<=r2&&r2<=size()); Node *a,*x,*b,*y,*c,*t;
        split(root,l1,a,t);split(t,r1-l1,x,t);split(t,l2-r1,b,t);split(t,r2-l2,y,c);root=merge(merge(merge(merge(a,y),b),x),c);
    }

    Hash prod(int l,int r){ assert(0<=l&&l<=r&&r<=size()); Node *a,*b,*c;split3(root,l,r,a,b,c);Hash h=hs(b);root=merge(merge(a,b),c);return h; }
    Hash all_prod()const{return hs(root);} array<ull,2> hash(int l,int r){return prod(l,r).f;} array<ull,2> rev_hash(int l,int r){return prod(l,r).r;}
    bool equal(int l1,int r1,int l2,int r2){return r1-l1==r2-l2&&same(prod(l1,r1),prod(l2,r2));}
    bool reverse_equal(int l1,int r1,int l2,int r2){return r1-l1==r2-l2&&same_reverse(prod(l1,r1),prod(l2,r2));}
    bool is_palindrome(int l,int r){auto h=prod(l,r);return h.f==h.r;}
    bool equal_with(DynamicRollingHash&o,int l1,int r1,int l2,int r2){return r1-l1==r2-l2&&same(prod(l1,r1),o.prod(l2,r2));}

    int lcp_with(DynamicRollingHash&o,int l1,int r1,int l2,int r2){
        int ok=0,ng=min(r1-l1,r2-l2)+1; while(ng-ok>1){int m=(ok+ng)/2;(same(prod(l1,l1+m),o.prod(l2,l2+m))?ok:ng)=m;} return ok;
    }
    int lcp(int l1,int r1,int l2,int r2){return lcp_with(*this,l1,r1,l2,r2);}
    int lcs_with(DynamicRollingHash&o,int l1,int r1,int l2,int r2){
        int ok=0,ng=min(r1-l1,r2-l2)+1; while(ng-ok>1){int m=(ok+ng)/2;(same(prod(r1-m,r1),o.prod(r2-m,r2))?ok:ng)=m;} return ok;
    }
    int lcs(int l1,int r1,int l2,int r2){return lcs_with(*this,l1,r1,l2,r2);}
    int lcp_reverse_with(DynamicRollingHash&o,int l1,int r1,int l2,int r2){
        int ok=0,ng=min(r1-l1,r2-l2)+1; while(ng-ok>1){int m=(ok+ng)/2;(same_reverse(prod(l1,l1+m),o.prod(r2-m,r2))?ok:ng)=m;} return ok;
    }
    int lcp_reverse(int l1,int r1,int l2,int r2){return lcp_reverse_with(*this,l1,r1,l2,r2);}
    int first_diff_with(DynamicRollingHash&o,int l1,int r1,int l2,int r2){int k=lcp_with(o,l1,r1,l2,r2),m=min(r1-l1,r2-l2);return k<m?k:(r1-l1==r2-l2?-1:m);}
    int first_diff(int l1,int r1,int l2,int r2){return first_diff_with(*this,l1,r1,l2,r2);}
    int compare_with(DynamicRollingHash&o,int l1,int r1,int l2,int r2){
        int n1=r1-l1,n2=r2-l2,k=lcp_with(o,l1,r1,l2,r2),m=min(n1,n2); if(k==m)return n1==n2?0:(n1<n2?-1:1); return get(l1+k)<o.get(l2+k)?-1:1;
    }
    int compare(int l1,int r1,int l2,int r2){return compare_with(*this,l1,r1,l2,r2);} bool less(int l1,int r1,int l2,int r2){return compare(l1,r1,l2,r2)<0;}

    int pal_radius_odd(int c){
        assert(0<=c&&c<size()); int ok=0,ng=min(c,size()-1-c)+1; while(ng-ok>1){int m=(ok+ng)/2;(is_palindrome(c-m,c+m+1)?ok:ng)=m;} return ok;
    }
    int pal_radius_even(int c){
        assert(0<=c&&c<=size()); int ok=0,ng=min(c,size()-c)+1; while(ng-ok>1){int m=(ok+ng)/2;(is_palindrome(c-m,c+m)?ok:ng)=m;} return ok;
    }
    pair<int,int> max_palindrome_odd(int c){int d=pal_radius_odd(c);return{c-d,c+d+1};}
    pair<int,int> max_palindrome_even(int c){int d=pal_radius_even(c);return{c-d,c+d};}

    bool has_period(int l,int r,int p){assert(0<=l&&l<=r&&r<=size());return p>0&&(p>=r-l||equal(l,r-p,l+p,r));}
    bool is_repeat(int l,int r,int p){return p>0&&(r-l)%p==0&&has_period(l,r,p);}

    Hash after_erase(int l,int r){return concat(prod(0,l),prod(r,size()));}
    Hash after_insert(int p,const Hash&h){return concat_all({prod(0,p),h,prod(p,size())});}
    Hash after_replace(int l,int r,const Hash&h){return concat_all({prod(0,l),h,prod(r,size())});}
    Hash after_reverse(int l,int r){return concat_all({prod(0,l),reversed(prod(l,r)),prod(r,size())});}

    vector<ll> to_vector(){vector<ll>v;v.reserve(size());dump(root,v);return v;}
    string to_string_chars(){auto v=to_vector();string s;s.reserve(v.size());for(ll x:v)s.push_back((char)x);return s;}
};

/*
DynamicRollingHash rh(s) string sで構築 O(N)
DynamicRollingHash rh(a) vector<T> aで構築 O(N)
int rh.size() 現在の列長を返す O(1)
bool rh.empty() 列が空か判定 O(1)
ll rh.get(i) A[i]を返す 期待O(logN)
void rh.set(i,x) A[i]=xに更新 期待O(logN)
void rh.clear() 列を空にする O(N)
void rh.assign(s/v) string/vectorで再構築 O(N)

void rh.insert(i,x) iの直前にxを挿入 期待O(logN)
void rh.insert(i,s/v) iの直前にstring/vectorを挿入 期待O(logN+|s/v|)
void rh.push_front(x) 先頭にxを挿入 期待O(logN)
void rh.push_back(x) 末尾にxを挿入 期待O(logN)
void rh.erase(i) A[i]を削除 期待O(logN)
void rh.erase(l,r) [l,r)を削除 期待O(logN+(r-l))
void rh.replace(l,r,s/v) [l,r)をstring/vectorに置換 期待O(logN+(r-l)+|s/v|)

void rh.range_assign(l,r,x) [l,r)をすべてxに変更 期待O(logN)
void rh.reverse(l,r) [l,r)を反転 期待O(logN)
void rh.rotate(l,r,k) [l,r)を右にk回転、k<0なら左回転 期待O(logN)
void rh.cut_and_paste(l,r,pos) [l,r)を切り取り削除後のposへ挿入 期待O(logN)
void rh.swap_ranges(l1,r1,l2,r2) 非交差な2区間を交換(r1<=l2) 期待O(logN)

Hash rh.prod(l,r) [l,r)の双方向二重Hashを返す 期待O(logN)
Hash rh.all_prod() 列全体の双方向二重Hashを返す O(1)
array<ull,2> rh.hash(l,r) [l,r)の順方向二重Hashを返す 期待O(logN)
array<ull,2> rh.rev_hash(l,r) reverse([l,r))の二重Hashを返す 期待O(logN)

bool rh.equal(l1,r1,l2,r2) 2区間が一致するか判定 期待O(logN)
bool rh.reverse_equal(l1,r1,l2,r2) 左区間と右区間の反転が一致するか判定 期待O(logN)
bool rh.is_palindrome(l,r) [l,r)が回文か判定 期待O(logN)
int rh.lcp(l1,r1,l2,r2) 2区間のLCP長を返す 期待O(log^2N)
int rh.lcs(l1,r1,l2,r2) 2区間のLCS長を返す 期待O(log^2N)
int rh.lcp_reverse(l1,r1,l2,r2) 左区間と右区間の反転のLCP長を返す 期待O(log^2N)
int rh.first_diff(l1,r1,l2,r2) 最初の不一致offset、完全一致なら-1 期待O(log^2N)
int rh.compare(l1,r1,l2,r2) 辞書順比較して-1/0/1を返す 期待O(log^2N)
bool rh.less(l1,r1,l2,r2) 左区間が辞書順で小さいか判定 期待O(log^2N)

bool rh.equal_with(other,l1,r1,l2,r2) rhとotherの区間一致判定 期待O(logN)
int rh.lcp_with(other,l1,r1,l2,r2) rhとotherのLCP長を返す 期待O(log^2N)
int rh.lcs_with(other,l1,r1,l2,r2) rhとotherのLCS長を返す 期待O(log^2N)
int rh.lcp_reverse_with(other,l1,r1,l2,r2) rh区間とother区間反転のLCP長を返す 期待O(log^2N)
int rh.first_diff_with(other,l1,r1,l2,r2) rhとotherの最初の不一致offset、完全一致なら-1 期待O(log^2N)
int rh.compare_with(other,l1,r1,l2,r2) rhとotherを辞書順比較して-1/0/1を返す 期待O(log^2N)

int rh.pal_radius_odd(c) c中心の最大奇数回文[c-d,c+d+1)のdを返す 期待O(log^2N)
int rh.pal_radius_even(c) c-1,c間中心の最大偶数回文[c-d,c+d)のdを返す 期待O(log^2N)
pair<int,int> rh.max_palindrome_odd(c) 最大奇数回文区間{l,r}を返す 期待O(log^2N)
pair<int,int> rh.max_palindrome_even(c) 最大偶数回文区間{l,r}を返す 期待O(log^2N)

bool rh.has_period(l,r,p) [l,r)が周期pを持つか判定 期待O(logN)
bool rh.is_repeat(l,r,p) [l,r)が長さpのブロックの完全反復か判定 期待O(logN)

Hash rh.single(x) 1要素[x]のHashを返す O(1)
Hash rh.concat(a,b) A+BのHashを返す O(1)
Hash rh.concat_all(hs) 複数Hashを順に連結したHashを返す O(Hash数)
Hash rh.reversed(h) reverse(A)のHashを返す O(1)
Hash rh.repeat(h,k) Aをk回連結したHashを返す O(logK)
bool rh.same(a,b) 2つのHashが一致するか判定 O(1)
bool rh.same_reverse(a,b) A==reverse(B)か判定 O(1)

Hash rh.after_erase(l,r) [l,r)削除後の仮想Hashを返す 期待O(logN)
Hash rh.after_insert(pos,h) posにh挿入後の仮想Hashを返す 期待O(logN)
Hash rh.after_replace(l,r,h) [l,r)をhに置換後の仮想Hashを返す 期待O(logN)
Hash rh.after_reverse(l,r) [l,r)反転後の仮想Hashを返す 期待O(logN)

vector<ll> rh.to_vector() 現在の列をvector<ll>で返す O(N)
string rh.to_string_chars() 各値をcharとしてstringで返す O(N)

区間はすべて[l,r)、MOD=2^61-1、base2本+salt2本+Treap priorityを実行時ランダム生成
*/
