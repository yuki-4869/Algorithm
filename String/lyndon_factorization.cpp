vector<pll> lyndon_factorization(const string&s){
    if(s.size()>static_cast<size_t>(INT_MAX))std::abort();
    const ll n=static_cast<ll>(s.size());
    auto byte=[](char c){return static_cast<unsigned char>(c);};
    vector<pll> result;
    ll i=0;
    while(i<n){
        ll j=i+1,k=i;
        while(j<n&&byte(s[k])<=byte(s[j])){
            if(byte(s[k])<byte(s[j]))k=i;
            else ++k;
            ++j;
        }
        const ll length=j-k;
        while(i<=k){result.emplace_back(i,i+length);i+=length;}
    }
    return result;
}
ll minimum_rotation(const string&s){
    if(s.size()>static_cast<size_t>(INT_MAX))std::abort();
    const ll n=static_cast<ll>(s.size());
    if(n==0)return 0;
    auto at=[&](ll i){return static_cast<unsigned char>(s[i%n]);};
    ll i=0,result=0;
    while(i<n){
        result=i;
        ll j=i+1,k=i;
        while(j<2*n&&at(k)<=at(j)){
            if(at(k)<at(j))k=i;
            else ++k;
            ++j;
        }
        const ll length=j-k;
        while(i<=k)i+=length;
    }
    return result;
}

/*
細かい注意点
N=s.size()はINT_MAX以下。超過時はNDEBUGでもメッセージなしで停止。追加includeは不要。
ll,pllは基本テンプレートの型エイリアス。入力文字列は変更しない。
辞書順はunsigned charの値順。埋め込みNUL・上位bit付きbyteにも対応し、UTF-8はbyte列として扱う。
Lyndon語は空でなく、すべての空でない真の接尾辞より辞書順で小さい文字列。
分解はLyndon語を辞書順非増加に並べた一意な分解。同じ因子も別々に返す。
分解結果は0-indexedの半開区間[l,r)。空文字列なら空vector。
最小巡回シフトはs[p..N)+s[0..p)の最小化。複数の最小解があれば最小のp、空なら0を返す。
最小巡回シフトは文字列を返さず開始位置だけを返す。反転を同一視する処理は呼び出し側で行う。

使える関数一覧
vector<pll> lyndon_factorization(s); // 各Lyndon因子の[l,r)を左から返す O(N+1)時間・返り値O(N+1)領域・作業領域O(1)
ll minimum_rotation(s);             // 辞書順最小の巡回シフトの最小開始位置 O(N+1)時間・追加領域O(1)

基本の関数
string s="banana";
auto factors=lyndon_factorization(s); // {{0,1},{1,3},{3,5},{5,6}} -> b,an,an,a
ll p=minimum_rotation(s);            // 5
string canonical=s.substr(p)+s.substr(0,p); // abanan
ll tied=minimum_rotation("baba");   // 1（3も同じ文字列だが最小の位置を返す）
*/
