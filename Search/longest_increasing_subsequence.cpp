struct LISResult{
    ll length=0;
    vl indices,ending_length;
};
LISResult longest_increasing_subsequence(const vl&a,bool strict=true){
    if(a.size()>static_cast<size_t>(INT_MAX))std::abort();
    const ll n=static_cast<ll>(a.size());
    LISResult result;
    result.ending_length.resize(n);
    vl tails,last,previous(n,-1);
    tails.reserve(n);last.reserve(n);
    for(ll i=0;i<n;++i){
        auto it=strict?lower_bound(tails.begin(),tails.end(),a[i]):upper_bound(tails.begin(),tails.end(),a[i]);
        const ll k=it-tails.begin();
        if(k>0)previous[i]=last[k-1];
        result.ending_length[i]=k+1;
        if(it==tails.end()){tails.push_back(a[i]);last.push_back(i);}
        else{*it=a[i];last[k]=i;}
    }
    result.length=static_cast<ll>(tails.size());
    result.indices.reserve(result.length);
    for(ll i=last.empty()?-1:last.back();i!=-1;i=previous[i])result.indices.push_back(i);
    reverse(result.indices.begin(),result.indices.end());
    return result;
}

/*
細かい注意点
ll,vlは基本テンプレートの型エイリアス。N=a.size()はINT_MAX以下。追加includeは不要。
長さの超過時はNDEBUGでもメッセージなしで停止。入力列は変更しない。
strict=trueは狭義増加(<)、falseは非減少(<=)。部分列は飛び飛びの要素を選べる。
全ll値・負数・重複に対応。番兵値や値の加減算を使わない。
indicesは元の列の0-indexed添字を昇順で返す。複数の最適解のうち1つで、辞書順最小は保証しない。
ending_length[i]は位置iで終わる最長長。a[0..i]全体の最長長はその区間の最大値。
空ならlength=0、indicesとending_lengthは空。返り値は入力と独立した値。

使える関数一覧
LISResult longest_increasing_subsequence(a,strict=true); // 最長部分列を復元 O(N log N)時間・追加O(N)領域
result.length;           // ll: 最長長 O(1)
result.indices;          // vl: 長さlengthの復元添字、1要素の参照O(1)、全走査O(length)
result.ending_length;    // vl: 各位置で終わる最長長、1要素の参照O(1)、全走査O(N)

基本の関数
vl a={3,1,2,2,4};
auto result=longest_increasing_subsequence(a);
assert(result.length==3);
assert((result.ending_length==vl{1,1,2,2,3}));
vl picked;
for(ll i:result.indices)picked.push_back(a[i]); // 値の列へ変換 O(length)
assert((picked==vl{1,2,4}));
auto nondecreasing=longest_increasing_subsequence(a,false);
assert(nondecreasing.length==4);
assert((nondecreasing.indices==vl{1,2,3,4}));
auto empty=longest_increasing_subsequence(vl{});
assert(empty.length==0 && empty.indices.empty() && empty.ending_length.empty());
*/
