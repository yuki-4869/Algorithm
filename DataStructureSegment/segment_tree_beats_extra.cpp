namespace beats_extra_detail {
    inline int check_size(std::size_t n){
        if(n>static_cast<std::size_t>(std::numeric_limits<int>::max()/4))
            std::abort();
        return static_cast<int>(n);
    }
    inline std::vector<long long> zeros(int n){
        if(n<0)std::abort();
        check_size(static_cast<std::size_t>(n));
        return std::vector<long long>(static_cast<std::size_t>(n),0);
    }
    inline void point(int n,int i){if(i<0||i>=n)std::abort();}
    inline void range(int n,int l,int r){if(l<0||l>r||r>n)std::abort();}
    inline void nonnegative(long long x){if(x<0)std::abort();}
    inline void positive(long long x){if(x<=0)std::abort();}
}

struct SegMod{
    static constexpr long long INF=std::numeric_limits<long long>::max();
    struct Node{long long sum=0,mn=INF,mx=-INF;int len=0;};

private:
    int n=0;
    std::vector<Node> st;
public:

    SegMod()=default;
    SegMod(const SegMod&)=default;
    SegMod& operator=(const SegMod&)=default;
    SegMod(SegMod&& other) noexcept:n(std::exchange(other.n,0)),st(std::move(other.st)){}
    SegMod& operator=(SegMod&& other) noexcept{
        if(this!=&other){n=std::exchange(other.n,0);st=std::move(other.st);}
        return *this;
    }
    explicit SegMod(int count):SegMod(beats_extra_detail::zeros(count)){}
    SegMod(const std::vector<long long>&a){init(a);}

private:
    static Node merge(Node a,Node b){
        if(!a.len)return b;
        if(!b.len)return a;
        return {a.sum+b.sum,std::min(a.mn,b.mn),std::max(a.mx,b.mx),a.len+b.len};
    }

    void build(int p,int l,int r,const std::vector<long long>&a){
        if(r-l==1){st[p]={a[l],a[l],a[l],1};return;}
        int m=l+(r-l)/2;
        build(p*2,l,m,a);build(p*2+1,m,r,a);
        st[p]=merge(st[p*2],st[p*2+1]);
    }

    void mod_(int p,int l,int r,int ql,int qr,long long x){
        if(qr<=l||r<=ql||st[p].mx<x)return;

        if(r-l==1){
            long long y=st[p].mx%x;
            st[p]={y,y,y,1};
            return;
        }

        int m=l+(r-l)/2;
        mod_(p*2,l,m,ql,qr,x);
        mod_(p*2+1,m,r,ql,qr,x);
        st[p]=merge(st[p*2],st[p*2+1]);
    }

    void set_(int p,int l,int r,int i,long long x){
        if(r-l==1){st[p]={x,x,x,1};return;}

        int m=l+(r-l)/2;
        if(i<m)set_(p*2,l,m,i,x);
        else set_(p*2+1,m,r,i,x);

        st[p]=merge(st[p*2],st[p*2+1]);
    }

    Node prod_(int p,int l,int r,int ql,int qr){
        if(qr<=l||r<=ql)return {};
        if(ql<=l&&r<=qr)return st[p];

        int m=l+(r-l)/2;
        return merge(
            prod_(p*2,l,m,ql,qr),
            prod_(p*2+1,m,r,ql,qr)
        );
    }

public:
    void init(const std::vector<long long>&a){
        beats_extra_detail::check_size(a.size());
        for(long long x:a)beats_extra_detail::nonnegative(x);
        n=static_cast<int>(a.size());

        st.assign(std::max<std::size_t>(1,4*static_cast<std::size_t>(n)),{});
        if(n)build(1,0,n,a);
    }

    int size()const{return n;}
    bool empty()const{return n==0;}
    void clear(){st.clear();n=0;}
    void mod(int l,int r,long long x){beats_extra_detail::range(n,l,r);beats_extra_detail::positive(x);if(l<r)mod_(1,0,n,l,r,x);}
    void set(int i,long long x){beats_extra_detail::point(n,i);beats_extra_detail::nonnegative(x);set_(1,0,n,i,x);}

