namespace ahc {

class GridBfs {
public:
    using Cell = std::pair<int, int>;

    GridBfs(int height, int width) : height_(height), width_(width) {
        if (height < 0 || width < 0 || 1LL * height * width > INT_MAX) std::abort();
        dist_.assign(height * width, -1);
        parent_dir_.resize(dist_.size());
    }

    template<class CanStep>
    void run(std::span<const Cell> starts, CanStep&& can_step) {
        seed(starts, [](int) { return true; });
        for (std::size_t head = 0; head < queue_.size(); ++head) {
            const int index = queue_[head];
            const Cell from{index / width_, index % width_};
            for (int direction = 0; direction < 4; ++direction) {
                const Cell to{from.first + dr_[direction], from.second + dc_[direction]};
                if (!inside(to)) continue;
                const int next = to.first * width_ + to.second;
                if (dist_[next] != -1 || !std::invoke(can_step, from, to)) continue;
                dist_[next] = dist_[index] + 1;
                parent_dir_[next] = std::uint8_t(direction);
                queue_.push_back(next);
            }
        }
    }

    void run_board(const std::vector<std::string>& board, std::span<const Cell> starts,
                   char wall = '#') {
        check_board(board);
        for (Cell start : starts) {
            if (!inside(start) || board[start.first][start.second] == wall) std::abort();
        }
        run(starts, [&](Cell, Cell to) { return board[to.first][to.second] != wall; });
    }

    void prepare_board(const std::vector<std::string>& board, char wall = '#') {
        check_board(board);
        prepared_moves_.assign(dist_.size(), 0);
        if (width_ != 0) {
            for (int r = 0; r < height_; ++r) {
                for (int c = 0; c < width_; ++c) {
                    if (board[r][c] == wall) continue;
                    unsigned mask = 16;
                    for (int d = 0; d < 4; ++d) {
                        const Cell next{r + dr_[d], c + dc_[d]};
                        if (inside(next) && board[next.first][next.second] != wall) mask |= 1u << d;
                    }
                    prepared_moves_[r * width_ + c] = std::uint8_t(mask);
                }
            }
        }
        prepared_ = true;
    }

    void run_prepared(std::span<const Cell> starts) {
        if (!prepared_) std::abort();
        seed(starts, [&](int index) { return prepared_moves_[index] != 0; });
        const int offset[4]{-width_, 1, width_, -1};
        for (std::size_t head = 0; head < queue_.size(); ++head) {
            const int index = queue_[head];
            const int next_distance = dist_[index] + 1;
            for (unsigned mask = prepared_moves_[index] & 15u; mask != 0; mask &= mask - 1) {
                const int direction = std::countr_zero(mask);
                const int next = index + offset[direction];
                if (dist_[next] != -1) continue;
                dist_[next] = next_distance;
                parent_dir_[next] = std::uint8_t(direction);
                queue_.push_back(next);
            }
        }
    }

    bool inside(Cell cell) const {
        return 0 <= cell.first && cell.first < height_
            && 0 <= cell.second && cell.second < width_;
    }

    int dist(Cell target) const { return dist_[id(target)]; }

    std::optional<std::string> path(Cell target) const {
        int index = id(target);
        if (dist_[index] < 0) return std::nullopt;
        std::string moves;
        moves.reserve(dist_[index]);
        while (dist_[index] > 0) {
            const int direction = parent_dir_[index];
            moves.push_back("URDL"[direction]);
            index -= dr_[direction] * width_ + dc_[direction];
        }
        std::reverse(moves.begin(), moves.end());
        return moves;
    }

private:
    template<class IsAllowed>
    void seed(std::span<const Cell> starts, IsAllowed allowed) {
        std::fill(dist_.begin(), dist_.end(), -1);
        queue_.clear();
        for (Cell start : starts) {
            const int index = id(start);
            if (!allowed(index)) std::abort();
            if (dist_[index] == 0) continue;
            dist_[index] = 0;
            queue_.push_back(index);
        }
    }

    void check_board(const std::vector<std::string>& board) const {
        if (board.size() != std::size_t(height_)) std::abort();
        for (const auto& row : board) {
            if (row.size() != std::size_t(width_)) std::abort();
        }
    }

    int id(Cell cell) const {
        if (!inside(cell)) std::abort();
        return cell.first * width_ + cell.second;
    }

    static constexpr int dr_[4]{-1, 0, 1, 0};
    static constexpr int dc_[4]{0, 1, 0, -1};
    int height_, width_;
    std::vector<int> dist_, queue_;
    std::vector<std::uint8_t> parent_dir_, prepared_moves_;
    bool prepared_ = false;
};

}

/*
細かい注意点
- 参照URL https://github.com/yuki-4869/Algorithm/blob/main/Heuristic/grid_bfs.cpp
- 0-indexedの (行,列)。縦横4近傍で各辺のコストは1。0 <= H*W <= INT_MAX。
- 複数始点は与えた順、各頂点からはURDL順で探索する。
- 始点と照会対象は盤内。run_boardの始点は通行可能マス。未探索・到達不能の距離は-1。
- can_step(from,to) は盤内の隣接マスへの通行可否。向きに依存する壁にも対応する。
- 同じ文字盤面で何度も探索する場合はprepare_boardで壁を固定し、run_preparedで始点だけ変える。

使える関数一覧
ahc::GridBfs bfs(H,W)                  盤面の大きさを設定する           O(HW)
ahc::GridBfs::Cell                     pair<int,int> の座標
bfs.run(starts,can_step)               通行判定を渡して多始点BFSする    O(HW+S)
bfs.run_board(board,starts,wall='#')   文字盤面上で多始点BFSする        O(H+HW+S)
bfs.prepare_board(board,wall='#')    壁を保存し隣接方向を前計算      O(H+HW)
bfs.run_prepared(starts)              保存した文字盤面で探索          O(HW+S)
bfs.inside(cell)                       盤内ならtrue                    O(1)
bfs.dist(target)                       最短距離または-1               O(1)
bfs.path(target)                       最短手順またはnullopt          O(1+最短距離)
Sは始点数。通行判定はO(1)とする。探索領域はO(HW)。

基本の関数
vector<string> board{"...",".#.","..."};
ahc::GridBfs bfs(3,3);
array<ahc::GridBfs::Cell,1> starts{{{0,0}}};
bfs.prepare_board(board);
bfs.run_prepared(starts);
int distance = bfs.dist({2,2});
auto moves = bfs.path({2,2});
*/
