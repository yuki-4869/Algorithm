unsigned __int128 rectangle_union_area(const vector<tu4>& rectangles){
    if(rectangles.size()>static_cast<size_t>(INT_MAX))std::abort();
    struct Event{ll x,down,up;int delta;};
    vector<Event> events;
    vl ys;
    for(auto [left,down,right,up]:rectangles){
        if(left>right||down>up)std::abort();
        if(left==right||down==up)continue;
        events.push_back({left,down,up,1});
        events.push_back({right,down,up,-1});
        ys.push_back(down);
        ys.push_back(up);
    }
    if(events.empty())return 0;
    sort(events.begin(),events.end(),[](const Event& a,const Event& b){return a.x<b.x;});
    sort(ys.begin(),ys.end());
    ys.erase(unique(ys.begin(),ys.end()),ys.end());

    struct Node{unsigned long long length=0;int cover=0;};
    vector<Node> tree(4*(ys.size()-1));
    auto update=[&](auto&& self,size_t v,size_t l,size_t r,size_t ql,size_t qr,int delta)->void{
        auto& node=tree[v];
        if(ql<=l&&r<=qr){
            node.cover+=delta;
        }else{
            const size_t mid=(l+r)/2;
            if(ql<mid)self(self,2*v,l,mid,ql,qr,delta);
            if(mid<qr)self(self,2*v+1,mid,r,ql,qr,delta);
        }
        if(node.cover>0)node.length=static_cast<unsigned long long>(LL(ys[r])-ys[l]);
        else if(r-l==1)node.length=0;
        else node.length=tree[2*v].length+tree[2*v+1].length;
    };

    unsigned __int128 area=0;
    ll prev_x=events.front().x;
    for(const auto& [x,down,up,delta]:events){
        area+=static_cast<unsigned __int128>(LL(x)-prev_x)*tree[1].length;
        prev_x=x;
        const size_t ql=lower_bound(ys.begin(),ys.end(),down)-ys.begin();
        const size_t qr=lower_bound(ys.begin(),ys.end(),up)-ys.begin();
        update(update,1,0,ys.size()-1,ql,qr,delta);
    }
    return area;
}

/*
細かい注意点
各長方形はtu4{left,down,right,up}で、領域は[left,right)×[down,up)。
端点はllの全範囲。幅または高さが0の長方形は面積に寄与しない。
左右・上下が逆、または長方形の個数がINT_MAXを超えるとメッセージなしで停止する。
重複する領域は1回だけ数える。空入力の面積は0。入力は変更しない。
基本テンプレートのll・LL・vl・tu4を使う。追加include・ACLの機能は不要。
返り値は符号なし128 bit整数。llに収まる場合は変換して出力できる。

使える関数一覧
unsigned __int128 rectangle_union_area(rectangles); // vector<tu4>の和集合面積。O(N log N)時間・O(N)追加領域

基本の関数
vector<tu4> rectangles={{0,0,3,2},{2,1,5,4},{0,0,3,2}};
auto area=rectangle_union_area(rectangles); // 6+9-1=14
assert(area==14);
cout<<static_cast<ll>(area)<<'\n';
*/