    Node prod(int l,int r){beats_extra_detail::range(n,l,r);return l==r?Node{}:prod_(1,0,n,l,r);}
    long long get(int i){beats_extra_detail::point(n,i);return sum(i,i+1);}
    long long sum(int l,int r){return prod(l,r).sum;}
    long long min(int l,int r){return prod(l,r).mn;}
    long long max(int l,int r){return prod(l,r).mx;}
    long long all_sum()const{return n?st[1].sum:0;}
};

struct SegDiv{
    static constexpr long long INF=std::numeric_limits<long long>::max();
    struct Node{long long sum=0,mn=INF,mx=-INF,setv=0;int len=0;bool has=false;};

private:
    int n=0;
    std::vector<Node> st;
public:

    SegDiv()=default;
    SegDiv(const SegDiv&)=default;
    SegDiv& operator=(const SegDiv&)=default;
    SegDiv(SegDiv&& other) noexcept:n(std::exchange(other.n,0)),st(std::move(other.st)){}
    SegDiv& operator=(SegDiv&& other) noexcept{
        if(this!=&other){n=std::exchange(other.n,0);st=std::move(other.st);}
        return *this;
    }
    explicit SegDiv(int count):SegDiv(beats_extra_detail::zeros(count)){}
    SegDiv(const std::vector<long long>&a){init(a);}

private:
    static Node merge(Node a,Node b){
        if(!a.len)return b;
        if(!b.len)return a;
        return {a.sum+b.sum,std::min(a.mn,b.mn),std::max(a.mx,b.mx),0,a.len+b.len,false};
    }

    void build(int p,int l,int r,const std::vector<long long>&a){
        if(r-l==1){st[p]={a[l],a[l],a[l],0,1,false};return;}
        int m=l+(r-l)/2;
        build(p*2,l,m,a);build(p*2+1,m,r,a);
        pull(p);
    }

    void pull(int p){st[p]=merge(st[p*2],st[p*2+1]);}

    void tag(int p,long long x){
        auto&t=st[p];
        t.sum=x*t.len;t.mn=t.mx=x;t.setv=x;t.has=true;
    }

    void push(int p){
        if(!st[p].has)return;
        tag(p*2,st[p].setv);
        tag(p*2+1,st[p].setv);
        st[p].has=false;
    }

    void assign_(int p,int l,int r,int ql,int qr,long long x){
        if(qr<=l||r<=ql)return;
        if(ql<=l&&r<=qr){tag(p,x);return;}

        push(p);
        int m=l+(r-l)/2;

        assign_(p*2,l,m,ql,qr,x);
        assign_(p*2+1,m,r,ql,qr,x);
        pull(p);
    }

    void divide_(int p,int l,int r,int ql,int qr,long long d){
        if(qr<=l||r<=ql||st[p].mx==0)return;

        if(ql<=l&&r<=qr&&st[p].mn/d==st[p].mx/d){
            tag(p,st[p].mn/d);
            return;
        }

        push(p);
        int m=l+(r-l)/2;

        divide_(p*2,l,m,ql,qr,d);
        divide_(p*2+1,m,r,ql,qr,d);
        pull(p);
    }

    Node prod_(int p,int l,int r,int ql,int qr){
        if(qr<=l||r<=ql)return {};
        if(ql<=l&&r<=qr)return st[p];

        push(p);
        int m=l+(r-l)/2;

        return merge(
            prod_(p*2,l,m,ql,qr),
            prod_(p*2+1,m,r,ql,qr)
        );
    }

public:
    void init(const std::vector<long long>&a){
        beats_extra_detail::check_size(a.size());
        for(long long x:a)beats_extra_detail::nonnegative(x);
        n=static_cast<int>(a.size());

        st.assign(std::max<std::size_t>(1,4*static_cast<std::size_t>(n)),{});
        if(n)build(1,0,n,a);
    }

