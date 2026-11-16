class SlopeTrick{
    vector<LL> left_,right_;
    LL minimum_=0,add_left_=0,add_right_=0;
    void push_left(LL x){
        left_.push_back(x-add_left_);
        push_heap(left_.begin(),left_.end());
    }
    void push_right(LL x){
        right_.push_back(x-add_right_);
        push_heap(right_.begin(),right_.end(),greater<LL>());
    }
    LL pop_left(){
        pop_heap(left_.begin(),left_.end());
        LL x=left_.back()+add_left_;
        left_.pop_back();
        return x;
    }
    LL pop_right(){
        pop_heap(right_.begin(),right_.end(),greater<LL>());
        LL x=right_.back()+add_right_;
        right_.pop_back();
        return x;
    }
public:
    explicit SlopeTrick(LL c=0):minimum_(c){}
    size_t size()const{return left_.size()+right_.size();}
    LL min_value()const{return minimum_;}
    pair<optional<LL>,optional<LL>> argmin()const{
        optional<LL> l,r;
        if(!left_.empty())l=left_.front()+add_left_;
        if(!right_.empty())r=right_.front()+add_right_;
        return {l,r};
    }
    void add_const(LL c){minimum_+=c;}
    void add_a_minus_x(LL a){
        if(!right_.empty() && right_.front()+add_right_<a){
            LL r=pop_right();
            minimum_+=a-r;
            push_right(a);
            a=r;
        }
        push_left(a);
    }
    void add_x_minus_a(LL a){
        if(!left_.empty() && left_.front()+add_left_>a){
            LL l=pop_left();
            minimum_+=l-a;
            push_left(a);
            a=l;
        }
        push_right(a);
    }
    void add_abs(LL a){add_a_minus_x(a);add_x_minus_a(a);}
    void prefix_min(){vector<LL>().swap(right_);add_right_=0;}
    void suffix_min(){vector<LL>().swap(left_);add_left_=0;}
    void shift(LL a,LL b){
        if(a>b)std::abort();
        add_left_+=a;add_right_+=b;
    }
    void shift(LL a){shift(a,a);}
    LL eval(LL x)const{
        LL result=minimum_;
        for(LL l:left_)result+=max(LL(0),l+add_left_-x);
        for(LL r:right_)result+=max(LL(0),x-r-add_right_);
        return result;
    }
    void clear(){
        vector<LL>().swap(left_);vector<LL>().swap(right_);
        minimum_=add_left_=add_right_=0;
    }
    void merge(SlopeTrick& other){
        if(this==&other){SlopeTrick copy=other;merge(copy);return;}
        if(size()<other.size())std::swap(*this,other);
        minimum_+=other.minimum_;
        for(LL l:other.left_)add_a_minus_x(l+other.add_left_);
        for(LL r:other.right_)add_x_minus_a(r+other.add_right_);
        other.clear();
    }
};

/*
細かい注意点
LL=__int128は基本テンプレートの型。追加include・他のライブラリへの依存は不要。
実数全体で有限、下に有界な凸区分線形関数を管理。折れ点・傾きは整数。
初期状態はf(x)=c。任意の関数を配列等から直接構築するAPIはない。
座標・値・オフセット・保存座標を含む全中間演算を符号付き128bitに収める。
LLの直接出力は基本テンプレートにない。llに収まると確認できる結果だけ(ll)で変換する。
argminの両端は閉区間。左のnulloptは-∞、右のnulloptは+∞。両方nulloptなら定数関数。
shift(a)はf(x-a)、shift(a,b)はmin_{x-b<=y<=x-a}f(y)。a<=bが必要。
prefix_minはmin_{y<=x}f(y)、suffix_minはmin_{y>=x}f(y)。元の関数を置き換える。
merge(other)はf+=other.fとしotherをf(x)=0へ初期化。自分自身ならf*=2。
重みwの折れ線追加はw回の追加が必要。重み付き一括追加・削除・最適経路の復元は非対応。
Kは保持中の折れ点数（重複込み）。sizeは異なる座標数ではない。K<=1の計算量はO(1)と読む。
vectorの再確保を含め、折れ線追加の計算量は償却。evalはO(K)なので頻繁な点照会には注意。
通常のコピーはO(K)時間・領域で独立した関数を作る。公開API以外の内部状態は直接操作しない。

使える関数一覧
SlopeTrick st(c=0);          // f(x)=cを構築 O(1)時間・領域
st.size();                  // size_t: 重複込みの折れ点数K O(1)
st.min_value();             // LL: 全体の最小値 O(1)
st.argmin();                // pair<optional<LL>,optional<LL>>: 最小値を取る閉区間 O(1)
st.add_const(c);            // void: f(x)+=c O(1)
st.add_a_minus_x(a);        // void: f(x)+=max(a-x,0) 償却O(log K)、折れ点1個追加
st.add_x_minus_a(a);        // void: f(x)+=max(x-a,0) 償却O(log K)、折れ点1個追加
st.add_abs(a);              // void: f(x)+=|x-a| 償却O(log K)、折れ点2個追加
st.prefix_min();            // void: f(x)=min_{y<=x}f(y) O(K)、右側の折れ点を削除
st.suffix_min();            // void: f(x)=min_{y>=x}f(y) O(K)、左側の折れ点を削除
st.shift(a);                // void: f(x)=old_f(x-a) O(1)
st.shift(a,b);              // void: f(x)=min_{x-b<=y<=x-a}old_f(y) O(1)、a<=b
st.eval(x);                 // LL: f(x)を返す、関数は変更しない O(K)時間・追加O(1)領域
st.clear();                 // void: f(x)=0へ戻しメモリを解放 O(K)
st.merge(other);            // void: 関数を加算しotherを初期化 償却O((min(N,M)+1)log(N+M))、N/Mは各size

基本の関数
SlopeTrick st;
st.add_abs(1);st.add_abs(5);
assert(st.min_value()==4 && st.eval(0)==6);
auto [l,r]=st.argmin();
assert(l && r && *l==1 && *r==5);
st.shift(-2,3);
auto interval=st.argmin();
assert(interval.first && interval.second && *interval.first==-1 && *interval.second==8);
st.add_abs(10);
assert(st.min_value()==6);
SlopeTrick cost(7);cost.add_abs(0);
st.merge(cost);
assert(cost.size()==0 && cost.min_value()==0);
SlopeTrick dp;
for(ll a:vl{3,1,2}){dp.prefix_min();dp.add_abs(a);}
assert(dp.min_value()==2);
*/
