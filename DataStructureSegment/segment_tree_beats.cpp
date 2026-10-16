struct SegBeats{
    static constexpr ll INF=(1LL<<60);

    struct Node{
        ll sum=0,mx1=-INF,mx2=-INF,mn1=INF,mn2=INF,add=0,setv=0;
        int mxc=0,mnc=0,len=0,mxid=INT_MAX,mnid=INT_MAX,lidx=INT_MAX;
        bool has_set=false;
    };

    int n=0;
    vector<Node> st;

    SegBeats()=default;
    SegBeats(int n):SegBeats(vector<ll>(n)){}
    SegBeats(const vector<ll>&a){init(a);}

private:
    static Node leaf(ll x,int i){
        Node t;
        t.sum=t.mx1=t.mn1=x;
        t.mxc=t.mnc=t.len=1;
        t.mxid=t.mnid=t.lidx=i;
        return t;
    }

    static Node merge(const Node&a,const Node&b){
        if(!a.len)return b;
        if(!b.len)return a;

        Node t;
        t.len=a.len+b.len;
        t.lidx=std::min(a.lidx,b.lidx);
        t.sum=a.sum+b.sum;

        if(a.mx1>b.mx1) t.mx1=a.mx1,t.mxc=a.mxc,t.mxid=a.mxid,t.mx2=std::max(a.mx2,b.mx1);
        else if(a.mx1<b.mx1) t.mx1=b.mx1,t.mxc=b.mxc,t.mxid=b.mxid,t.mx2=std::max(a.mx1,b.mx2);
        else t.mx1=a.mx1,t.mxc=a.mxc+b.mxc,t.mxid=std::min(a.mxid,b.mxid),t.mx2=std::max(a.mx2,b.mx2);

        if(a.mn1<b.mn1) t.mn1=a.mn1,t.mnc=a.mnc,t.mnid=a.mnid,t.mn2=std::min(a.mn2,b.mn1);
        else if(a.mn1>b.mn1) t.mn1=b.mn1,t.mnc=b.mnc,t.mnid=b.mnid,t.mn2=std::min(a.mn1,b.mn2);
        else t.mn1=a.mn1,t.mnc=a.mnc+b.mnc,t.mnid=std::min(a.mnid,b.mnid),t.mn2=std::min(a.mn2,b.mn2);

        return t;
    }

    void build(int p,int l,int r,const vector<ll>&a){
        if(r-l==1){st[p]=leaf(a[l],l);return;}
        int m=(l+r)/2;
        build(p*2,l,m,a);build(p*2+1,m,r,a);pull(p);
    }

    void pull(int p){st[p]=merge(st[p*2],st[p*2+1]);}

    void tag_set(int p,ll x){
        auto&t=st[p];
        t.sum=x*t.len;
        t.mx1=t.mn1=x;t.mx2=-INF;t.mn2=INF;
        t.mxc=t.mnc=t.len;t.mxid=t.mnid=t.lidx;
        t.add=0;t.setv=x;t.has_set=true;
    }

    void tag_add(int p,ll x){
        auto&t=st[p];
        t.sum+=x*t.len;t.mx1+=x;t.mn1+=x;
        if(t.mx2!=-INF)t.mx2+=x;
        if(t.mn2!=INF)t.mn2+=x;
        if(t.has_set)t.setv+=x;
        else t.add+=x;
    }

    void tag_chmin(int p,ll x){
        auto&t=st[p];
        if(t.mx1<=x)return;

        ll old=t.mx1;
        t.sum+=(x-old)*t.mxc;

        if(t.mn1==old)t.mn1=x,t.mnid=t.lidx;
        else if(t.mn2==old)t.mn2=x;

        t.mx1=x;
        if(t.mn1==t.mx1)t.mxid=t.mnid=t.lidx;
        if(t.has_set)t.setv=x;
    }

    void tag_chmax(int p,ll x){
        auto&t=st[p];
        if(t.mn1>=x)return;

        ll old=t.mn1;
        t.sum+=(x-old)*t.mnc;

        if(t.mx1==old)t.mx1=x,t.mxid=t.lidx;
        else if(t.mx2==old)t.mx2=x;

        t.mn1=x;
        if(t.mn1==t.mx1)t.mxid=t.mnid=t.lidx;
        if(t.has_set)t.setv=x;
    }

    void push(int p){
        if(st[p].has_set){
            tag_set(p*2,st[p].setv);
            tag_set(p*2+1,st[p].setv);
            st[p].has_set=false;
        }

        if(st[p].add){
            ll x=st[p].add;
            tag_add(p*2,x);
            tag_add(p*2+1,x);
            st[p].add=0;
        }

        if(st[p*2].mx1>st[p].mx1)tag_chmin(p*2,st[p].mx1);
        if(st[p*2+1].mx1>st[p].mx1)tag_chmin(p*2+1,st[p].mx1);
        if(st[p*2].mn1<st[p].mn1)tag_chmax(p*2,st[p].mn1);
        if(st[p*2+1].mn1<st[p].mn1)tag_chmax(p*2+1,st[p].mn1);
    }

    void chmin_(int p,int l,int r,int ql,int qr,ll x){
        if(qr<=l||r<=ql||st[p].mx1<=x)return;
        if(ql<=l&&r<=qr&&st[p].mx2<x){tag_chmin(p,x);return;}

        push(p);
        int m=(l+r)/2;

        chmin_(p*2,l,m,ql,qr,x);
        chmin_(p*2+1,m,r,ql,qr,x);
        pull(p);
    }

    void chmax_(int p,int l,int r,int ql,int qr,ll x){
        if(qr<=l||r<=ql||st[p].mn1>=x)return;
        if(ql<=l&&r<=qr&&st[p].mn2>x){tag_chmax(p,x);return;}

        push(p);
        int m=(l+r)/2;

        chmax_(p*2,l,m,ql,qr,x);
        chmax_(p*2+1,m,r,ql,qr,x);
        pull(p);
    }

    void add_(int p,int l,int r,int ql,int qr,ll x){
        if(qr<=l||r<=ql)return;
        if(ql<=l&&r<=qr){tag_add(p,x);return;}

        push(p);
        int m=(l+r)/2;

        add_(p*2,l,m,ql,qr,x);
        add_(p*2+1,m,r,ql,qr,x);
        pull(p);
    }

    void assign_(int p,int l,int r,int ql,int qr,ll x){
        if(qr<=l||r<=ql)return;
        if(ql<=l&&r<=qr){tag_set(p,x);return;}

        push(p);
        int m=(l+r)/2;

        assign_(p*2,l,m,ql,qr,x);
        assign_(p*2+1,m,r,ql,qr,x);
        pull(p);
    }

    Node prod_(int p,int l,int r,int ql,int qr){
        if(qr<=l||r<=ql)return {};
        if(ql<=l&&r<=qr)return st[p];

        push(p);
        int m=(l+r)/2;

        return merge(
            prod_(p*2,l,m,ql,qr),
            prod_(p*2+1,m,r,ql,qr)
        );
    }

    int first_ge_(int p,int l,int r,int ql,int qr,ll x){
        if(qr<=l||r<=ql||st[p].mx1<x)return -1;
        if(r-l==1)return l;

        push(p);
        int m=(l+r)/2,z=first_ge_(p*2,l,m,ql,qr,x);

        return z!=-1?z:first_ge_(p*2+1,m,r,ql,qr,x);
    }

    int last_ge_(int p,int l,int r,int ql,int qr,ll x){
        if(qr<=l||r<=ql||st[p].mx1<x)return -1;
        if(r-l==1)return l;

        push(p);
        int m=(l+r)/2,z=last_ge_(p*2+1,m,r,ql,qr,x);

        return z!=-1?z:last_ge_(p*2,l,m,ql,qr,x);
    }

    int first_le_(int p,int l,int r,int ql,int qr,ll x){
        if(qr<=l||r<=ql||st[p].mn1>x)return -1;
        if(r-l==1)return l;

        push(p);
        int m=(l+r)/2,z=first_le_(p*2,l,m,ql,qr,x);

        return z!=-1?z:first_le_(p*2+1,m,r,ql,qr,x);
    }

    int last_le_(int p,int l,int r,int ql,int qr,ll x){
        if(qr<=l||r<=ql||st[p].mn1>x)return -1;
        if(r-l==1)return l;

        push(p);
        int m=(l+r)/2,z=last_le_(p*2+1,m,r,ql,qr,x);

        return z!=-1?z:last_le_(p*2,l,m,ql,qr,x);
    }

public:
    void init(const vector<ll>&a){
        n=a.size();
        st.assign(std::max(1,4*n),{});
        if(n)build(1,0,n,a);
    }

    int size()const{return n;}
    bool empty()const{return !n;}

    void chmin(int l,int r,ll x){if(l<r)chmin_(1,0,n,l,r,x);}
    void chmax(int l,int r,ll x){if(l<r)chmax_(1,0,n,l,r,x);}
    void add(int l,int r,ll x){if(l<r)add_(1,0,n,l,r,x);}
    void assign(int l,int r,ll x){if(l<r)assign_(1,0,n,l,r,x);}
    void clamp(int l,int r,ll lo,ll hi){do { if (!(lo<=hi)) std::abort(); } while (false);chmax(l,r,lo);chmin(l,r,hi);}
    void set(int i,ll x){assign(i,i+1,x);}

    Node prod(int l,int r){return l<r?prod_(1,0,n,l,r):Node{};}
    Node all_prod()const{return n?st[1]:Node{};}

    ll get(int i){return sum(i,i+1);}
    ll sum(int l,int r){return prod(l,r).sum;}
    ll min(int l,int r){return prod(l,r).mn1;}
    ll max(int l,int r){return prod(l,r).mx1;}
    ll second_min(int l,int r){return prod(l,r).mn2;}
    ll second_max(int l,int r){return prod(l,r).mx2;}
    int count_min(int l,int r){return prod(l,r).mnc;}
    int count_max(int l,int r){return prod(l,r).mxc;}

    pair<ll,int> argmin(int l,int r){
        auto t=prod(l,r);
        return {t.mn1,t.mnid};
    }

    pair<ll,int> argmax(int l,int r){
        auto t=prod(l,r);
        return {t.mx1,t.mxid};
    }

    int first_ge(int l,int r,ll x){return first_ge_(1,0,n,l,r,x);}
    int last_ge(int l,int r,ll x){return last_ge_(1,0,n,l,r,x);}
    int first_le(int l,int r,ll x){return first_le_(1,0,n,l,r,x);}
    int last_le(int l,int r,ll x){return last_le_(1,0,n,l,r,x);}

    ll all_sum()const{return n?st[1].sum:0;}
    ll all_min()const{return n?st[1].mn1:INF;}
    ll all_max()const{return n?st[1].mx1:-INF;}
};

