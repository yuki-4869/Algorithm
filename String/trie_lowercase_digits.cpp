struct TrieAZ09{
    static const ll SIG=36;
    struct Node{
        array<ll,SIG> nx; ll cnt=0,end=0,val=0;
        Node(){nx.fill(-1);}
    };
    vector<Node> t{Node()};

    ll id(char c)const{return 'a'<=c&&c<='z'?c-'a':c-'0'+26;}

    template<class F> void call(F& f,Node& n,ll d,bool add){
        if constexpr(is_invocable_v<F,Node&,ll,bool>)f(n,d,add);
        else f(n,d);
    }

    ll go(ll v,char c){
        ll x=id(c);
        if(t[v].nx[x]<0)t[v].nx[x]=t.size(),t.emplace_back();
        return t[v].nx[x];
    }
    ll next(ll v,char c)const{
        ll u=t[v].nx[id(c)];
        return u<0||t[u].cnt==0?-1:u;
    }

    template<class F> ll insert(const string&s,F f){
        t[0].cnt++;ll v=0;
        for(ll d=0;d<(ll)s.size();d++)
            v=go(v,s[d]),call(f,t[v],d+1,true),t[v].cnt++;
        t[v].end++;return v;
    }
    ll insert(const string&s){return insert(s,[](Node&,ll){});}

    template<class F> bool erase(const string&s,F f){
        if(!contains(s))return false;
        t[0].cnt--;ll v=0;
        for(ll d=0;d<(ll)s.size();d++)
            v=t[v].nx[id(s[d])],call(f,t[v],d+1,false),t[v].cnt--;
        t[v].end--;return true;
    }
    bool erase(const string&s){return erase(s,[](Node&,ll){});}

    ll find(const string&s)const{
        ll v=0;for(char c:s){v=next(v,c);if(v<0)return -1;}return v;
    }
    ll count(const string&s)const{ll v=find(s);return v<0?0:t[v].end;}
    ll prefix_count(const string&s)const{ll v=find(s);return v<0?0:t[v].cnt;}
    bool contains(const string&s)const{return count(s)>0;}
    bool starts_with(const string&s)const{return prefix_count(s)>0;}

    template<class F> ll walk(const string&s,F f){
        ll v=0,d=0;
        for(char c:s){v=next(v,c);if(v<0)break;f(t[v],++d);}
        return d;
    }

    Node& operator[](ll v){return t[v];}
    const Node& operator[](ll v)const{return t[v];}
    ll size()const{return t[0].cnt;}
    ll node_count()const{return t.size();}
    bool empty()const{return size()==0;}
    void reserve(ll n){t.reserve(n);}
    void clear(){t.assign(1,Node());}
};

/*
細かい注意点
文字集合は英小文字a-zと数字0-9。範囲外の文字は渡さない。重複する文字列を登録できる。
コールバックf(node,depth,add)は各prefixでcnt更新前に呼ばれる。add=trueが追加、falseが削除。
depthは1始まり。根にはコールバックを呼ばない。f(node,depth)形式も使える。
Nodeのcntはprefix個数、endは終端個数、valは利用者が自由に使うlong long。
ノード番号は0-indexed。vector拡張でNodeへの参照が無効になる場合がある。
goはノードを作るが文字列の登録数を増やさない。削除後もノード領域は残る。

使える関数一覧
TrieAZ09 T;                // 空で構築
T.id(c);                  // ll: 文字を内部番号へ変換 O(1)
T.insert(s);              // ll: 1個追加し終端ノードを返す O(|s|)
T.insert(s,f);            // ll: prefixごとにfを実行して追加 O(|s|)
T.erase(s);               // bool: 1個削除、なければfalse O(|s|)
T.erase(s,f);             // bool: prefixごとにfを実行して削除 O(|s|)
T.find(s);                // ll: ノード番号、なければ-1 O(|s|)
T.count(s);               // ll: sの登録個数 O(|s|)
T.prefix_count(s);        // ll: sをprefixに持つ登録個数 O(|s|)
T.contains(s);            // bool: sが登録済みか O(|s|)
T.starts_with(s);         // bool: sをprefixに持つ文字列があるか O(|s|)
T.go(v,c);                // ll: 次のノード、なければ作成 O(1)
T.next(v,c);              // ll: 現在有効な次のノード、なければ-1 O(1)
T.walk(s,f);              // ll: prefixごとにf(node,depth)、辿れた長さ O(|s|)
T[v];                    // Node& / const Node&: ノード参照 O(1)
T.size();                 // ll: 重複込み登録数 O(1)
T.node_count();           // ll: 確保済みノード数 O(1)
T.empty();                // bool: 登録数0か O(1)
T.reserve(n);             // void: ノード容量を確保 O(現在ノード数)
T.clear();                // void: 全登録を削除 O(ノード数)

基本の関数
TrieAZ09 trie;
trie.insert("a12");
auto count = trie.count("a12");          // 1
auto prefixes = trie.prefix_count("a");
trie.erase("a12");
*/
