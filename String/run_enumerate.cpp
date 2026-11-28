namespace run_enumerate_detail {
class Lce {
public:
    explicit Lce(const std::vector<int>& text) : rank_(text.size()) {
        if (text.empty()) return;
        const auto suffix = atcoder::suffix_array(text,255);
        for (int i = 0; i < static_cast<int>(suffix.size()); ++i) rank_[suffix[i]] = i;
        table_.push_back(atcoder::lcp_array(text,suffix));
        const int m = static_cast<int>(table_[0].size());
        for (int half = 1; half <= m/2; half *= 2) {
            const auto& prev = table_.back();
            std::vector<int> row(m-2*half+1);
            for (int i = 0; i < static_cast<int>(row.size()); ++i) {
                row[i] = std::min(prev[i],prev[i+half]);
            }
            table_.push_back(std::move(row));
        }
    }

    const std::vector<int>& ranks() const { return rank_; }

    int get(int i, int j) const {
        const int n = static_cast<int>(rank_.size());
        if (i == n || j == n) return 0;
        if (i == j) return n-i;
        auto [lo,hi] = std::minmax(rank_[i],rank_[j]);
        const int level = std::bit_width(static_cast<unsigned>(hi-lo))-1;
        return std::min(table_[level][lo],table_[level][hi-(1 << level)]);
    }

private:
    std::vector<int> rank_;
    std::vector<std::vector<int>> table_;
};
}

std::vector<std::tuple<int,int,int>> enumerate_runs(std::string_view s) {
    if (s.size() >= static_cast<std::size_t>(INT_MAX)) std::abort();
    const int n = static_cast<int>(s.size());
    if (n < 2) return {};
    std::vector<int> text(s.begin(),s.end());
    for (int& symbol : text) symbol = static_cast<unsigned char>(symbol);
    const run_enumerate_detail::Lce forward(text);
    std::reverse(text.begin(),text.end());
    const run_enumerate_detail::Lce backward(text);
    std::reverse(text.begin(),text.end());

    std::vector<std::tuple<int,int,int>> result;
    std::vector<int> stack;
    stack.reserve(n);
    auto collect = [&](const std::vector<int>& order) {
        stack.clear();
        for (int i = n-1; i >= 0; --i) {
            while (!stack.empty() && order[stack.back()] > order[i]) stack.pop_back();
            if (!stack.empty()) {
                const int j = stack.back(), period = j-i;
                const int left = backward.get(n-i,n-j);
                const int right = forward.get(i,j);
                if (left+right >= period) result.emplace_back(period,i-left,j+right);
            }
            stack.push_back(i);
        }
    };
    collect(forward.ranks());
    for (int& symbol : text) symbol = 255-symbol;
    const auto suffix = atcoder::suffix_array(text,255);
    for (int i = 0; i < n; ++i) text[suffix[i]] = i;
    collect(text);
    std::sort(result.begin(),result.end());
    result.erase(std::unique(result.begin(),result.end()),result.end());
    return result;
}

/*
細かい注意点
区間は0-indexedの[l,r)。最小周期tを持ち、長さが2t以上で、周期tを保ったまま延長できない区間を返す。
バイト列として扱い、空文字列にも対応。長さはINT_MAX未満。
codecfではatcoder/stringを追加で読む。

使える関数一覧
enumerate_runs(s); // vector<tuple<int,int,int>>: (最小周期,左端,右端) の辞書順 O(N log(N+1)) 時間・領域

基本の関数
auto result=enumerate_runs("ababab");
if(result!=std::vector<std::tuple<int,int,int>>{{2,0,6}})std::abort();
*/
