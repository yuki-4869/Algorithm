template<class S, auto op, auto e>
class SwagDeque {
public:
    bool empty() const { return left_.empty() && right_.empty(); }
    std::size_t size() const { return left_.size()+right_.size(); }

    const S& front() const {
        if (empty()) std::abort();
        return left_.empty() ? right_.front().value : left_.back().value;
    }
    const S& back() const {
        if (empty()) std::abort();
        return right_.empty() ? left_.front().value : right_.back().value;
    }

    void push_front(S x) {
        S fold = left_.empty() ? x : op(x,left_.back().fold);
        left_.push_back({std::move(x),std::move(fold)});
    }
    void push_back(S x) {
        S fold = right_.empty() ? x : op(right_.back().fold,x);
        right_.push_back({std::move(x),std::move(fold)});
    }

    void pop_front() {
        if (empty()) std::abort();
        if (left_.empty()) rebalance(true);
        left_.pop_back();
    }
    void pop_back() {
        if (empty()) std::abort();
        if (right_.empty()) rebalance(false);
        right_.pop_back();
    }

    S all_prod() const {
        if (left_.empty()) return right_.empty() ? e() : right_.back().fold;
        if (right_.empty()) return left_.back().fold;
        return op(left_.back().fold,right_.back().fold);
    }

private:
    struct Entry { S value, fold; };
    std::vector<Entry> left_, right_;

    void rebalance(bool need_front) {
        auto& source = need_front ? right_ : left_;
        auto old = std::move(source);
        source.clear();
        const std::size_t n = old.size();
        const std::size_t left_count = (n+static_cast<std::size_t>(need_front))/2;
        auto take = [&](std::size_t i) -> S {
            return std::move(old[need_front ? i : n-1-i].value);
        };
        for (std::size_t i = left_count; i > 0; --i) push_front(take(i-1));
        for (std::size_t i = left_count; i < n; ++i) push_back(take(i));
    }
};

/*
細かい注意点
opは結合則、e()は単位元。可換性は不要。all_prodは先頭から末尾への順序で集約する。
front/back/pop_front/pop_backは空で呼べない。参照は更新前に使い終える。
計算量はSのコピー・ムーブとop/eをO(1)とした場合。領域は保持する最大要素数に比例。

使える関数一覧
SwagDeque<S,op,e> dq; // 空deque O(1)
dq.push_front(x);     // void: 先頭に追加 償却O(1)
dq.push_back(x);      // void: 末尾に追加 償却O(1)
dq.pop_front();       // void: 先頭を削除 償却O(1)
dq.pop_back();        // void: 末尾を削除 償却O(1)
dq.front();          // const S&: 先頭 O(1)
dq.back();           // const S&: 末尾 O(1)
dq.all_prod();       // S: 全体積。空ならe() O(1)
dq.size();           // size_t: 要素数 O(1)
dq.empty();          // bool: 空かどうか O(1)

基本の関数
constexpr auto concat=[](const std::string& a,const std::string& b){return a+b;};
constexpr auto identity=[](){return std::string{};};
SwagDeque<std::string,concat,identity> dq;
dq.push_back("b"); dq.push_front("a"); dq.push_back("c");
dq.pop_front(); dq.pop_front();
if(dq.all_prod()!="c" || dq.front()!="c")std::abort();
*/