    int size()const{return n;}
    bool empty()const{return n==0;}
    void clear(){st.clear();n=0;}
    void divide(int l,int r,long long d){beats_extra_detail::range(n,l,r);beats_extra_detail::positive(d);if(l<r&&d!=1)divide_(1,0,n,l,r,d);}
    void assign(int l,int r,long long x){beats_extra_detail::range(n,l,r);beats_extra_detail::nonnegative(x);if(l<r)assign_(1,0,n,l,r,x);}
    void set(int i,long long x){beats_extra_detail::point(n,i);assign(i,i+1,x);}

    Node prod(int l,int r){beats_extra_detail::range(n,l,r);return l==r?Node{}:prod_(1,0,n,l,r);}
    long long get(int i){beats_extra_detail::point(n,i);return sum(i,i+1);}
    long long sum(int l,int r){return prod(l,r).sum;}
    long long min(int l,int r){return prod(l,r).mn;}
    long long max(int l,int r){return prod(l,r).mx;}
    long long all_sum()const{return n?st[1].sum:0;}
};

struct SegSqrt{
    static constexpr long long INF=std::numeric_limits<long long>::max();
    struct Node{long long sum=0,mn=INF,mx=-INF;int len=0;long long setv=0;bool has=false;};

private:
    int n=0;
    std::vector<Node> st;
public:

    SegSqrt()=default;
    SegSqrt(const SegSqrt&)=default;
    SegSqrt& operator=(const SegSqrt&)=default;
    SegSqrt(SegSqrt&& other) noexcept:n(std::exchange(other.n,0)),st(std::move(other.st)){}
    SegSqrt& operator=(SegSqrt&& other) noexcept{
        if(this!=&other){n=std::exchange(other.n,0);st=std::move(other.st);}
        return *this;
    }
    explicit SegSqrt(int count):SegSqrt(beats_extra_detail::zeros(count)){}
    SegSqrt(const std::vector<long long>&a){init(a);}

private:
    static long long isqrt(long long x){
        long long y=static_cast<long long>(std::sqrt(static_cast<long double>(x)));
        while((__int128)(y+1)*(y+1)<=x)++y;
        while((__int128)y*y>x)--y;
        return y;
    }

    static Node merge(Node a,Node b){
        if(!a.len)return b;
        if(!b.len)return a;
        return {a.sum+b.sum,std::min(a.mn,b.mn),std::max(a.mx,b.mx),a.len+b.len};
    }

    void build(int p,int l,int r,const std::vector<long long>&a){
        if(r-l==1){st[p]={a[l],a[l],a[l],1};return;}
        int m=l+(r-l)/2;
        build(p*2,l,m,a);build(p*2+1,m,r,a);
        st[p]=merge(st[p*2],st[p*2+1]);
    }

    void tag(int p,long long x){auto& t=st[p];t.sum=x*t.len;t.mn=t.mx=x;t.setv=x;t.has=true;}
    void push(int p){if(st[p].has){tag(p*2,st[p].setv);tag(p*2+1,st[p].setv);st[p].has=false;}}

    void sqrt_(int p,int l,int r,int ql,int qr){
        if(qr<=l||r<=ql||st[p].mx<=1)return;

        if(ql<=l&&r<=qr){
            const long long lo=isqrt(st[p].mn),hi=isqrt(st[p].mx);
            if(lo==hi){tag(p,lo);return;}
        }
        push(p);
        int m=l+(r-l)/2;
        sqrt_(p*2,l,m,ql,qr);
        sqrt_(p*2+1,m,r,ql,qr);
        st[p]=merge(st[p*2],st[p*2+1]);
    }

    void set_(int p,int l,int r,int i,long long x){
        if(r-l==1){st[p]={x,x,x,1};return;}

        push(p);
        int m=l+(r-l)/2;
        if(i<m)set_(p*2,l,m,i,x);
        else set_(p*2+1,m,r,i,x);

        st[p]=merge(st[p*2],st[p*2+1]);
    }

