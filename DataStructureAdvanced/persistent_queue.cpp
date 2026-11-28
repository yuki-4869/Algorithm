template<class T>
class PersistentQueue {
public:
    class Version {
        friend class PersistentQueue;
        int tail_ = -1;
        int length_ = 0;
        Version(int tail, int length) : tail_(tail), length_(length) {}
    public:
        Version() = default;
    };

    explicit PersistentQueue(int max_pushes)
        : capacity_(max_pushes), levels_(1) {
        if (capacity_ < 0) std::abort();
        levels_ = std::max(1, static_cast<int>(std::bit_width(static_cast<unsigned>(capacity_))));
        if (static_cast<std::size_t>(capacity_) > ancestors_.max_size()/levels_) std::abort();
        values_.reserve(capacity_);
        ancestors_.reserve(static_cast<std::size_t>(capacity_)*levels_);
    }

    int size(Version version) const { return version.length_; }
    bool empty(Version version) const { return version.length_ == 0; }

    Version push(Version version, T value) {
        const int last = static_cast<int>(values_.size());
        if (last == capacity_) std::abort();
        values_.push_back(std::move(value));
        int parent = version.tail_;
        ancestors_.push_back(parent);
        for (int k = 1; k < levels_; ++k) {
            parent = parent < 0 ? -1 : ancestor(parent,k-1);
            ancestors_.push_back(parent);
        }
        return Version(last,version.length_+1);
    }

    Version pop(Version version) const {
        if (empty(version)) std::abort();
        if (version.length_ == 1) return {};
        return Version(version.tail_,version.length_-1);
    }

    T front(Version version) const {
        if (empty(version)) std::abort();
        int node = version.tail_, distance = version.length_-1;
        for (int k = 0; distance > 0; ++k, distance >>= 1) {
            if (distance & 1) node = ancestor(node,k);
        }
        return values_[node];
    }

private:
    int ancestor(int node, int k) const {
        return ancestors_[static_cast<std::size_t>(node)*levels_+k];
    }

    int capacity_, levels_;
    std::vector<T> values_;
    std::vector<int> ancestors_;
};

/*
細かい注意点
Version{}は空の版。push・popの返り値は新しい版で、元の版は再利用できる。
空版を除き、同じPersistentQueueが発行した版を渡す。Pは全版を通したpush回数の上限。
front・popは空版に使えない。frontはTをコピーして返す。計算量はTの操作をO(1)とした場合。

使える関数一覧
PersistentQueue<T> q(P); // 最大P回のpushに備えて記憶域を予約する
PersistentQueue<T>::Version v{}; // 空版 O(1)
q.push(v,x);             // Version: xを末尾に追加した版 O(log(P+2))
q.front(v);              // T: 先頭要素 O(log(P+2))
q.pop(v);                // Version: 先頭を取り除いた版 O(1)
q.size(v);               // int: 要素数 O(1)
q.empty(v);              // bool: 空かどうか O(1)

基本の関数
PersistentQueue<int> q(3);
PersistentQueue<int>::Version empty;
auto a=q.push(empty,10);
auto b=q.push(a,20);
auto c=q.pop(b);
if(q.front(a)!=10 || q.front(b)!=10 || q.front(c)!=20)std::abort();
*/
