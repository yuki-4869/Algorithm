struct Mo {
    int n;

    vector<int> left;
    vector<int> right;
    vector<int> order;

    Mo() : n(0) {}

    explicit Mo(int n)
        : n(n) {}

    int add_query(int l, int r) {
        do { if (!(0 <= l && l <= r && r <= n)) std::abort(); } while (false);

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
細かい注意点
0-indexed、全区間は半開区間[l,r)。0<=l<=r<=N。
全クエリをadd_queryしてからrun。処理順は内部で並べ替え、output(id)のIDは登録順。
add/removeの引数は値ではなく配列添字。現在のデータ構造は空区間から始める。
左右別の処理順はadd_left(--L),add_right(R++),remove_left(L++),remove_right(--R)。
各追加削除O(1)なら移動量は典型的にO(N sqrt(Q))、別途クエリのソートO(Q log Q)。

使える関数一覧
Mo mo(N);                                       // 配列長を指定して初期化
mo.add_query(l,r);                               // int: クエリを登録しIDを返す
mo.build();                                     // void: 処理順を構築 O(Q log Q)
mo.run(add,remove,output);                       // void: 左右共通の処理で回答
mo.run(add_left,add_right,remove_left,remove_right,output); // void: 左右別の処理で回答

基本の関数
vl A={1,2,3}; Mo mo(3);
mo.add_query(0,2); mo.add_query(1,3);
ll sum=0; vl answers(2);
mo.run([&](int i){sum+=A[i];},
       [&](int i){sum-=A[i];},
       [&](int id){answers[id]=sum;});       // {3,5}
*/