    Node prod_(int p,int l,int r,int ql,int qr){
        if(qr<=l||r<=ql)return {};
        if(ql<=l&&r<=qr)return st[p];
        push(p);
        int m=l+(r-l)/2;

        return merge(
            prod_(p*2,l,m,ql,qr),
            prod_(p*2+1,m,r,ql,qr)
        );
    }

public:
    void init(const std::vector<long long>&a){
        beats_extra_detail::check_size(a.size());
        for(long long x:a)beats_extra_detail::nonnegative(x);
        n=static_cast<int>(a.size());

        st.assign(std::max<std::size_t>(1,4*static_cast<std::size_t>(n)),{});
        if(n)build(1,0,n,a);
    }

    int size()const{return n;}
    bool empty()const{return n==0;}
    void clear(){st.clear();n=0;}
    void sqrt(int l,int r){beats_extra_detail::range(n,l,r);if(l<r)sqrt_(1,0,n,l,r);}
    void set(int i,long long x){beats_extra_detail::point(n,i);beats_extra_detail::nonnegative(x);set_(1,0,n,i,x);}

    Node prod(int l,int r){beats_extra_detail::range(n,l,r);return l==r?Node{}:prod_(1,0,n,l,r);}
    long long get(int i){beats_extra_detail::point(n,i);return sum(i,i+1);}
    long long sum(int l,int r){return prod(l,r).sum;}
    long long min(int l,int r){return prod(l,r).mn;}
    long long max(int l,int r){return prod(l,r).mx;}
    long long all_sum()const{return n?st[1].sum:0;}
};

struct SegBitwiseBeats{
    using ull=unsigned long long;
    static constexpr ull ALL=static_cast<ull>(std::numeric_limits<long long>::max());
    static constexpr long long INF=std::numeric_limits<long long>::max();

    struct Node{
        long long sum=0,mn=INF,mx=-INF;
        ull bor=0,band=ALL,lazy_and=ALL,lazy_or=0;
        int len=0;
    };

private:
    int n=0;
    std::vector<Node> st;
public:

    SegBitwiseBeats()=default;
    SegBitwiseBeats(const SegBitwiseBeats&)=default;
    SegBitwiseBeats& operator=(const SegBitwiseBeats&)=default;
    SegBitwiseBeats(SegBitwiseBeats&& other) noexcept:n(std::exchange(other.n,0)),st(std::move(other.st)){}
    SegBitwiseBeats& operator=(SegBitwiseBeats&& other) noexcept{
        if(this!=&other){n=std::exchange(other.n,0);st=std::move(other.st);}
        return *this;
    }
    explicit SegBitwiseBeats(int count):SegBitwiseBeats(beats_extra_detail::zeros(count)){}
    SegBitwiseBeats(const std::vector<long long>&a){init(a);}

private:
    static Node leaf(long long x){
        return {x,x,x,(ull)x,(ull)x,ALL,0,1};
    }

    static Node merge(const Node&a,const Node&b){
        if(!a.len)return b;
        if(!b.len)return a;

        Node t;
        t.len=a.len+b.len;
        t.sum=a.sum+b.sum;
        t.mn=std::min(a.mn,b.mn);
        t.mx=std::max(a.mx,b.mx);
        t.bor=a.bor|b.bor;
        t.band=a.band&b.band;
        return t;
    }

    void build(int p,int l,int r,const std::vector<long long>&a){
        if(r-l==1){st[p]=leaf(a[l]);return;}
        int m=l+(r-l)/2;
        build(p*2,l,m,a);build(p*2+1,m,r,a);
        pull(p);
    }

    void pull(int p){st[p]=merge(st[p*2],st[p*2+1]);}

    void tag_and(int p,ull x){
        auto&t=st[p];

        ull z=t.band&~x;
        long long d=-(long long)z;

        t.sum+=d*t.len;
        t.mn+=d;
        t.mx+=d;
        t.bor&=x;
        t.band&=x;
        t.lazy_and&=x;
        t.lazy_or&=x;
    }

    void tag_or(int p,ull x){
        auto&t=st[p];

        ull z=(~t.bor)&x;
        long long d=(long long)z;

        t.sum+=d*t.len;
        t.mn+=d;
        t.mx+=d;
        t.bor|=x;
        t.band|=x;
        t.lazy_or|=x;
    }

