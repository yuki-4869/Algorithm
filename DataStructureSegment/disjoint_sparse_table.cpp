template<class S,S(*op)(S,S),S(*e)()>
class DisjointSparseTable{
    int n_=0;
    std::vector<S> values_;
    std::vector<std::vector<S>> table_;
public:
    DisjointSparseTable()=default;
    explicit DisjointSparseTable(const std::vector<S>&a){build(a);}
    void build(const std::vector<S>&a){
        if(a.size()>static_cast<std::size_t>(std::numeric_limits<int>::max()))std::abort();
        n_=static_cast<int>(a.size());
        values_=a;
        const int levels=n_<=1?0:std::bit_width(static_cast<unsigned>(n_-1));
        table_.assign(levels,a);
        const std::size_t n=a.size();
        for(int k=0;k<levels;++k){
            const std::size_t half=std::size_t{1}<<k;
            for(std::size_t left=0;left<n;left+=2*half){
                const std::size_t mid=std::min(left+half,n),right=std::min(left+2*half,n);
                for(std::size_t i=mid-1;i>left;--i)table_[k][i-1]=op(values_[i-1],table_[k][i]);
                for(std::size_t i=mid+1;i<right;++i)table_[k][i]=op(table_[k][i-1],values_[i]);
            }
        }
    }
    int size()const{return n_;}
    S get(int i)const{
        if(i<0||i>=n_)std::abort();
        return values_[i];
    }
    S prod(int l,int r)const{
        if(l<0||l>r||r>n_)std::abort();
        if(l==r)return e();
        if(r-l==1)return values_[l];
        const int k=std::bit_width(static_cast<unsigned>(l)^static_cast<unsigned>(r-1))-1;
        return op(table_[k][l],table_[k][r-1]);
    }
    S all_prod()const{return prod(0,n_);}
};

/*
細かい注意点
0-indexed、区間は[l,r)。構築後の点更新は不可。列を変える場合はbuildで再構築する。
opは結合則、eは左右両方の単位元を満たす。可換性・冪等性・逆元は不要。
prodは添字の小さい順に集約する。空区間はe()、1要素の区間はその要素を返す。
Sはコピー構築・代入可能とし、デフォルト構築は不要。演算・コピーがO(1)の場合の計算量を記す。
0<=N<=INT_MAX、0<=l<=r<=N、0<=i<N。範囲違反時はメッセージなしで停止する。
整数演算は型に収める。標準ライブラリだけを使い、code/codet/codecfへの追加includeは不要。
buildには旧表の解放・上書き分の時間も加わり、確保済み容量は再利用される場合がある。

使える関数一覧
DisjointSparseTable<S,op,e> st;                // 空列で構築 O(1)
DisjointSparseTable<S,op,e> st(v);             // 列vで構築 O(N log(N+1))、領域も同じ
void st.build(v);                             // 列vで再構築 O(N log(N+1))
int st.size();                               // 要素数を返す O(1)
S st.get(i);                                 // i番目の値を返す O(1)
S st.prod(l,r);                              // [l,r)の集約値を返す O(1)
S st.all_prod();                             // 列全体の集約値を返す O(1)

基本の関数
constexpr auto sum_op=[](ll a,ll b){return a+b;};
constexpr auto sum_e=[](){return 0LL;};
DisjointSparseTable<ll,sum_op,sum_e> st(vl{3,1,4,1,5});
auto sum=st.prod(1,4);                        // 6 O(1)
auto value=st.get(2);                         // 4 O(1)
auto total=st.all_prod();                     // 14 O(1)
auto empty=st.prod(3,3);                      // 0 O(1)
st.build(vl{2,7,1});                          // 列を作り直す O(N log(N+1))
auto n=st.size();                            // 3 O(1)
*/
