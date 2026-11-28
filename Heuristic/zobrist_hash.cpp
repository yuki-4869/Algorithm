namespace ahc {

class ZobristHash {
public:
    explicit ZobristHash(std::uint64_t seed = 0) : seed_(seed) {}

    std::uint64_t key(std::uint32_t index, std::uint32_t value) const {
        std::uint64_t bits = ((std::uint64_t(index) << 32) | value) + seed_;
        bits += 0x9e3779b97f4a7c15ULL;
        bits = (bits ^ (bits >> 30)) * 0xbf58476d1ce4e5b9ULL;
        bits = (bits ^ (bits >> 27)) * 0x94d049bb133111ebULL;
        return bits ^ (bits >> 31);
    }

    std::uint64_t replace(std::uint64_t hash, std::uint32_t index,
                          std::uint32_t before, std::uint32_t after) const {
        return hash ^ key(index, before) ^ key(index, after);
    }

    std::uint64_t build(std::span<const std::uint32_t> values) const {
        if (values.size() > (std::uint64_t(1) << 32)) std::abort();
        std::uint64_t hash = 0;
        for (std::size_t index = 0; index < values.size(); ++index) {
            hash ^= key(std::uint32_t(index), values[index]);
        }
        return hash;
    }

private:
    std::uint64_t seed_;
};

}

/*
細かい注意点
- 参照URL https://github.com/yuki-4869/Algorithm/blob/main/Heuristic/zobrist_hash.cpp
- 位置・値は符号なし32 bit。列の位置は0-indexed。状態を比較するときは同じseed・同じ符号化を使う。
- XOR差分更新に対応。replaceに実際の更新前の値を渡す。
- 同じ状態のキーは一致する。異なる状態での衝突はあり得る。
- keyを直接XORする場合、同じ特徴を偶数個入れると相殺する。重複個数や残り手数が必要なら別の位置に保持する。

使える関数一覧
ahc::ZobristHash z(seed=0)                 seedを固定する                 O(1)
z.key(index,value)                        位置・値の特徴キーを作る       O(1)
z.replace(hash,index,before,after)         1点変更後のハッシュを返す       O(1)
z.build(values)                           列全体のハッシュを構築する      O(N)

基本の関数
ahc::ZobristHash z(2026);
vector<uint32_t> board{1,0,2,0};
auto hash = z.build(board);
hash = z.replace(hash,1,board[1],3);
board[1] = 3;
assert(hash == z.build(board));
*/