    void push(int p){
        auto&t=st[p];

        if(t.lazy_and!=ALL){
            tag_and(p*2,t.lazy_and);
            tag_and(p*2+1,t.lazy_and);
        }

        if(t.lazy_or){
            tag_or(p*2,t.lazy_or);
            tag_or(p*2+1,t.lazy_or);
        }

        t.lazy_and=ALL;
        t.lazy_or=0;
    }

    void and_(int p,int l,int r,int ql,int qr,ull x){
        if(qr<=l||r<=ql||(st[p].bor&~x)==0)return;

        ull vary=st[p].bor^st[p].band;

        if(ql<=l&&r<=qr&&(vary&~x)==0){
            tag_and(p,x);
            return;
        }

        push(p);
        int m=l+(r-l)/2;

        and_(p*2,l,m,ql,qr,x);
        and_(p*2+1,m,r,ql,qr,x);
        pull(p);
    }

    void or_(int p,int l,int r,int ql,int qr,ull x){
        if(qr<=l||r<=ql||((~st[p].band)&x)==0)return;

        ull vary=st[p].bor^st[p].band;

        if(ql<=l&&r<=qr&&(vary&x)==0){
            tag_or(p,x);
            return;
        }

        push(p);
        int m=l+(r-l)/2;

        or_(p*2,l,m,ql,qr,x);
        or_(p*2+1,m,r,ql,qr,x);
        pull(p);
    }

    void set_(int p,int l,int r,int i,long long x){
        if(r-l==1){st[p]=leaf(x);return;}

        push(p);
        int m=l+(r-l)/2;

        if(i<m)set_(p*2,l,m,i,x);
        else set_(p*2+1,m,r,i,x);

        pull(p);
    }

    Node prod_(int p,int l,int r,int ql,int qr){
        if(qr<=l||r<=ql)return {};
        if(ql<=l&&r<=qr)return st[p];

        push(p);
        int m=l+(r-l)/2;

        return merge(
            prod_(p*2,l,m,ql,qr),
            prod_(p*2+1,m,r,ql,qr)
        );
    }

public:
    void init(const std::vector<long long>&a){
        beats_extra_detail::check_size(a.size());
        for(long long x:a)beats_extra_detail::nonnegative(x);
        n=static_cast<int>(a.size());

        st.assign(std::max<std::size_t>(1,4*static_cast<std::size_t>(n)),{});
        if(n)build(1,0,n,a);
    }

    int size()const{return n;}
    bool empty()const{return n==0;}
    void clear(){st.clear();n=0;}

    void bit_and(int l,int r,long long x){
        beats_extra_detail::range(n,l,r);
        beats_extra_detail::nonnegative(x);
        if(l<r)and_(1,0,n,l,r,(ull)x);
    }

    void bit_or(int l,int r,long long x){
        beats_extra_detail::range(n,l,r);
        beats_extra_detail::nonnegative(x);
        if(l<r)or_(1,0,n,l,r,(ull)x);
    }

    void set(int i,long long x){
        beats_extra_detail::point(n,i);
        beats_extra_detail::nonnegative(x);
        set_(1,0,n,i,x);
    }

    Node prod(int l,int r){beats_extra_detail::range(n,l,r);return l==r?Node{}:prod_(1,0,n,l,r);}
    long long get(int i){beats_extra_detail::point(n,i);return sum(i,i+1);}
    long long sum(int l,int r){return prod(l,r).sum;}
    long long min(int l,int r){return prod(l,r).mn;}
    long long max(int l,int r){return prod(l,r).mx;}
    long long all_sum()const{return n?st[1].sum:0;}
};

