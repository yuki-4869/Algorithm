namespace ahc {

template<class T>
class RollbackArray {
public:
    using Snapshot = std::size_t;

    explicit RollbackArray(std::size_t n, T initial = T{}) : values_(n, initial) {}
    explicit RollbackArray(std::vector<T> values) : values_(std::move(values)) {}

    std::size_t size() const { return values_.size(); }
    T get(std::size_t index) const { return values_[index]; }
    Snapshot snapshot() const { return history_.size(); }

    void set(std::size_t index, T value) {
        history_.emplace_back(index, values_[index]);
        values_[index] = std::move(value);
    }

    void rollback(Snapshot version) {
        if (version > history_.size()) std::abort();
        while (history_.size() > version) {
            const auto& [index, value] = history_.back();
            values_[index] = value;
            history_.pop_back();
        }
    }

    void clear_history() { history_.clear(); }

private:
    std::vector<T> values_;
    std::vector<std::pair<std::size_t, T>> history_;
};

}

/*
細かい注意点
- 参照URL https://github.com/yuki-4869/Algorithm/blob/main/Heuristic/rollback_array.cpp
- 添字は0-indexedで [0,size())。同じ値を代入しても履歴1件を積む。
- snapshotは現在の履歴上にある間だけ有効。rollbackで捨てた先のsnapshotを再利用しない。
- clear_history後は既存のsnapshotを全て破棄する。Tにはコピーが軽い値型を使う。

使える関数一覧
ahc::RollbackArray<T> a(n,initial=T{})   同じ値で初期化              O(N)
ahc::RollbackArray a(vector<T>)         列で初期化                  コピーO(N)、moveならO(1)
ahc::RollbackArray<T>::Snapshot        スナップショットの型
a.size(), a.get(i)                     要素数、値のコピー          O(1)
a.snapshot()                           現時点の印を返す            O(1)
a.set(i,x)                             変更前の値を記録して代入    償却O(1)
a.rollback(snap)                       snap以降の更新を取り消す   O(取り消した回数)
a.clear_history()                      現在値を残して全履歴を破棄  O(履歴件数)

基本の関数
ahc::RollbackArray data(vector<int>{1,2,3});
auto here = data.snapshot();
data.set(0,4);
data.set(1,5);
data.rollback(here);
assert(data.get(0) == 1 && data.get(1) == 2);
*/