/*
細かい注意点
0-indexed、区間は[l,r)。中間演算と区間和はlong longに収まること。
chmin/chmax/clampは償却評価。他の区間操作や検索はO(log N)。
prodのNodeはsum,mx1,mx2,mn1,mn2,mxc,mncなどを持つ。
argmin/argmaxは同値なら最左位置。第2値がなければ±INF。

使える関数一覧
SegBeats seg(v) chmin/chmax/add/assign + sum/min/maxを管理 O(N)
void seg.chmin(l,r,x) [l,r)をmin(A[i],x)に更新 償却O(log^2N)
void seg.chmax(l,r,x) [l,r)をmax(A[i],x)に更新 償却O(log^2N)
void seg.add(l,r,x) [l,r)にx加算 O(logN)
void seg.assign(l,r,x) [l,r)をxに代入 O(logN)
void seg.clamp(l,r,lo,hi) [l,r)を[lo,hi]に丸める 償却O(log^2N)
void seg.set(i,x) A[i]=xに変更 O(logN)

ll seg.get(i) A[i]を返す O(logN)
Node seg.prod(l,r) [l,r)の全情報を返す O(logN)
ll seg.sum(l,r) [l,r)の和を返す O(logN)
ll seg.min(l,r) [l,r)の最小値を返す O(logN)
ll seg.max(l,r) [l,r)の最大値を返す O(logN)
ll seg.second_min(l,r) 第2最小値、無ければINF O(logN)
ll seg.second_max(l,r) 第2最大値、無ければ-INF O(logN)
int seg.count_min(l,r) 最小値の個数を返す O(logN)
int seg.count_max(l,r) 最大値の個数を返す O(logN)
pair<ll,int> seg.argmin(l,r) {最小値,最左index}を返す O(logN)
pair<ll,int> seg.argmax(l,r) {最大値,最左index}を返す O(logN)

int seg.first_ge(l,r,x) 最左のA[i]>=xを返す、無ければ-1 O(logN)
int seg.last_ge(l,r,x) 最右のA[i]>=xを返す、無ければ-1 O(logN)
int seg.first_le(l,r,x) 最左のA[i]<=xを返す、無ければ-1 O(logN)
int seg.last_le(l,r,x) 最右のA[i]<=xを返す、無ければ-1 O(logN)

Node seg.all_prod() 全体の全情報を返す O(1)
ll seg.all_sum() 全体の和を返す O(1)
ll seg.all_min() 全体の最小値を返す O(1)
ll seg.all_max() 全体の最大値を返す O(1)

基本の関数
SegBeats seg(vl{1,5,3});
seg.chmin(0,3,4);                             // {1,4,3}
seg.add(0,2,2);                               // {3,6,3}
auto result = seg.prod(0,3);                  // 全集約情報
auto sum = seg.sum(0,3);                      // 12
*/
