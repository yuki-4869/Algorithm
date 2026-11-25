class SuffixAutomaton{
    struct State{
        array<int,26> next{};
        int length=0,link=-1;
        ll occurrences=0;
    };
    vector<State> states_;
    ll distinct_=0;

    static int letter(char c){
        if(c<'a'||c>'z')std::abort();
        return c-'a';
    }
    int extend(int last,int c){
        const int current=static_cast<int>(states_.size());
        states_.emplace_back();
        states_[current].length=states_[last].length+1;
        states_[current].occurrences=1;
        int v=last;
        while(v>=0&&states_[v].next[c]==0){
            states_[v].next[c]=current;
            v=states_[v].link;
        }
        if(v<0){
            states_[current].link=0;
            return current;
        }
        const int q=states_[v].next[c];
        if(states_[v].length+1==states_[q].length){
            states_[current].link=q;
            return current;
        }
        const int clone=static_cast<int>(states_.size());
        states_.push_back(states_[q]);
        states_[clone].length=states_[v].length+1;
        states_[clone].occurrences=0;
        while(v>=0&&states_[v].next[c]==q){
            states_[v].next[c]=clone;
            v=states_[v].link;
        }
        states_[q].link=states_[current].link=clone;
        return current;
    }
public:
    explicit SuffixAutomaton(const string& s){
        if(s.size()>static_cast<size_t>(INT_MAX/2))std::abort();
        states_.reserve(max<size_t>(1,2*s.size()));
        states_.emplace_back();
        int last=0;
        for(char c:s)last=extend(last,letter(c));

        vector<int> ends(s.size()+1),order(states_.size());
        for(const auto& state:states_)++ends[state.length];
        partial_sum(ends.begin(),ends.end(),ends.begin());
        for(int v=0;v<static_cast<int>(states_.size());++v)
            order[--ends[states_[v].length]]=v;
        for(size_t i=order.size();i>1;--i){
            const int v=order[i-1],link=states_[v].link;
            states_[link].occurrences+=states_[v].occurrences;
            distinct_+=states_[v].length-states_[link].length;
        }
    }
    ll count(const string& pattern)const{
        if(pattern.empty())return states_[0].occurrences+1;
        int v=0;
        for(char c:pattern){
            const int index=letter(c);
            if(v<0)continue;
            const int next=states_[v].next[index];
            v=next==0?-1:next;
        }
        return v<0?0:states_[v].occurrences;
    }
    ll distinct_count()const{return distinct_;}
    ll longest_common_substring_length(const string& t)const{
        int v=0,length=0,best=0;
        for(char c:t){
            const int index=letter(c);
            while(v!=0&&states_[v].next[index]==0){
                v=states_[v].link;
                length=states_[v].length;
            }
            if(states_[v].next[index]!=0){
                v=states_[v].next[index];
                best=max(best,++length);
            }
        }
        return best;
    }
};

/*
細かい注意点
構築文字列・クエリ文字列ともに英小文字a..zのみ。N=s.size()はINT_MAX/2以下。
不正文字や構築時の長さ超過はNDEBUGでもメッセージなしで停止する。
構築後はsへの参照を保持しない。出現回数は重なりを含め、空文字列はN+1回出現とする。
distinct_countは空文字列を含めない。最長共通部分文字列は連続する区間の長さを返す。
各状態にintの遷移表26要素を持つ。N>0なら最大2N状態分を確保する。
llは基本テンプレートの型エイリアス。追加include・ACLの機能は不要。

使える関数一覧
SuffixAutomaton sam(s);                    // sを前処理 O(N)時間・領域
sam.count(pattern);                        // ll: patternの出現回数 O(pattern.size())時間・追加O(1)領域
sam.distinct_count();                      // ll: 異なる非空部分文字列の数 O(1)時間・追加領域
sam.longest_common_substring_length(t);    // ll: s,tの最長共通部分文字列の長さ O(t.size())時間・追加O(1)領域

基本の関数
SuffixAutomaton sam("ababa");
assert(sam.count("aba")==2);
assert(sam.count("ac")==0 && sam.count("")==6);
assert(sam.distinct_count()==9);
assert(sam.longest_common_substring_length("zzbabb")==3); // "bab"
*/
