struct TrieAzAZ{
    static const ll SIG=52;
    struct Node{
        array<ll,SIG> nx; ll cnt=0,end=0,val=0;
        Node(){nx.fill(-1);}
    };
    vector<Node> t{Node()};

    ll id(char c)const{return 'a'<=c&&c<='z'?c-'a':c-'A'+26;}

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
ll id(c)              a-zを0～25、A-Zを26～51に変換 O(1)
ll insert(s)          sを1個追加、終点ノードを返す O(|s|)
ll insert(s,f)        各prefixでcnt更新前にf(node,depth,true)を実行 O(|s|)
bool erase(s)         sを1個削除、なければfalse O(|s|)
bool erase(s,f)       各prefixでcnt更新前にf(node,depth,false)を実行 O(|s|)
ll find(s)            sのノード番号、なければ-1 O(|s|)
ll count(s)           sそのものの登録個数 O(|s|)
ll prefix_count(s)    sをprefixに持つ文字列数 O(|s|)
bool contains(s)      sが存在するか O(|s|)
bool starts_with(s)   sをprefixに持つ文字列が存在するか O(|s|)
ll go(v,c)            vからcへ進む、なければ作成 O(1)
ll next(v,c)          vからcへ進む、現在存在しなければ-1 O(1)
ll walk(s,f)          各prefixでf(node,depth)、辿れた長さを返す O(|s|)
Node& T[v]            ノードvを取得 O(1)
ll size()             重複込み登録文字列数 O(1)
ll node_count()       作成済みノード数 O(1)
bool empty()          空ならtrue O(1)
void reserve(n)       ノード領域を予約 O(現在ノード数)
void clear()          全削除 O(ノード数)

TrieAzAZ T;

auto f=[&](auto& n,ll d,bool add){
    if(add){
        // insert、n.cntは追加前
    }else{
        // erase、n.cntは削除前
    }
};
T.insert(s,f);
T.erase(s,f);
*/