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
        if (deduplicate_) ensure_capacity(candidate_count);
    }

    void push(std::size_t parent, Move move, Score score, std::uint64_t key = 0) {
        if (width_ == 0) return;
        if (!deduplicate_) {
            candidates_.push_back({parent, std::move(move), score, key});
            return;
        }
        ensure_capacity(candidates_.size() + 1);
        auto& slot = find_slot(table_, key);
        if (slot.layer == layer_) {
            auto& previous = candidates_[slot.index];
            if (previous.score < score) previous = {parent, std::move(move), score, key};
        } else {
            candidates_.push_back({parent, std::move(move), score, key});
            slot = {key, candidates_.size() - 1, layer_};
        }
    }

    std::vector<Candidate> take() {
        std::vector<Candidate> result;
        take_into(result);
        return result;
    }

    void take_into(std::vector<Candidate>& result) {
        auto& indices = indices_;
        indices.resize(candidates_.size());
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
        result.clear();
        result.reserve(indices.size());
        for (auto index : indices) result.push_back(std::move(candidates_[index]));
        clear();
    }

    void clear() {
        candidates_.clear();
        if (++layer_ == 0) {
            std::fill(table_.begin(), table_.end(), Slot{});
            layer_ = 1;
        }
    }

private:
    struct Slot {
        std::uint64_t key = 0;
        std::size_t index = 0;
        std::uint64_t layer = 0;
    };

    Slot& find_slot(std::vector<Slot>& table, std::uint64_t key) const {
        auto hash = key;
        hash = (hash ^ (hash >> 30)) * 0xbf58476d1ce4e5b9ULL;
        hash = (hash ^ (hash >> 27)) * 0x94d049bb133111ebULL;
        hash ^= hash >> 31;
        const std::size_t mask = table.size() - 1;
        auto pos = std::size_t(hash) & mask;
        while (table[pos].layer == layer_ && table[pos].key != key) pos = (pos + 1) & mask;
        return table[pos];
    }

    void ensure_capacity(std::size_t needed) {
        const auto existing = table_.size();
        if (needed <= existing - existing / 4) return;
        auto capacity = std::max(std::size_t(8), existing);
        while (needed > capacity - capacity / 4) {
            if (capacity > table_.max_size() / 2) std::abort();
            capacity *= 2;
        }
        std::vector<Slot> table(capacity);
        for (std::size_t i = 0; i < candidates_.size(); ++i) {
            const auto key = candidates_[i].key;
            find_slot(table, key) = {key, i, layer_};
        }
        table_.swap(table);
    }

    std::size_t width_;
    bool deduplicate_;
    std::uint64_t layer_ = 1;
    std::vector<Candidate> candidates_;
    std::vector<std::size_t> indices_;
    std::vector<Slot> table_;
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
- 同じbeamと出力vectorを使い回すと、take_intoで層ごとの確保を減らせる。

使える関数一覧
ahc::BeamSelection<Move,Score> beam(width,deduplicate=false)  空の候補集合を作る          O(1)
beam.reserve(candidate_count)  候補領域を予約する                 期待O(1+candidate_count+U)
beam.push(parent,move,score,key=0)  1候補を追加する                償却期待O(1)
beam.take()                    上位候補を降順で返し、層を空にする  期待O(1+U+K log K)
beam.take_into(out)            上位候補でoutを上書きして層を空にする  期待O(1+U+K log K+旧out.size())
beam.clear()                   候補を空にする                     O(1+U)
Candidate::parent, move, score, key  親の添字・遷移・評価・同一視キー
Uは保持候補数、K=min(width,U)。clearは層番号が64 bitで一周した時だけ表も初期化する。関数の計算量はMoveとScoreの操作をO(1)とした場合。

基本の関数
ahc::BeamSelection<char> beam(2,true);
beam.push(0,'L',10,101);
beam.push(0,'R',20,102);
beam.push(1,'U',21,102);
vector<ahc::BeamSelection<char>::Candidate> next;
beam.take_into(next);
for(const auto& candidate : next){
    // prev[candidate.parent] に candidate.move を適用して次の状態を作る。
}
*/
