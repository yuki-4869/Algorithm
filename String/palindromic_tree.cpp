class PalindromicTree{
    struct Node{
        array<int,26> next{};
        int length,link=0,end=-1;
        ll count=0;
        explicit Node(int length):length(length){}
    };
    vector<Node> nodes_;
    string text_;
    int suffix_=1;
    void check_id(ll id)const{if(id<0||id>=distinct_count())std::abort();}
    int extendable(int v,int pos,char c)const{
        while(pos-1-nodes_[v].length<0||text_[pos-1-nodes_[v].length]!=c)v=nodes_[v].link;
        return v;
    }
public:
    PalindromicTree(){clear();}
    explicit PalindromicTree(const string&s):PalindromicTree(){
        if(s.size()>static_cast<size_t>(INT_MAX-2))std::abort();
        for(char c:s)push(c);
    }
    void clear(){
        vector<Node>{Node(-1),Node(0)}.swap(nodes_);
        string{}.swap(text_);suffix_=1;
    }
    ll push(char c){
        if(c<'a'||c>'z'||text_.size()>=static_cast<size_t>(INT_MAX-2))std::abort();
        const int letter=c-'a';
        text_.push_back(c);
        const int pos=static_cast<int>(text_.size())-1;
        const int v=extendable(suffix_,pos,c);
        if(nodes_[v].next[letter]==0){
            const int id=static_cast<int>(nodes_.size());
            nodes_.emplace_back(nodes_[v].length+2);
            nodes_[v].next[letter]=id;nodes_[id].end=pos;
            if(nodes_[id].length==1)nodes_[id].link=1;
            else{
                const int u=extendable(nodes_[v].link,pos,c);
                nodes_[id].link=nodes_[u].next[letter];
            }
        }
        suffix_=nodes_[v].next[letter];++nodes_[suffix_].count;
        return suffix_-2;
    }
    ll size()const{return static_cast<ll>(text_.size());}
    ll distinct_count()const{return static_cast<ll>(nodes_.size())-2;}
    ll longest_suffix()const{return suffix_>=2?suffix_-2:-1;}
    ll length(ll id)const{check_id(id);return nodes_[id+2].length;}
    pll first_occurrence(ll id)const{
        check_id(id);const auto&node=nodes_[id+2];
        const ll r=ll(node.end)+1;return {r-node.length,r};
    }
    ll suffix_link(ll id)const{check_id(id);return max(-1,nodes_[id+2].link-2);}
    vl occurrence_counts()const{
        const ll p=distinct_count();vl result(p);
        for(ll i=0;i<p;++i)result[i]=nodes_[i+2].count;
        for(ll i=p-1;i>=0;--i){const ll link=suffix_link(i);if(link>=0)result[link]+=result[i];}
        return result;
    }
};

/*
細かい注意点
ll,vl,pllは基本テンプレートの型エイリアス。追加include・別ライブラリは不要。
英小文字a..zのみ。文字数NはINT_MAX-2以下。不正文字・長さ超過・範囲外IDはNDEBUGでも無言で停止。
空でない異なる回文に初登場順で0-indexedのIDを付ける。IDはclearするまで不変。仮想根は公開しない。
push(c)は追加後の最長回文接尾辞のIDを返す。新しいIDとは限らない。削除・途中変更は非対応。
first_occurrence(id)は最初の出現区間[l,r)。同じ回文の複数出現は同じIDで扱い、出現数は重なりも含む。
occurrence_countsはID順の独立したvectorを返す。何度呼んでも状態を変えず、その後もpushできる。
固定26文字の遷移表を各ノードに持つ。P=異なる回文数<=N、保持領域O(N)、遷移表だけで約104(P+2)byte。
pushは償却O(1)だが1回の最悪時間はO(N)。全接頭辞でoccurrence_countsを呼ぶと合計O(N^2)になり得る。

使える関数一覧
PalindromicTree tree;                // 空状態を構築 O(1)
PalindromicTree tree(s);             // sを読み込む O(N)時間・領域
tree.clear();                       // void: 空へ戻しID・確保領域を破棄 O(N)、空ならO(1)
tree.push(c);                       // ll: 末尾追加後の最長回文接尾辞ID 償却O(1)
tree.size();                        // ll: 現在の文字数 O(1)
tree.distinct_count();              // ll: 異なる空でない回文数P O(1)
tree.longest_suffix();              // ll: 最長回文接尾辞ID、空なら-1 O(1)
tree.length(id);                    // ll: 回文の長さ O(1)
tree.first_occurrence(id);          // pll: 最初の出現の半開区間[l,r) O(1)
tree.suffix_link(id);               // ll: 最長の真の回文接尾辞ID、空しかなければ-1 O(1)
tree.occurrence_counts();           // vl: 各IDの重なりを含む出現数 O(P)時間・追加領域、空ならO(1)

基本の関数
string s="ababa";
PalindromicTree tree(s);
assert(tree.size()==5 && tree.distinct_count()==5);
ll id=tree.longest_suffix();
assert(tree.length(id)==5 && tree.length(tree.suffix_link(id))==3);
assert((tree.first_occurrence(id)==pll{0,5}));
auto counts=tree.occurrence_counts();
assert((counts==vl{3,2,2,1,1}));       // ID順にa,b,aba,bab,ababa
ll next=tree.push('b');               // 新しい回文bababのID
assert(tree.length(next)==5);
assert((tree.occurrence_counts()==vl{3,3,2,2,1,1}));
tree.clear();
assert(tree.size()==0 && tree.distinct_count()==0 && tree.longest_suffix()==-1);
*/
