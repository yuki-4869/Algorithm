namespace ahc {

class IndexedSet {
public:
    explicit IndexedSet(int universe) {
        if (universe < 0) std::abort();
        positions_.assign(universe, -1);
    }

    int size() const { return int(values_.size()); }
    bool empty() const { return values_.empty(); }
    std::span<const int> values() const { return values_; }

    bool contains(int value) const { return positions_[value] >= 0; }

    bool insert(int value) {
        if (contains(value)) return false;
        values_.push_back(value);
        positions_[value] = size() - 1;
        return true;
    }

    bool erase(int value) {
        if (!contains(value)) return false;
        const int index = positions_[value];
        const int last = values_.back();
        values_[index] = last;
        positions_[last] = index;
        values_.pop_back();
        positions_[value] = -1;
        return true;
    }

    void clear() {
        for (int value : values_) positions_[value] = -1;
        values_.clear();
    }

private:
    std::vector<int> positions_, values_;
};

}

/*
細かい注意点
- 参照URL https://github.com/yuki-4869/Algorithm/blob/main/Heuristic/indexed_set.cpp
- 対象は整数 [0,universe)。添字は0-indexed。削除は末尾の値と入れ替えるため列順は変わる。
- values() は内部列を参照する。変更後は取り直す。一様な要素を選ぶときは非空を確認する。

使える関数一覧
ahc::IndexedSet active(universe)  空集合を作る                  O(universe)
active.size(), active.empty()   要素数・空かどうか            O(1)
active.values()                 現在の全要素をspanで返す      O(1)
active.contains(x)              所属していればtrue            O(1)
active.insert(x)                新しく追加できればtrue        償却O(1)
active.erase(x)                 要素が存在し削除すればtrue    O(1)
active.clear()                  全要素を消す                  O(size)

基本の関数
ahc::IndexedSet active(10);
active.insert(2);
active.insert(8);
active.erase(2);
assert(active.values()[0] == 8);
*/
