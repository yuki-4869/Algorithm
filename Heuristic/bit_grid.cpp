namespace ahc {

class BitGrid {
public:
    explicit BitGrid(int height, int width) : width_(width) {
        if (height < 0 || width < 0 || width > 64) std::abort();
        mask_ = width == 64 ? UINT64_MAX : (std::uint64_t(1) << width) - 1;
        rows_.resize(height);
    }

    int height() const { return int(rows_.size()); }
    int width() const { return width_; }

    std::uint64_t row(int r) const { return rows_[r]; }
    void set_row(int r, std::uint64_t bits) { rows_[r] = bits & mask_; }

    bool test(int r, int c) const { return (row(r) >> c) & 1; }

    void set(int r, int c, bool value = true) {
        const std::uint64_t bit = std::uint64_t(1) << c;
        if (value) rows_[r] |= bit;
        else rows_[r] &= ~bit;
    }

    std::size_t count() const {
        std::size_t total = 0;
        for (auto bits : rows_) total += std::popcount(bits);
        return total;
    }

    BitGrid neighbors4() const {
        BitGrid result(height(), width_);
        for (int r = 0; r < height(); ++r) result.rows_[r] = adjacent(rows_, r);
        return result;
    }

    BitGrid flood(const BitGrid& allowed) const {
        if (height() != allowed.height() || width_ != allowed.width_) std::abort();
        BitGrid reached(height(), width_);
        if (width_ == 0) return reached;
        std::vector<int> pending;
        std::vector<char> in_queue(height());
        for (int r = 0; r < height(); ++r) {
            reached.rows_[r] = rows_[r] & allowed.rows_[r];
            if (reached.rows_[r]) {
                pending.push_back(r);
                in_queue[r] = true;
            }
        }
        auto extend = [&](int r, std::uint64_t incoming) {
            const auto fresh = incoming & allowed.rows_[r] & ~reached.rows_[r];
            if (fresh == 0) return;
            reached.rows_[r] |= fresh;
            if (!in_queue[r]) {
                in_queue[r] = true;
                pending.push_back(r);
            }
        };
        while (!pending.empty()) {
            const int r = pending.back();
            pending.pop_back();
            in_queue[r] = false;
            auto bits = reached.rows_[r];
            const auto open = allowed.rows_[r];
            const auto first = open & -open;
            if ((open & (open + first)) == 0) {
                bits = open;
            } else {
                auto left = open, right = open;
                for (int shift = 1; shift < width_ && bits != open; shift <<= 1) {
                    bits |= ((bits << shift) & right) | ((bits >> shift) & left);
                    right &= right << shift;
                    left &= left >> shift;
                }
            }
            reached.rows_[r] = bits;
            if (r > 0) extend(r - 1, bits);
            if (r + 1 < height()) extend(r + 1, bits);
        }
        return reached;
    }

private:
    std::uint64_t adjacent(const std::vector<std::uint64_t>& rows, int r) const {
        std::uint64_t bits = (rows[r] << 1) | (rows[r] >> 1);
        if (r > 0) bits |= rows[r - 1];
        if (r + 1 < height()) bits |= rows[r + 1];
        return bits & mask_;
    }

    int width_;
    std::uint64_t mask_;
    std::vector<std::uint64_t> rows_;
};

}

/*
細かい注意点
- 参照URL https://github.com/yuki-4869/Algorithm/blob/main/Heuristic/bit_grid.cpp
- 各行は64 bit。0 <= W <= 64。座標は0-indexedで列0が最下位ビット。
- 行・列の添字は盤内。set_rowは盤外のビットを切り捨てる。
- floodではallowedと盤面サイズを揃える。通行不能な始点は除く。
- floodは新しく到達した行だけ再処理し、連続する横区間はまとめて広げる。行が単一の連続区間なら一括で到達済みにする。

使える関数一覧
ahc::BitGrid grid(H,W)              ビットが全て0の盤面を作る              O(H)
grid.height(), grid.width()        高さ・幅を返す                          O(1)
grid.row(r), grid.set_row(r,bits)   行のビットを取得・設定する              O(1)
grid.test(r,c), grid.set(r,c,b=true)  1マスの取得・設定                    O(1)
grid.count()                       1のマス数を返す                        O(H)
grid.neighbors4()                  1のマスから1手で移動できる集合         O(H)
grid.flood(allowed)                 allowed内で到達できる全マスの集合      O(1+H+R log(W+1))
Rは処理された行の延べ数でR<=HW。領域はO(H)。

基本の関数
ahc::BitGrid start(2,4), allowed(2,4);
start.set(0,0);
allowed.set_row(0,0b1011);
allowed.set_row(1,0b1111);
auto reached = start.flood(allowed);
assert(reached.count() == 7);
*/
