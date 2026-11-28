namespace ahc {

class SpatialBuckets2D {
public:
    using Point = std::pair<int,int>;

    SpatialBuckets2D(int universe, int bucket_size) : bucket_size_(bucket_size) {
        if (universe < 0 || bucket_size <= 0) std::abort();
        points_.resize(universe);
    }

    std::optional<Point> get(int id) const {
        const auto& entry = points_[id];
        if (!entry.active) return std::nullopt;
        return entry.point;
    }

    void set(int id, int x, int y) {
        auto& entry = points_[id];
        const auto dest = key(bucket(x),bucket(y));
        if (entry.active && key(bucket(entry.point.first),bucket(entry.point.second)) == dest) {
            entry.point = {x,y};
            return;
        }
        auto where = buckets_.try_emplace(dest, -1).first;
        erase(id);
        const int next = where->second;
        if (next >= 0) points_[next].previous = id;
        entry = {{x,y},-1,next,true};
        where->second = id;
    }

    bool erase(int id) {
        auto& entry = points_[id];
        if (!entry.active) return false;
        const int previous = entry.previous;
        const int next = entry.next;
        if (previous >= 0) {
            points_[previous].next = next;
        } else {
            const auto where = buckets_.find(key(bucket(entry.point.first),bucket(entry.point.second)));
            if (next < 0) buckets_.erase(where);
            else where->second = next;
        }
        if (next >= 0) points_[next].previous = previous;
        entry.active = false;
        return true;
    }

    std::vector<int> query_box(long long x0, long long y0, long long x1, long long y1) const {
        x0 = std::max(x0, (long long)INT_MIN);
        y0 = std::max(y0, (long long)INT_MIN);
        x1 = std::min(x1, 1LL + INT_MAX);
        y1 = std::min(y1, 1LL + INT_MAX);
        std::vector<int> result;
        if (x0 >= x1 || y0 >= y1 || buckets_.empty()) return result;
        const auto bx0 = bucket(x0), by0 = bucket(y0);
        const auto bx1 = bucket(x1 - 1), by1 = bucket(y1 - 1);
        auto collect = [&](int head) {
            for (int id = head; id >= 0; id = points_[id].next) {
                const auto [x,y] = points_[id].point;
                if (x0 <= x && x < x1 && y0 <= y && y < y1) result.push_back(id);
            }
        };
        const auto cols = std::uint64_t(bx1 - bx0 + 1);
        const auto rows = std::uint64_t(by1 - by0 + 1);
        if (rows > buckets_.size() / cols) {
            for (const auto& entry : buckets_) collect(entry.second);
        } else {
            for (long long bx = bx0; bx <= bx1; ++bx) {
                for (long long by = by0; by <= by1; ++by) {
                    const auto found = buckets_.find(key(bx,by));
                    if (found != buckets_.end()) collect(found->second);
                }
            }
        }
        return result;
    }

private:
    struct Entry {
        Point point{};
        int previous = -1, next = -1;
        bool active = false;
    };

    long long bucket(long long coordinate) const {
        return coordinate / bucket_size_ - (coordinate % bucket_size_ < 0);
    }

    static std::uint64_t key(long long x, long long y) {
        return (std::uint64_t(std::uint32_t(x)) << 32) | std::uint32_t(y);
    }

    int bucket_size_;
    std::vector<Entry> points_;
    std::unordered_map<std::uint64_t,int> buckets_;
};

}

/*
細かい注意点
- 参照URL https://github.com/yuki-4869/Algorithm/blob/main/Heuristic/spatial_buckets_2d.cpp
- IDは [0,universe)。座標はintで負値も可。異なるIDを同じ座標に置ける。
- bucket_sizeは正。query_boxは [x0,x1) × [y0,y1) で、結果順は任意。
- 小さい箱の反復照会向け。照会先のバケット数が登録バケット数より多ければ登録点を走査する。

使える関数一覧
ahc::SpatialBuckets2D points(universe,bucket_size)  空で構築             O(universe)
ahc::SpatialBuckets2D::Point                      pair<int,int>
points.get(id)                                    座標またはnullopt   O(1)
points.set(id,x,y)                                 追加・移動           償却期待O(1)
points.erase(id)                                   削除できればtrue     期待O(1)
points.query_box(x0,y0,x1,y1)                       箱内のID列           期待O(1+B+K)
Bは照会で調べるバケット数、Kは座標を照合する点数。領域はO(universe)。

基本の関数
ahc::SpatialBuckets2D points(5,8);
points.set(1,-3,4);
points.set(2,5,5);
assert(points.query_box(-5,0,0,10) == vector<int>{1});
points.erase(1);
assert(!points.get(1));
*/
