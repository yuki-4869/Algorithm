class PersistentUnionFind {
public:
    class Version {
        int root_ = 0;
        explicit Version(int root) : root_(root) {}
        friend class PersistentUnionFind;
    public:
        Version() = default;
    };

    explicit PersistentUnionFind(int n) : n_(n) {
        if (n < 0) std::abort();
    }

    int leader(Version v, int x) const {
        if (x < 0 || x >= n_) std::abort();
        int parent;
        while ((parent = get(v.root_,x)) >= 0) x = parent;
        return x;
    }

    int size(Version v, int x) const { return -get(v.root_,leader(v,x)); }
    bool same(Version v, int x, int y) const { return leader(v,x) == leader(v,y); }

    Version merge(Version v, int a, int b) {
        a = leader(v,a);
        b = leader(v,b);
        if (a == b) return v;
        int size_a = -get(v.root_,a), size_b = -get(v.root_,b);
        if (size_a < size_b) {
            std::swap(a,b);
            std::swap(size_a,size_b);
        }
        int root = set(v.root_,0,n_,a,-size_a-size_b);
        return Version(set(root,0,n_,b,a));
    }

private:
    struct Node { int left = 0, right = 0; };
    int n_;
    std::vector<Node> nodes_{Node{}};

    int get(int node, int index) const {
        int lo = 0, hi = n_;
        while (node != 0 && hi-lo > 1) {
            const int mid = lo+(hi-lo)/2;
            if (index < mid) {
                hi = mid;
                node = nodes_[node].left;
            } else {
                lo = mid;
                node = nodes_[node].right;
            }
        }
        return node == 0 ? -1 : nodes_[node].left;
    }

    int set(int node, int lo, int hi, int index, int value) {
        if (nodes_.size() == static_cast<std::size_t>(INT_MAX)) std::abort();
        const int copy = static_cast<int>(nodes_.size());
        nodes_.push_back(nodes_[node]);
        if (hi-lo == 1) nodes_[copy].left = value;
        else {
            const int mid = lo+(hi-lo)/2;
            if (index < mid) nodes_[copy].left = set(nodes_[copy].left,lo,mid,index,value);
            else nodes_[copy].right = set(nodes_[copy].right,mid,hi,index,value);
        }
        return copy;
    }
};

/*
細かい注意点
頂点は0始まり。PersistentUnionFind::Version{}は全頂点が孤立した版。
初期版を除き、同じUnion-Findが発行した版を渡す。merge先の版は返り値として保存する。
成分サイズによる併合と永続配列を使用。時間はL=log(N+2)で表す。

使える関数一覧
PersistentUnionFind uf(n);         // 全頂点を初期化する管理オブジェクト O(1)
PersistentUnionFind::Version v{};  // 初期版 O(1)
uf.merge(v,a,b);                   // Version: 版vでa,bを結んだ版 O(L^2)、O(L)追加領域
uf.leader(v,a);                    // int: 代表頂点 O(L^2)
uf.same(v,a,b);                    // bool: 連結か O(L^2)
uf.size(v,a);                      // int: 成分サイズ O(L^2)

基本の関数
PersistentUnionFind uf(3);
PersistentUnionFind::Version empty;
auto a=uf.merge(empty,0,1), b=uf.merge(empty,1,2);
if(!uf.same(a,0,1) || uf.same(a,0,2) || uf.size(b,1)!=2 || uf.same(empty,0,1))std::abort();
*/
