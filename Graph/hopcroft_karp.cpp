class HopcroftKarp{
    int n_left_,n_right_,cardinality_=0;
    bool solved_=false;
    std::vector<std::vector<int>> graph_;
    std::vector<int> left_,right_;
    static int checked_size_(int n){if(n<0)std::abort();return n;}
    int layers_(std::vector<int>&dist,std::vector<int>&queue)const{
        std::fill(dist.begin(),dist.end(),-1);
        queue.clear();
        for(int u=0;u<n_left_;++u)if(left_[u]==-1){dist[u]=0;queue.push_back(u);}
        int shortest=-1;
        for(std::size_t head=0;head<queue.size();++head){
            const int u=queue[head];
            if(shortest!=-1&&dist[u]>=shortest)continue;
            for(int v:graph_[u]){
                const int next=right_[v];
                if(next==-1)shortest=dist[u]+1;
                else if(dist[next]==-1){dist[next]=dist[u]+1;queue.push_back(next);}
            }
        }
        return shortest;
    }
    bool augment_(int root,int shortest,std::vector<int>&dist,std::vector<std::size_t>&next_edge,std::vector<int>&path){
        path.clear();
        path.push_back(root);
        while(!path.empty()){
            const int u=path.back();
            bool advanced=false;
            while(next_edge[u]<graph_[u].size()){
                int v=graph_[u][next_edge[u]++];
                const int next=right_[v];
                if(next==-1){
                    if(dist[u]+1!=shortest)continue;
                    for(std::size_t i=path.size();i>0;--i){
                        const int a=path[i-1],old=left_[a];
                        left_[a]=v;
                        right_[v]=a;
                        v=old;
                    }
                    return true;
                }
                if(dist[next]==dist[u]+1&&dist[next]<shortest){
                    path.push_back(next);
                    advanced=true;
                    break;
                }
            }
            if(!advanced){dist[u]=-1;path.pop_back();}
        }
        return false;
    }
public:
    HopcroftKarp():HopcroftKarp(0,0){}
    HopcroftKarp(int n_left,int n_right)
        :n_left_(checked_size_(n_left)),n_right_(checked_size_(n_right)),graph_(n_left_),left_(n_left_,-1),right_(n_right_,-1){}
    HopcroftKarp(const HopcroftKarp&)=default;
    HopcroftKarp&operator=(const HopcroftKarp&)=default;
    HopcroftKarp(HopcroftKarp&&other)noexcept
        :n_left_(std::exchange(other.n_left_,0)),n_right_(std::exchange(other.n_right_,0)),
         cardinality_(std::exchange(other.cardinality_,0)),solved_(std::exchange(other.solved_,true)),
         graph_(std::move(other.graph_)),left_(std::move(other.left_)),right_(std::move(other.right_)){}
    HopcroftKarp&operator=(HopcroftKarp&&other)noexcept{
        if(this!=&other){
            n_left_=std::exchange(other.n_left_,0);
            n_right_=std::exchange(other.n_right_,0);
            cardinality_=std::exchange(other.cardinality_,0);
            solved_=std::exchange(other.solved_,true);
            graph_=std::move(other.graph_);
            left_=std::move(other.left_);
            right_=std::move(other.right_);
        }
        return *this;
    }
    int left_size()const{return n_left_;}
    int right_size()const{return n_right_;}
    void add_edge(int u,int v){
        if(u<0||u>=n_left_||v<0||v>=n_right_)std::abort();
        graph_[u].push_back(v);
        solved_=false;
    }
    int max_matching(){
        if(solved_||cardinality_==std::min(n_left_,n_right_)){solved_=true;return cardinality_;}
        std::vector<int>dist(n_left_),queue,path;
        std::vector<std::size_t>next_edge(n_left_);
        queue.reserve(n_left_);
        path.reserve(n_left_);
        for(int shortest;(shortest=layers_(dist,queue))!=-1;){
            std::fill(next_edge.begin(),next_edge.end(),0);
            for(int u=0;u<n_left_;++u)if(left_[u]==-1&&augment_(u,shortest,dist,next_edge,path))++cardinality_;
        }
        solved_=true;
        return cardinality_;
    }
    int mate_left(int u){
        if(u<0||u>=n_left_)std::abort();
        max_matching();
        return left_[u];
    }
    int mate_right(int v){
        if(v<0||v>=n_right_)std::abort();
        max_matching();
        return right_[v];
    }
    std::vector<std::pair<int,int>> matching(){
        max_matching();
        std::vector<std::pair<int,int>> result;
        result.reserve(cardinality_);
        for(int u=0;u<n_left_;++u)if(left_[u]!=-1)result.emplace_back(u,left_[u]);
        return result;
    }
    std::pair<std::vector<int>,std::vector<int>> min_vertex_cover(){
        max_matching();
        std::vector<char>seen_left(n_left_),seen_right(n_right_);
        std::vector<int>queue,cover_left,cover_right;
        for(int u=0;u<n_left_;++u)if(left_[u]==-1){seen_left[u]=true;queue.push_back(u);}
        for(std::size_t head=0;head<queue.size();++head){
            const int u=queue[head];
            for(int v:graph_[u]){
                if(left_[u]==v||seen_right[v])continue;
                seen_right[v]=true;
                const int next=right_[v];
                if(next!=-1&&!seen_left[next]){seen_left[next]=true;queue.push_back(next);}
            }
        }
        for(int u=0;u<n_left_;++u)if(!seen_left[u])cover_left.push_back(u);
        for(int v=0;v<n_right_;++v)if(seen_right[v])cover_right.push_back(v);
        return {cover_left,cover_right};
    }
};

