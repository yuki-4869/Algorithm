namespace ahc {

template<class T>
class GridFenwick {
public:
    GridFenwick(int height, int width) : height_(height), width_(width) {
        if (height < 0 || width < 0) std::abort();
        if (height == 0 || width == 0) return;
        const std::size_t rows = std::size_t(height) + 1;
        const std::size_t cols = std::size_t(width) + 1;
        if (rows > tree_.max_size() / cols) std::abort();
        tree_.assign(rows * cols, T(0));
    }

    explicit GridFenwick(const std::vector<std::vector<T>>& initial)
        : GridFenwick(dimension(initial.size()), initial.empty() ? 0 : dimension(initial[0].size())) {
        for (const auto& row : initial) {
            if (row.size() != std::size_t(width_)) std::abort();
        }
        if (width_ == 0) return;
        for (std::size_t r = 1; r <= std::size_t(height_); ++r) {
            for (std::size_t c = 1; c <= std::size_t(width_); ++c) {
                slot(r,c) = initial[r - 1][c - 1];
            }
            for (std::size_t c = 1; c <= std::size_t(width_); ++c) {
                const std::size_t parent = c + (c & -c);
                if (parent <= std::size_t(width_)) slot(r,parent) += slot(r,c);
            }
        }
        for (std::size_t r = 1; r <= std::size_t(height_); ++r) {
            const std::size_t parent = r + (r & -r);
            if (parent <= std::size_t(height_)) {
                for (std::size_t c = 1; c <= std::size_t(width_); ++c) {
                    slot(parent,c) += slot(r,c);
                }
            }
        }
    }

    int height() const { return height_; }
    int width() const { return width_; }

    void add(int r, int c, T delta) {
        if (r < 0 || r >= height_ || c < 0 || c >= width_) std::abort();
        for (std::size_t i = std::size_t(r) + 1; i <= std::size_t(height_); i += i & -i) {
            for (std::size_t j = std::size_t(c) + 1; j <= std::size_t(width_); j += j & -j) {
                slot(i,j) += delta;
            }
        }
    }

    T prefix_sum(int r, int c) const {
        if (r < 0 || r > height_ || c < 0 || c > width_) std::abort();
        T sum(0);
        if (c == 0) return sum;
        for (std::size_t i = r; i; i &= i - 1) {
            for (std::size_t j = c; j; j &= j - 1) sum += tree_[index(i,j)];
        }
        return sum;
    }

    T sum(int top, int left, int bottom, int right) const {
        if (top > bottom || left > right) std::abort();
        return prefix_sum(bottom,right) - prefix_sum(top,right)
             - prefix_sum(bottom,left) + prefix_sum(top,left);
    }

private:
    static int dimension(std::size_t length) {
        if (length > INT_MAX) std::abort();
        return int(length);
    }

    std::size_t index(std::size_t r, std::size_t c) const {
        return r * (std::size_t(width_) + 1) + c;
    }

    T& slot(std::size_t r, std::size_t c) { return tree_[index(r,c)]; }

    int height_, width_;
    std::vector<T> tree_;
};

}

/*
細かい注意点
- 参照URL https://github.com/yuki-4869/Algorithm/blob/main/Heuristic/grid_fenwick.cpp
- 座標は0-indexedの (行,列)。長方形は [top,bottom) × [left,right)。
- Tは0と加減算を扱え、内部の集約値と途中の演算が収まる型を使う。
- 初期配列は長方形。空のvectorは0×0として扱う。

使える関数一覧
ahc::GridFenwick<T> fw(H,W)              全て0で構築                   O(1+HW)
ahc::GridFenwick fw(initial)             vector<vector<T>>から構築    O(1+H+HW)
fw.height(), fw.width()                 行数・列数                    O(1)
fw.add(r,c,delta)                       1点へ加算                     O(log(H+1)log(W+1))
fw.prefix_sum(r,c)                      [0,r)×[0,c) の和              O(1+log(H+1)log(W+1))
fw.sum(top,left,bottom,right)           半開長方形の和                O(1+log(H+1)log(W+1))

基本の関数
ahc::GridFenwick fw(vector<vector<long long>>{{1,2},{3,4}});
fw.add(1,0,5);
assert(fw.sum(1,0,2,2) == 12);
*/
