template<class S,S(*op)(S,S),S(*e)()>
class SwagQueue{
    struct Node{S value,aggregate;};
    std::vector<Node> in_,out_;
    void prepare_(){
        if(!out_.empty())return;
        std::vector<Node> next;
        next.reserve(in_.size());
        for(auto it=in_.rbegin();it!=in_.rend();++it){
            S aggregate=next.empty()?it->value:op(it->value,next.back().aggregate);
            next.push_back({it->value,aggregate});
        }
        out_.swap(next);
        in_.clear();
    }
public:
    SwagQueue()=default;
    std::size_t size()const{return in_.size()+out_.size();}
    bool empty()const{return in_.empty()&&out_.empty();}
    void push(const S&value){
        S aggregate=in_.empty()?value:op(in_.back().aggregate,value);
        in_.push_back({value,aggregate});
    }
    void pop(){
        if(empty())std::abort();
        prepare_();
        out_.pop_back();
    }
    S front()const{
        if(empty())std::abort();
        return out_.empty()?in_.front().value:out_.back().value;
    }
    S all_prod()const{
        if(out_.empty())return in_.empty()?e():in_.back().aggregate;
        if(in_.empty())return out_.back().aggregate;
        return op(out_.back().aggregate,in_.back().aggregate);
    }
};

/*
細かい注意点
FIFOのキュー。先頭を位置0として、追加した順にopで集約する。任意位置の更新・削除は不可。
opは結合則、eは左右両方の単位元を満たすこと。可換性・逆元・冪等性は不要。
Sはコピー構築・コピー代入可能で、デフォルト構築は不要。op,eは結果が外部状態に依存しないこと。
空のall_prodはe()。空のpop/frontはNDEBUGでもメッセージなしで停止。
frontとall_prodは値を返す。返り値の変更はキューに影響しない。
以下はSのコピー・破棄、op,eがO(1)の場合。整数の途中計算は型に収める。
push/popは償却O(1)。1回のpopは要素の移し替えで最悪O(N)。front/all_prodは最悪O(1)。
vectorの容量は削除後も保持するため、確保領域は過去最大の要素数Pに対してO(P)。
標準ヘッダーだけを使い、基本テンプレート以外のincludeは不要。

使える関数一覧
SwagQueue<S,op,e> q;  // 空キューを構築 O(1)
size_t q.size();      // 現在の要素数 O(1)
bool q.empty();       // 空ならtrue O(1)
void q.push(value);   // 末尾へ1要素追加 償却O(1)
void q.pop();         // 先頭の1要素を削除 償却O(1)、最悪O(N)
S q.front();          // 先頭の値を返す 最悪O(1)、constからも取得可能
S q.all_prod();       // 先頭から末尾までの集約値、空ならe() 最悪O(1)、constからも取得可能

基本の関数
constexpr auto sum_op=[](ll a,ll b){return a+b;};
constexpr auto sum_e=[](){return 0LL;};
SwagQueue<ll,sum_op,sum_e> q;
q.push(3);q.push(1);q.push(4);
auto total=q.all_prod(); // 8
q.pop();                // 3を削除
auto first=q.front();   // 1
auto rest=q.all_prod();  // 5
auto count=q.size();    // 2
*/