/*
細かい注意点
左右の頂点番号はそれぞれ0-indexed。左NL個・右NR個の二部グラフを明示して渡す。0<=NL,NR<=INT_MAX。
無重みの最大マッチングをHopcroft-Karp法で求める。多重辺・孤立点・左右0頂点にも対応。
N=NL+NR、Mは多重辺を含む追加辺数。探索は再帰を使わず、保持領域はO(N+M)。
add_edge後も既存の対応を残し、次の取得時に最大化する。削除・重み付き・一般グラフには非対応。
max_matchingは追加分ではなく現在の最大対応数を返す。取得APIも必要なら自動で最大化する。
最大化は1回につき最悪O(1+(N+M)*sqrt(N+1))。変更なしの再呼び出しはO(1)。追加後の複数回分は別々に数える。
対応と最小頂点被覆は一意とは限らない。matchingは左番号順、被覆の左右の配列は各番号順。
コピーは独立な複製O(N+M)。ムーブ元は空。代入には以前の領域の解放コストが加わる。
負の頂点数・範囲外添字はNDEBUGでもメッセージなしで停止。基本テンプレート以外のincludeは不要。

使える関数一覧
HopcroftKarp hk;                      // 左右0頂点で構築 O(1)
HopcroftKarp hk(NL,NR);               // 左右の頂点数を指定して構築 O(N+1)
int hk.left_size();                  // 左の頂点数を返す O(1)
int hk.right_size();                 // 右の頂点数を返す O(1)
void hk.add_edge(u,v);                // 左uと右vの辺を追加 償却O(1)
int hk.max_matching();               // 最大対応数を返す 上記最大化の計算量
int hk.mate_left(u);                 // 左uの相手の右番号、未対応なら-1 最大化後O(1)
int hk.mate_right(v);                // 右vの相手の左番号、未対応なら-1 最大化後O(1)
vector<pair<int,int>> hk.matching();  // 最大マッチングの(左,右)一覧 最大化後O(NL+1)
pair<vector<int>,vector<int>> hk.min_vertex_cover(); // 最小頂点被覆の(左番号列,右番号列) 最大化後O(N+M+1)

基本の関数
HopcroftKarp hk(3,3);
hk.add_edge(0,0);
hk.add_edge(0,1);
hk.add_edge(1,0);
auto first=hk.max_matching();         // 2
hk.add_edge(2,2);
auto count=hk.max_matching();         // 合計3。追加分の1ではない
auto pairs=hk.matching();             // {{0,1},{1,0},{2,2}}
auto partner=hk.mate_left(0);         // 1
auto reverse_partner=hk.mate_right(0);// 1
auto [cover_left,cover_right]=hk.min_vertex_cover(); // この例では左{0,1,2}、右{}
*/
