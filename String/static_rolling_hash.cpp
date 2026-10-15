//StaticRollingHash


struct StaticRollingHash {
    using ll=long long;
    using ull=unsigned long long;
    using u128=__uint128_t;
    static constexpr ull MOD=(1ULL<<61)-1;

    struct Hash {
        array<ull,2> f{},r{},pw{1,1};
        int len=0;
    };

private:
    struct Params { array<ull,2> base,salt; };

    static ull sm(ull x){
        x+=0x9e3779b97f4a7c15ULL;
        x=(x^(x>>30))*0xbf58476d1ce4e5b9ULL;
        x=(x^(x>>27))*0x94d049bb133111ebULL;
        return x^(x>>31);
    }

    static const Params& par(){
        static Params p=[]{
            ull x=chrono::steady_clock::now().time_since_epoch().count();
            x^=(ull)(uintptr_t)&x;
            random_device rd;
            x^=((ull)rd()<<32)^rd();
            Params p;
            for(int k=0;k<2;k++){
                x=sm(x);
                p.base[k]=1000000007ULL+x%(MOD-2000000013ULL);
                x=sm(x);
                p.salt[k]=x;
            }
            if(p.base[0]==p.base[1]) p.base[1]++;
            return p;
        }();
        return p;
    }

    static ull add(ull a,ull b){ a+=b; return a>=MOD?a-MOD:a; }
    static ull sub(ull a,ull b){ return a>=b?a-b:a+MOD-b; }
    static ull mul(ull a,ull b){
        u128 z=(u128)a*b;
        ull x=(ull)(z&MOD)+(ull)(z>>61);
        x=(x&MOD)+(x>>61);
        return x>=MOD?x-MOD:x;
    }
    static ull enc(ll x,int k){ return sm((ull)x^par().salt[k])%(MOD-1)+1; }

    int n=0;
    vector<ll> a;
    array<vector<ull>,2> pw,pre,rpre;

    template<class T>
    void build(const vector<T>& v){
        n=v.size(); a.resize(n);
        for(int i=0;i<n;i++) a[i]=(ll)v[i];

        for(int k=0;k<2;k++){
            pw[k].assign(n+1,1);
            pre[k].assign(n+1,0);
            rpre[k].assign(n+1,0);
            for(int i=0;i<n;i++){
                pw[k][i+1]=mul(pw[k][i],par().base[k]);
                pre[k][i+1]=add(mul(pre[k][i],par().base[k]),enc(a[i],k));
                rpre[k][i+1]=add(mul(rpre[k][i],par().base[k]),enc(a[n-1-i],k));
            }
        }
    }

    ull geth(int l,int r,int k) const {
        return sub(pre[k][r],mul(pre[k][l],pw[k][r-l]));
    }
    ull getrh(int l,int r,int k) const {
        return sub(rpre[k][n-l],mul(rpre[k][n-r],pw[k][r-l]));
    }

public:
    StaticRollingHash()=default;

    StaticRollingHash(const string& s){
        vector<ll> v(s.size());
        for(int i=0;i<(int)s.size();i++) v[i]=(unsigned char)s[i];
        build(v);
    }

    template<class T>
    StaticRollingHash(const vector<T>& v){ build(v); }

    int size() const { return n; }
    bool empty() const { return n==0; }
    ll get(int i) const { return a[i]; }

    static Hash single(ll x){
        Hash h; h.len=1;
        for(int k=0;k<2;k++)
            h.f[k]=h.r[k]=enc(x,k),h.pw[k]=par().base[k];
        return h;
    }

    static Hash concat(const Hash& a,const Hash& b){
        Hash c; c.len=a.len+b.len;
        for(int k=0;k<2;k++){
            c.f[k]=add(mul(a.f[k],b.pw[k]),b.f[k]);
            c.r[k]=add(mul(b.r[k],a.pw[k]),a.r[k]);
            c.pw[k]=mul(a.pw[k],b.pw[k]);
        }
        return c;
    }

    static Hash reversed(Hash h){ swap(h.f,h.r); return h; }

