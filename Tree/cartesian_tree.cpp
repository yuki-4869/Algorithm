//CartesianTree(根が最小)
template<class T> struct CartesianTree {
    int root;  // root
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

  
  //rep(i,sz(v)) v[i] *= -1;//大きい数を根にしたいときはこれを挟む
  CartesianTree<ll> ct(v);
  
  /*使い方
  ct.par[x] xの親のindex(根の場合は-1)
  ct.left[x] xの左子のindex(存在しない場合は-1)
  ct.right[x] xの右子のindex(存在しない場合は-1)
  */