/*
細かい注意点
0-indexed、区間は[l,r)。N<=INT_MAX/4。各クラスは独立した木。
全値・マスクは0以上のlong long、法・除数は正。区間和と中間演算もlong longに収まること。
Nodeの共通情報は{sum,mn,mx,len}。Bitwise型にはbor/bandもある。
空区間のprodは{0,LLONG_MAX,-LLONG_MAX,0}、bor=0、band=LLONG_MAX。
空区間更新は何もしないが、法・除数・値などの前提条件は検査する。
範囲外、負値、負の長さ、0以下の法/除数などはメッセージなしで停止。
divide/sqrtは最小値と最大値が同じ結果へ写る節点を一括代入する。
mod/divide/sqrt/AND/ORは単発最悪O(N)。任意の更新列でO(log N)を保証しない。
set/assignやAND/ORの交互更新は償却評価の前提を変える。保持メモリO(N)。
Nodeのsetv/has/lazy_*は内部実装用。

使える関数一覧
// 型の選択
SegMod seg(v);                                 // 非負列の区間mod + 和・最小・最大 O(N)
SegDiv seg(v);                                 // 非負列のfloor除算・代入 + 和・最小・最大 O(N)
SegSqrt seg(v);                                // 非負列のfloor平方根 + 和・最小・最大 O(N)
SegBitwiseBeats seg(v);                        // 非負63bit列のAND/OR + 和・最小・最大 O(N)

// 構築・コピー・ムーブ（otherも同じType）
Type seg;                                      // 返り値なし: 長さ0で構築 O(1)
Type seg(n);                                   // 返り値なし: 長さn、全要素0で構築 O(N)
Type seg(v);                                   // 返り値なし: 配列vから構築 O(N)
Type seg(other);                               // 返り値なし: 独立なコピーを構築 O(N_other)
Type seg(std::move(other));                    // 返り値なし: 木を移し、otherを長さ0にする O(1)
seg = other;                                   // Type&: segへの参照。独立なコピーで上書き O(N+N_other)
seg = std::move(other);                        // Type&: segへの参照。木を移し、otherを長さ0にする O(N+1)

// 全型共通API
seg.init(v);                                   // void: vで木を作り直す O(N)
seg.size();                                    // int: 配列長Nを返す O(1)
seg.empty();                                   // bool: 配列長0ならtrue O(1)
seg.clear();                                   // void: 全ノードを破棄し長さ0にする O(N)、確保容量は残る場合あり
seg.set(i,x);                                  // void: A[i]=xに変更 O(log(N+1))
seg.get(i);                                    // long long: A[i]を返す O(log(N+1))
seg.prod(l,r);                                 // Node: [l,r)の{sum,mn,mx,len}等を返す O(log(N+1))
seg.sum(l,r);                                  // long long: [l,r)の和を返す O(log(N+1))
seg.min(l,r);                                  // long long: [l,r)の最小値を返す O(log(N+1))
seg.max(l,r);                                  // long long: [l,r)の最大値を返す O(log(N+1))
seg.all_sum();                                 // long long: 配列全体の和を返す O(1)

// 型別の更新API（いずれも返り値はvoid）
seg.mod(l,r,x);                                // SegMod: A[i]%=x。x>0。単発最悪O(N)、変化しない節点は省略
seg.divide(l,r,d);                             // SegDiv: A[i]=floor(A[i]/d)。d>0。単発最悪O(N)、d=1ならO(1)
seg.assign(l,r,x);                             // SegDiv: [l,r)をxに代入 O(log(N+1))
seg.sqrt(l,r);                                 // SegSqrt: A[i]=floor(sqrt(A[i]))。単発最悪O(N)、0/1節点は省略
seg.bit_and(l,r,x);                            // SegBitwiseBeats: A[i]&=x。単発最悪O(N)、共通ビットの変化は一括適用
seg.bit_or(l,r,x);                             // SegBitwiseBeats: A[i]|=x。単発最悪O(N)、共通ビットの変化は一括適用
// divide/sqrtは最小値・最大値が同じ結果へ写る節点を一括代入する。

基本の関数
SegDiv seg(vector<ll>{8,9,10});
seg.divide(0,3,3);                            // {2,3,3}
auto result = seg.prod(0,3);                  // {sum=8,mn=2,mx=3,len=3}
seg.assign(1,3,5);                            // {2,5,5}
auto sum = seg.sum(0,3);                      // 12
*/