    static Hash repeat(Hash h,long long k){
        Hash r;
        while(k){
            if(k&1) r=concat(r,h);
            h=concat(h,h); k>>=1;
        }
        return r;
    }

    static bool same(const Hash& a,const Hash& b){ return a.len==b.len&&a.f==b.f; }
    static bool same_reverse(const Hash& a,const Hash& b){ return a.len==b.len&&a.f==b.r; }

    Hash prod(int l,int r) const {
        Hash h; h.len=r-l;
        for(int k=0;k<2;k++){
            h.f[k]=geth(l,r,k);
            h.r[k]=getrh(l,r,k);
            h.pw[k]=pw[k][r-l];
        }
        return h;
    }

    Hash all_prod() const { return prod(0,n); }
    array<ull,2> hash(int l,int r) const { return prod(l,r).f; }
    array<ull,2> rev_hash(int l,int r) const { return prod(l,r).r; }

    bool equal(int l1,int r1,int l2,int r2) const {
        return r1-l1==r2-l2&&hash(l1,r1)==hash(l2,r2);
    }

    bool reverse_equal(int l1,int r1,int l2,int r2) const {
        return r1-l1==r2-l2&&hash(l1,r1)==rev_hash(l2,r2);
    }

    bool is_palindrome(int l,int r) const {
        auto h=prod(l,r);
        return h.f==h.r;
    }

    bool equal_with(const StaticRollingHash& o,int l1,int r1,int l2,int r2) const {
        return r1-l1==r2-l2&&hash(l1,r1)==o.hash(l2,r2);
    }

    int lcp_with(const StaticRollingHash& o,int l1,int r1,int l2,int r2) const {
        int ok=0,ng=min(r1-l1,r2-l2)+1;
        while(ng-ok>1){
            int m=(ok+ng)/2;
            (hash(l1,l1+m)==o.hash(l2,l2+m)?ok:ng)=m;
        }
        return ok;
    }

    int lcp(int l1,int r1,int l2,int r2) const {
        return lcp_with(*this,l1,r1,l2,r2);
    }

    int lcs_with(const StaticRollingHash& o,int l1,int r1,int l2,int r2) const {
        int ok=0,ng=min(r1-l1,r2-l2)+1;
        while(ng-ok>1){
            int m=(ok+ng)/2;
            (hash(r1-m,r1)==o.hash(r2-m,r2)?ok:ng)=m;
        }
        return ok;
    }

    int lcs(int l1,int r1,int l2,int r2) const {
        return lcs_with(*this,l1,r1,l2,r2);
    }

    int first_diff(int l1,int r1,int l2,int r2) const {
        int k=lcp(l1,r1,l2,r2),mn=min(r1-l1,r2-l2);
        if(k<mn) return k;
        return r1-l1==r2-l2?-1:mn;
    }

    int compare(int l1,int r1,int l2,int r2) const {
        int k=lcp(l1,r1,l2,r2),n1=r1-l1,n2=r2-l2,mn=min(n1,n2);
        if(k==mn) return n1==n2?0:(n1<n2?-1:1);
        return a[l1+k]<a[l2+k]?-1:1;
    }

    bool less(int l1,int r1,int l2,int r2) const {
        return compare(l1,r1,l2,r2)<0;
    }

    int pal_radius_odd(int c) const {
        int ok=0,ng=min(c,n-1-c)+1;
        while(ng-ok>1){
            int m=(ok+ng)/2;
            (is_palindrome(c-m,c+m+1)?ok:ng)=m;
        }
        return ok;
    }

    int pal_radius_even(int c) const {
        int ok=0,ng=min(c,n-c)+1;
        while(ng-ok>1){
            int m=(ok+ng)/2;
            (is_palindrome(c-m,c+m)?ok:ng)=m;
        }
        return ok;
    }

    pair<int,int> max_palindrome_odd(int c) const {
        int d=pal_radius_odd(c); return {c-d,c+d+1};
    }

    pair<int,int> max_palindrome_even(int c) const {
        int d=pal_radius_even(c); return {c-d,c+d};
    }

