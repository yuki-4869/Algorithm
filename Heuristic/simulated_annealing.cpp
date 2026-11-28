namespace ahc {

class AnnealingSchedule {
public:
    AnnealingSchedule(double start, double finish) : start_(start), finish_(finish) {
        if (!std::isfinite(start) || !std::isfinite(finish) || start < 0 || finish < 0) {
            std::abort();
        }
        if (start > 0 && finish > 0) {
            log_start_ = std::log(start);
            log_finish_ = std::log(finish);
        }
    }

    double operator()(double progress) const {
        if (progress <= 0 || start_ == finish_) return start_;
        if (progress >= 1) return finish_;
        if (start_ == 0 || finish_ == 0) return std::lerp(start_, finish_, progress);
        return std::exp(std::lerp(log_start_, log_finish_, progress));
    }

private:
    double start_, finish_;
    double log_start_ = 0, log_finish_ = 0;
};

class AnnealingLossTable {
public:
    explicit AnnealingLossTable(std::size_t max_loss, double temperature = 0)
        : probabilities_(max_loss), double_thresholds_(max_loss) { set_temperature(temperature); }

    void set_temperature(double temperature) {
        if (!std::isfinite(temperature) || temperature < 0) std::abort();
        temperature_ = temperature;
        for (std::size_t i = 0; i < probabilities_.size(); ++i) {
            const auto exact = compute(i + 1);
            auto round_up = static_cast<double>(exact);
            if (static_cast<long double>(round_up) < exact) {
                round_up = std::nextafter(round_up, 1.0);
            }
            probabilities_[i] = exact;
            double_thresholds_[i] = round_up;
        }
    }

    double temperature() const { return temperature_; }
    std::size_t max_loss() const { return probabilities_.size(); }

    long double probability(std::uint64_t loss) const {
        if (loss == 0) return 1;
        if (loss <= max_loss()) return probabilities_[loss - 1];
        return compute(loss);
    }

private:
    template<class State, class Score> friend class Annealing;

    template<class Sample>
    bool accepts_loss(std::uint64_t loss, Sample& sample) const {
        using Value = std::remove_cvref_t<std::invoke_result_t<Sample&>>;
        if constexpr (std::is_same_v<Value,double> || std::is_same_v<Value,float>) {
            if (loss <= max_loss()) {
                return std::invoke(sample) < double_thresholds_[loss - 1];
            }
        }
        return std::invoke(sample) < probability(loss);
    }

    long double compute(std::uint64_t loss) const {
        if (temperature_ == 0) return 0;
        return std::exp(-static_cast<long double>(loss) / temperature_);
    }

    std::vector<long double> probabilities_;
    std::vector<double> double_thresholds_;
    double temperature_ = 0;
};

template<class State, class Score>
class Annealing {
public:
    Annealing(State initial_state, Score initial_score)
        : state_(std::move(initial_state)), best_state_(state_),
          score_(initial_score), best_score_(initial_score) {}

    const State& state() const { return state_; }
    Score score() const { return score_; }
    const State& best_state() const { return best_state_; }
    Score best_score() const { return best_score_; }

    template<class Sample, class Apply>
    bool try_move(Score delta, double temperature, Sample&& sample, Apply&& apply) {
        if (delta < Score(0)) {
            if (temperature <= 0) return false;
            const auto probability = std::exp(static_cast<long double>(delta) / temperature);
            if (!(std::invoke(sample) < probability)) return false;
        }
        apply_move(delta, apply);
        return true;
    }

    template<class Sample, class Apply>
    bool try_move(Score delta, const AnnealingLossTable& table, Sample&& sample, Apply&& apply)
        requires (std::signed_integral<Score> && sizeof(Score) <= sizeof(std::uint64_t))
    {
        if (delta < Score(0)) {
            if (table.temperature() == 0) return false;
            const auto loss = std::uint64_t(0) - std::uint64_t(delta);
            if (!table.accepts_loss(loss, sample)) return false;
        }
        apply_move(delta, apply);
        return true;
    }

private:
    template<class Apply>
    void apply_move(Score delta, Apply& apply) {
        std::invoke(apply, state_);
        score_ += delta;
        if (best_score_ < score_) {
            best_state_ = state_;
            best_score_ = score_;
        }
    }

    State state_, best_state_;
    Score score_, best_score_;
};

}

/*
細かい注意点
- 参照URL https://github.com/yuki-4869/Algorithm/blob/main/Heuristic/simulated_annealing.cpp
- スコア最大化。delta=変更後-変更前。最小化では評価値の符号を反転する。
- 差分と総和が表現できる符号付き型を使う。浮動小数のスコア・差分・経過率は有限。温度は有限かつ非負。
- sample() は [0,1) の一様乱数。apply(state) は受理時だけ呼ばれる。
- 温度0なら非悪化手のみ受理する。同点の最良状態は初出を保持する。
- Stateは独立したコピーを作れる型が必要。最良更新時にStateをコピーする。
- 符号付き64 bit以下の整数スコアはAnnealingLossTableで悪化量1..max_lossの受理確率を使い回せる。
- 表の温度はバッチごとに更新。表の範囲外は従来通りexpを計算する。
- sampleの戻り値がdouble/floatなら、丸め方向をそろえた比較値を使い受理結果を保つ。

使える関数一覧
ahc::AnnealingSchedule schedule(start,finish)  温度スケジュールを構築               O(1)
schedule(progress)                             経過率での温度を返す                 O(1)
ahc::AnnealingLossTable table(max_loss,temp=0)  悪化手の受理確率を前計算           O(1+max_loss)
table.set_temperature(temp)                    同じ表サイズで確率を再計算        O(1+max_loss)
table.temperature(), table.max_loss()          温度と表の最大悪化量               O(1)
table.probability(loss)                        0なら1、悪化量lossの受理確率       O(1)
ahc::Annealing sa(state,score)                  初期状態・初期スコアを設定           状態コピー1回
sa.state(), sa.score()                         現在の状態へのconst参照・スコア      O(1)
sa.best_state(), sa.best_score()               最良状態へのconst参照・スコア        O(1)
sa.try_move(delta,temp,sample,apply)            近傍を受理すれば適用しtrueを返す      O(1)+コールバック・最良更新時のコピー
sa.try_move(delta,table,sample,apply)           整数スコアの表参照版                同上

基本の関数
ahc::Annealing sa(vector<int>{1,0},0LL);
ahc::AnnealingSchedule schedule(2.0,0.1);
mt19937_64 engine(42);
auto sample = [&] { return generate_canonical<double,53>(engine); };
sa.try_move(1LL,schedule(0.0),sample,[](auto& a){ swap(a[0],a[1]); });
auto answer = sa.best_state();
*/
