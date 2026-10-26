vector<pll> convex_hull(vector<pll> points,bool keep_collinear=false){
    for(auto [x,y]:points)if(x<-1000000000000000000LL||x>1000000000000000000LL||y<-1000000000000000000LL||y>1000000000000000000LL)std::abort();
    sort(points.begin(),points.end());
    points.erase(unique(points.begin(),points.end()),points.end());
    if(points.size()<=2)return points;
    auto turn=[](const pll&a,const pll&b,const pll&c)->LL{
        return (LL(b.first)-a.first)*(LL(c.second)-a.second)-(LL(b.second)-a.second)*(LL(c.first)-a.first);
    };
    bool collinear=true;
    for(const auto&p:points)if(turn(points.front(),points.back(),p)!=0){collinear=false;break;}
    if(collinear){
        if(keep_collinear)return points;
        return {points.front(),points.back()};
    }
    vector<pll> lower,upper;
    auto append=[&](vector<pll>&chain,const pll&p){
        while(chain.size()>=2){
            LL cross=turn(chain[chain.size()-2],chain.back(),p);
            if(cross>0||(keep_collinear&&cross==0))break;
            chain.pop_back();
        }
        chain.push_back(p);
    };
    for(const auto&p:points)append(lower,p);
    for(auto it=points.rbegin();it!=points.rend();++it)append(upper,*it);
    lower.pop_back();
    upper.pop_back();
    lower.insert(lower.end(),upper.begin(),upper.end());
    return lower;
}

/*
細かい注意点
各点はpll=(x,y)。座標は整数で各成分の絶対値が10^18以下。範囲外はメッセージなしで停止。
ll,LL,pllは基本テンプレートの型エイリアスを使用。追加includeは不要。
入力は値渡しで元の列を変更しない。重複座標は1点にまとめ、元の添字は返さない。
辞書順最小の点から反時計回り。末尾に始点を重ねない。返り値の添字は0-indexed。
keep_collinear=falseは辺の途中の点を除外、trueは境界上の入力点をすべて残す。
全点が一直線ならfalseは両端、trueは辞書順の全点。空入力は空、同一点のみなら1点。
浮動小数点・誤差判定・点の動的追加削除には非対応。

使える関数一覧
vector<pll> convex_hull(points,keep_collinear=false); // 凸包の座標列を返す O(1+N log(N+1))時間・O(N+1)領域

基本の関数
vector<pll> points={{0,0},{1,0},{2,0},{2,2},{0,2},{1,1},{0,0}};
auto hull=convex_hull(points);      // {{0,0},{2,0},{2,2},{0,2}}
auto boundary=convex_hull(points,true); // {{0,0},{1,0},{2,0},{2,2},{0,2}}
*/
