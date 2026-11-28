namespace ahc {

class Timer {
    using Clock = std::chrono::steady_clock;

public:
    explicit Timer(double limit_ms) : start_(Clock::now()), limit_ms_(limit_ms) {
        if (!std::isfinite(limit_ms) || limit_ms < 0) std::abort();
    }

    void reset() { start_ = Clock::now(); }
    double limit_ms() const { return limit_ms_; }

    double elapsed_ms() const {
        return std::chrono::duration<double, std::milli>(Clock::now() - start_).count();
    }

    double remaining_ms() const { return std::max(0.0, limit_ms_ - elapsed_ms()); }
    bool expired() const { return elapsed_ms() >= limit_ms_; }

    double progress() const {
        if (limit_ms_ == 0) return 1;
        return std::min(1.0, elapsed_ms() / limit_ms_);
    }

private:
    Clock::time_point start_;
    double limit_ms_;
};

}

/*
細かい注意点
- 参照URL https://github.com/yuki-4869/Algorithm/blob/main/Heuristic/timer.cpp
- steady_clockの実時間をミリ秒で測る。制限時間に入力・出力も含めるなら、入力前に作る。
- limit_ms は有限かつ非負。0なら作成直後から期限切れ。
- まとめて進めるループでは、1バッチの最悪時間と出力時間を余裕に含める。

使える関数一覧
ahc::Timer timer(limit_ms)   現在時刻から計測を開始する            O(1)
timer.reset()               同じ予算で現在時刻から再計測する      O(1)
timer.limit_ms()            設定した予算を返す                    O(1)
timer.elapsed_ms()          経過時間をミリ秒で返す                O(1)
timer.remaining_ms()        残り時間をミリ秒で返す                O(1)
timer.expired()             期限に達していればtrue                O(1)
timer.progress()            経過率を [0,1] で返す                 O(1)

基本の関数
ahc::Timer timer(1);
while (!timer.expired()) {
    const double progress = timer.progress();
    // 近傍を何回か試す。バッチサイズは1試行の重さに合わせる。
}
*/
