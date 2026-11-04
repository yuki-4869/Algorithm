struct HungarianResult{
    LL cost=0;
    vl column;
};
HungarianResult hungarian(const vvl&cost){
    if(cost.size()>static_cast<size_t>(INT_MAX))std::abort();
    const ll n=static_cast<ll>(cost.size());
    if(n==0)return {};
    if(cost[0].size()>static_cast<size_t>(INT_MAX))std::abort();
    const ll m=static_cast<ll>(cost[0].size());
    if(n>m)std::abort();
    for(const auto&row:cost)if(row.size()!=cost[0].size())std::abort();
    vector<LL> u(n+1),v(m+1);
    vl matched(m+1),previous(m+1);
    for(ll i=1;i<=n;++i){
        matched[0]=i;
        ll column=0;
        vector<LL> slack(m+1);
        vector<char> used(m+1);
        do{
            used[column]=true;
            const ll row=matched[column];
            ll next=0;
            LL delta=0;
            for(ll j=1;j<=m;++j)if(!used[j]){
                const LL reduced=LL(cost[row-1][j-1])-u[row]-v[j];
                if(column==0||reduced<slack[j]){slack[j]=reduced;previous[j]=column;}
                if(next==0||slack[j]<delta){delta=slack[j];next=j;}
            }
            for(ll j=0;j<=m;++j){
                if(used[j]){u[matched[j]]+=delta;v[j]-=delta;}
                else slack[j]-=delta;
            }
            column=next;
        }while(matched[column]!=0);
        do{
            const ll next=previous[column];
            matched[column]=matched[next];
            column=next;
        }while(column!=0);
    }
    HungarianResult result{0,vl(n,-1)};
    for(ll j=1;j<=m;++j)if(matched[j]!=0){
        const ll i=matched[j]-1;
        result.column[i]=j-1;
        result.cost+=LL(cost[i][j-1]);
    }
    return result;
}

/*
細かい注意点
N行M列の完全な費用行列。0<=N<=M<=INT_MAX。空の行列は費用0・空の割当。
行iごとに異なる列を1つ割り当て、費用合計を最小化する。行・列は独立した0-indexed。
全要素はllの範囲全体に対応し、負数も使用可能。合計費用・ポテンシャルはLL=__int128。
入力行列は変更しない。全組が選択可能であり、特別な値による禁止辺の指定は扱わない。
行数が列数を超える・行の長さが揃わない・寸法超過はNDEBUGでもメッセージなしで停止。
最適解が複数ある場合、どれを返すかは保証しない。辞書順最小の割当ではない。
LLの値は通常のcoutへ直接渡せない。必要に応じて128bit整数用の出力処理を用意する。
基本テンプレートのll,LL,vl,vvlを使用し、追加includeやACLへの依存はない。

使える関数一覧
HungarianResult hungarian(cost); // 最小費用と各行の割当列を返す O(1+N*N*M)時間・入力を除くO(N+M+1)領域
LL result.cost;                 // 最小の費用合計
vl result.column;               // result.column[i]が行iの割当先。長さN、列番号はすべて異なる

基本の関数
vvl cost={{4,1,3},{2,0,5}};
auto result=hungarian(cost);
auto assignment=result.column;  // {1,0}
LL minimum=result.cost;          // 3
vvl negative={{-5,2},{4,-3}};
auto best=hungarian(negative);   // 費用-8、割当{0,1}
auto empty=hungarian(vvl{});     // 費用0、割当{}
*/
