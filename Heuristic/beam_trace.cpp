namespace ahc {

template<class Move>
class BeamTrace {
public:
    using NodeId = std::size_t;

    static constexpr NodeId root() { return 0; }
    std::size_t size() const { return nodes_.size(); }

    void reserve(std::size_t count) { nodes_.reserve(count); }
    void clear() { nodes_.clear(); }

    NodeId append(NodeId parent, Move move) {
        if (parent > size()) std::abort();
        nodes_.push_back({parent, std::move(move)});
        return size();
    }

    std::vector<Move> path(NodeId node) const {
        if (node > size()) std::abort();
        std::vector<Move> result;
        while (node != root()) {
            const auto& record = nodes_[node - 1];
            result.push_back(record.move);
            node = record.parent;
        }
        std::reverse(result.begin(), result.end());
        return result;
    }

private:
    struct Node {
        NodeId parent;
        Move move;
    };
    std::vector<Node> nodes_;
};

}

/*
細かい注意点
- 参照URL https://github.com/yuki-4869/Algorithm/blob/main/Heuristic/beam_trace.cpp
- 根は0で操作を持たない。appendへ渡す親は根または登録済みのノード。
- clearすると根以外のIDは無効になる。pathにはコピーできるMove型を使う。
- 候補全体の操作を保存せず、各層を通過した候補だけを記録するとメモリを節約できる。

使える関数一覧
ahc::BeamTrace<Move> trace          空の履歴を作る                         O(1)
ahc::BeamTrace<Move>::NodeId        ノードIDの型
trace.root(), trace.size()         根ID、根を除く登録数                   O(1)
trace.reserve(count)              ノード領域を予約する                   最悪O(size)
trace.clear()                     全履歴を消す                           O(size)
trace.append(parent,move)          操作を保存し新しいIDを返す             償却O(1)
trace.path(id)                    根からidまでの操作列を返す             O(1+経路長)

基本の関数
ahc::BeamTrace<char> trace;
auto first = trace.append(trace.root(),'U');
auto best = trace.append(first,'L');
assert(trace.path(best) == (vector<char>{'U','L'}));
*/
