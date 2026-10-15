//Mo's Algorithm
struct Mo {
    int n;

    vector<int> left;
    vector<int> right;
    vector<int> order;

    Mo() : n(0) {}

    explicit Mo(int n)
        : n(n) {}

    int add_query(int l, int r) {
        assert(0 <= l && l <= r && r <= n);

        int id = left.size();

        left.push_back(l);
        right.push_back(r);

        return id;
    }

    void build() {
        int q = left.size();

        order.resize(q);
        iota(order.begin(), order.end(), 0);

        if (q == 0) return;

        int block_size = max(
            1,
            (int)(n / max(1.0, sqrt((double)q)))
        );

        sort(
            order.begin(),
            order.end(),
            [&](int a, int b) {
                int block_a = left[a] / block_size;
                int block_b = left[b] / block_size;

                if (block_a != block_b) {
                    return block_a < block_b;
                }

                if (block_a & 1) {
                    return right[a] > right[b];
                } else {
                    return right[a] < right[b];
                }
            }
        );
    }

    template<
        class AddLeft,
        class AddRight,
        class RemoveLeft,
        class RemoveRight,
        class Output
    >
    void run(
        AddLeft add_left,
        AddRight add_right,
        RemoveLeft remove_left,
        RemoveRight remove_right,
        Output output
    ) {
        build();

        int l = 0;
        int r = 0;

        for (int id : order) {
            while (left[id] < l) {
                add_left(--l);
            }

            while (r < right[id]) {
                add_right(r++);
            }

            while (l < left[id]) {
                remove_left(l++);
            }

            while (right[id] < r) {
                remove_right(--r);
            }

            output(id);
        }
    }

    template<
        class Add,
        class Remove,
        class Output
    >
    void run(
        Add add,
        Remove remove,
        Output output
    ) {
        run(
            add,
            add,
            remove,
            remove,
            output
        );
    }

    int size() const {
        return left.size();
    }
};

/*
0-indexed
クエリ区間は半開区間 [l,r)

Mo mo(N);                                      // 配列長 N で初期化

int id = mo.add_query(l,r);                    // クエリ [l,r) を追加し、クエリ番号を返す O(1)

mo.run(
    add,
    remove,
    output
);                                              // 左右で処理が同じ場合

add(i);                                        // 要素 i を現在区間に追加
remove(i);                                     // 要素 i を現在区間から削除
output(id);                                    // クエリ id の答えを保存


左右で操作が異なる場合:

mo.run(
    add_left,
    add_right,
    remove_left,
    remove_right,
    output
);

add_left(i);                                   // 左側に i を追加
add_right(i);                                  // 右側に i を追加
remove_left(i);                                // 左側から i を削除
remove_right(i);                               // 右側から i を削除


現在区間は常に [L,R)

add_left(--L);
add_right(R++);
remove_left(L++);
remove_right(--R);

※ クエリを全て add_query() した後に run() する
※ クエリの順番は内部で並べ替えられる
※ output(id) の id は元のクエリ番号
※ add/remove が O(1) なら典型的には O((N+Q)sqrt(N)) 程度
*/