    bool has_period(int l,int r,int p) const {
        return p>0&&(p>=r-l||equal(l,r-p,l+p,r));
    }

    bool is_repeat(int l,int r,int p) const {
        return p>0&&(r-l)%p==0&&has_period(l,r,p);
    }

    Hash after_erase(int l,int r) const {
        return concat(prod(0,l),prod(r,n));
    }

    Hash after_insert(int p,const Hash& h) const {
        return concat(concat(prod(0,p),h),prod(p,n));
    }

    Hash after_replace(int l,int r,const Hash& h) const {
        return concat(concat(prod(0,l),h),prod(r,n));
    }

    Hash after_reverse(int l,int r) const {
        return concat(concat(prod(0,l),reversed(prod(l,r))),prod(r,n));
    }
};

/*
StaticRollingHash rh(s) string sで構築 O(N)
StaticRollingHash rh(a) vector<T> aで構築 O(N)
int rh.size() 列長を返す O(1)
ll rh.get(i) A[i]を返す O(1)

Hash rh.prod(l,r) [l,r)の双方向二重Hashを返す O(1)
Hash rh.all_prod() 列全体のHashを返す O(1)
array<ull,2> rh.hash(l,r) [l,r)の順方向Hashを返す O(1)
array<ull,2> rh.rev_hash(l,r) reverse([l,r))のHashを返す O(1)

bool rh.equal(l1,r1,l2,r2) 2区間が一致するか判定 O(1)
bool rh.reverse_equal(l1,r1,l2,r2) 左区間と右区間の反転が一致するか判定 O(1)
bool rh.is_palindrome(l,r) [l,r)が回文か判定 O(1)
int rh.lcp(l1,r1,l2,r2) 2区間のLCP長を返す O(logN)
int rh.lcs(l1,r1,l2,r2) 2区間のLCS長を返す O(logN)
int rh.first_diff(l1,r1,l2,r2) 最初の不一致offset、完全一致なら-1 O(logN)
int rh.compare(l1,r1,l2,r2) 辞書順比較して-1/0/1を返す O(logN)
bool rh.less(l1,r1,l2,r2) 左区間が辞書順で小さいか判定 O(logN)

bool rh.equal_with(other,l1,r1,l2,r2) rhとotherの区間一致判定 O(1)
int rh.lcp_with(other,l1,r1,l2,r2) rhとotherのLCP長を返す O(logN)
int rh.lcs_with(other,l1,r1,l2,r2) rhとotherのLCS長を返す O(logN)

int rh.pal_radius_odd(c) c中心の最大奇数回文半径dを返す O(logN)
int rh.pal_radius_even(c) c-1,c間中心の最大偶数回文半径dを返す O(logN)
pair<int,int> rh.max_palindrome_odd(c) 最大奇数回文区間{l,r}を返す O(logN)
pair<int,int> rh.max_palindrome_even(c) 最大偶数回文区間{l,r}を返す O(logN)

bool rh.has_period(l,r,p) [l,r)が周期pを持つか判定 O(1)
bool rh.is_repeat(l,r,p) 長さpのブロックの完全な繰り返しか判定 O(1)

Hash rh.single(x) 1要素[x]のHashを返す O(1)
Hash rh.concat(a,b) A+BのHashを返す O(1)
Hash rh.reversed(h) reverse(A)のHashを返す O(1)
Hash rh.repeat(h,k) Aをk回連結したHashを返す O(logK)
bool rh.same(a,b) 2つのHashが一致するか判定 O(1)
bool rh.same_reverse(a,b) A==reverse(B)か判定 O(1)

Hash rh.after_erase(l,r) [l,r)削除後の仮想Hashを返す O(1)
Hash rh.after_insert(pos,h) posにh挿入後の仮想Hashを返す O(1)
Hash rh.after_replace(l,r,h) [l,r)をhに置換後の仮想Hashを返す O(1)
Hash rh.after_reverse(l,r) [l,r)反転後の仮想Hashを返す O(1)

区間はすべて[l,r)、MOD=2^61-1、base2本+salt2本を実行時ランダム生成
*/