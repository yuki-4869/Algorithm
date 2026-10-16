template<class T> struct CartesianTree {
    int root;
    vector<int> par, left, right;

    CartesianTree() : root(0) {}
    CartesianTree(const vector<T>& v) :
    root(0), par(v.size(), -1), left(v.size(), -1), right(v.size(), -1) {
        vector<int> st(v.size(), 0);
        int top = 0;
        for (int i = 1; i < v.size(); ++i) {
            if (v[st[top]] > v[i]) {
                while (top >= 1 && v[st[top - 1]] > v[i]) --top;
                left[i] = st[top];
                par[left[i]] = i;
                if (top == 0) {
                    root = i;
                } else {
                    par[i] = st[top - 1];
                    right[par[i]] = i;
                }
                st[top] = i;
            } else {
                par[i] = st[top];
                right[par[i]] = i;
                st[++top] = i;
            }
        }
    }
};

/*
細かい注意点
0-indexed、非空配列。最小値が根のCartesian TreeをO(N)で構築する。
根の親と存在しない左右の子は-1。同値は先に現れた位置が上側になる。
最大値を根にする場合は、符号反転が表現可能な値域で配列を反転して渡す。

使える関数一覧
CartesianTree<T> ct(A); // 配列から構築 O(N)
ct.root;               // int: 根の添字 O(1)
ct.par[v];             // int: 親の添字 O(1)
ct.left[v];            // int: 左の子 O(1)
ct.right[v];           // int: 右の子 O(1)

基本の関数
vl A={3,1,2};
CartesianTree<ll> ct(A);
auto root = ct.root;                         // 1
auto parent = ct.par[0];                     // 1
*/
