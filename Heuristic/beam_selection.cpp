namespace ahc {

template<class Move, class Score = long long>
class BeamSelection {
public:
    struct Candidate {
        std::size_t parent;
        Move move;
        Score score;
        std::uint64_t key;
    };

    explicit BeamSelection(std::size_t width, bool deduplicate = false)
        : width_(width), deduplicate_(deduplicate) {}

    void reserve(std::size_t candidate_count) {
        candidates_.reserve(candidate_count);
        if (deduplicate_) by_key_.reserve(candidate_count);
    }

    void push(std::size_t parent, Move move, Score score, std::uint64_t key = 0) {
        if (width_ == 0) return;
        if (deduplicate_) {
            auto [where, inserted] = by_key_.try_emplace(key, candidates_.size());
            if (!inserted) {
                auto& previous = candidates_[where->second];
                if (previous.score < score) previous = {parent, std::move(move), score, key};
                return;
            }
        }
        candidates_.push_back({parent, std::move(move), score, key});
    }

    std::vector<Candidate> take() {
        std::vector<std::size_t> indices(candidates_.size());
        std::iota(indices.begin(), indices.end(), std::size_t(0));
        auto better = [&](std::size_t lhs, std::size_t rhs) {
            if (candidates_[rhs].score < candidates_[lhs].score) return true;
            if (candidates_[lhs].score < candidates_[rhs].score) return false;
            return lhs < rhs;
        };
        if (indices.size() > width_) {
            std::nth_element(indices.begin(), indices.begin() + width_, indices.end(), better);
            indices.resize(width_);
        }
        std::sort(indices.begin(), indices.end(), better);
        std::vector<Candidate> result;
        result.reserve(indices.size());
        for (auto index : indices) result.push_back(std::move(candidates_[index]));
        clear();
        return result;
    }

    void clear() {
        candidates_.clear();
        by_key_.clear();
    }

private:
    std::size_t width_;
    bool deduplicate_;
    std::vector<Candidate> candidates_;
    std::unordered_map<std::uint64_t, std::size_t> by_key_;
};

}

/*
細かい注意点
- 参照URL https://github.com/yuki-4869/Algorithm/blob/main/Heuristic/beam_selection.cpp
- 1層ずつ使用する。parentは前層の0-indexedの位置。スコア最大化。
- 重複排除を有効にしたら、同一視できる状態に同じkeyを渡す。keyの省略値は0。
- 同一キー内で同点なら先着。同スコアの異なるキーは、そのキーの最初の登録順。
- 候補に保存するのはスコア・親・遷移情報。状態の生成と手順の復元は呼び出し側で行う。
- 浮動小数のスコアにNaNを渡さない。Moveには小さい値型を推奨する。

使える関数一覧
ahc::BeamSelection<Move,Score> beam(width,deduplicate=false)  空の候補集合を作る          O(1)
beam.reserve(candidate_count)  候補領域を予約する                 期待O(U)
beam.push(parent,move,score,key=0)  1候補を追加する                償却期待O(1)
beam.take()                    上位候補を降順で返し、層を空にする  期待O(U+K log K)
beam.clear()                   候補を空にする                     O(U)
Candidate::parent, move, score, key  親の添字・遷移・評価・同一視キー
Uは保持候補数、K=min(width,U)。関数の計算量はMoveとScoreの操作をO(1)とした場合。

基本の関数
ahc::BeamSelection<char> beam(2,true);
beam.push(0,'L',10,101);
beam.push(0,'R',20,102);
beam.push(1,'U',21,102);
for(const auto& candidate : beam.take()){
    // prev[candidate.parent] に candidate.move を適用して次の状態を作る。
}
*/
