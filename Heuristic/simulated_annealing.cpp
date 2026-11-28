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
        std::invoke(apply, state_);
        score_ += delta;
        if (best_score_ < score_) {
            best_state_ = state_;
            best_score_ = score_;
        }
        return true;
    }

private:
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

使える関数一覧
ahc::AnnealingSchedule schedule(start,finish)  温度スケジュールを構築               O(1)
schedule(progress)                             経過率での温度を返す                 O(1)
ahc::Annealing sa(state,score)                  初期状態・初期スコアを設定           状態コピー1回
sa.state(), sa.score()                         現在の状態へのconst参照・スコア      O(1)
sa.best_state(), sa.best_score()               最良状態へのconst参照・スコア        O(1)
sa.try_move(delta,temp,sample,apply)            近傍を受理すれば適用しtrueを返す      O(1)+コールバック・最良更新時のコピー

基本の関数
ahc::Annealing sa(vector<int>{1,0},0LL);
ahc::AnnealingSchedule schedule(2.0,0.1);
mt19937_64 engine(42);
auto sample = [&] { return generate_canonical<double,53>(engine); };
sa.try_move(1LL,schedule(0.0),sample,[](auto& a){ swap(a[0],a[1]); });
auto answer = sa.best_state();
*/
