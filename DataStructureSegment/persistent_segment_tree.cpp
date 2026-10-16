template <typename T, typename F, int NODES = 10000000>
struct PersistentSegmentTree {
  struct Node {
    T data;
    Node *l, *r;
    Node() {}
    Node(const T &_data) : data(_data), l(nullptr), r(nullptr) {}
  };

  Node *pool;
  int pid;
  ll N;
  const F f;
  const T ID;
  Node *nil;

  PersistentSegmentTree(const ll &N_, const F &f_, const T &ID_)
      : pid(0), N(N_), f(f_), ID(ID_) {
    pool = new Node[NODES];
    nil = my_new(ID);
    nil->l = nil->r = nil;
  }

  ~PersistentSegmentTree() {
    delete[] pool;
  }

  Node *my_new(const T &data) {
    pool[pid].data = data;
    pool[pid].l = pool[pid].r = nil;
    return &(pool[pid++]);
  }

  Node *merge(Node *l, Node *r) {
    pool[pid].data = f(l->data, r->data);
    pool[pid].l = l;
    pool[pid].r = r;
    return &(pool[pid++]);
  }

private:

  Node *build_(const vector<T> &v, ll l, ll r) {
    if (l + 1 == r) return my_new(v[l]);
    ll m = (l + r) >> 1;
    return merge(build_(v, l, m), build_(v, m, r));
  }

  template <typename G>
  Node *update_apply_(ll a, const G &g, Node *n, ll l, ll r) {
    if (l + 1 == r) {
      T old_val = (n == nil) ? ID : n->data;
      return my_new(g(old_val));
    }
    ll m = (l + r) >> 1;
    if (a < m) return merge(update_apply_(a, g, n->l, l, m), n->r);
    return merge(n->l, update_apply_(a, g, n->r, m, r));
  }

  T query_(ll a, ll b, Node *n, ll l, ll r) {
    if (n == nil) return ID;
    if (r <= a || b <= l) return ID;
    if (a <= l && r <= b) return n->data;
    ll m = (l + r) >> 1;
    return f(query_(a, b, n->l, l, m), query_(a, b, n->r, m, r));
  }

  template <typename C>
  ll max_right_(Node *n, ll l, ll r, ll ql, T &sm, const C &check) {
    if (r <= ql) return N;
    if (ql <= l) {
      T nxt = f(sm, n == nil ? ID : n->data);
      if (check(nxt)) {
        sm = nxt;
        return r;
      }
      if (l + 1 == r) return l;
    }
    ll m = (l + r) >> 1;
    ll res = max_right_(n->l, l, m, ql, sm, check);
    if (res < m) return res;
    return max_right_(n->r, m, r, ql, sm, check);
  }

public:
  Node *new_tree() { return nil; }

  Node *build(const vector<T> &v) {
    return build_(v, 0, v.size());
  }

  Node *update(Node *n, ll k, const T &x) {
    auto g = [&](const T &old_val) { return x; };
    return update_apply_(k, g, n, 0, N);
  }

  template <typename G>
  Node *update_apply(Node *n, ll k, const G &g) {
    return update_apply_(k, g, n, 0, N);
  }

  T query(Node *n, ll a, ll b) {
    return query_(a, b, n, 0, N);
  }

  template <typename C>
  ll max_right(Node *n, ll l, const C &check) {
    if (l == N) return N;
    T sm = ID;
    return max_right_(n, 0, N, l, sm, check);
  }
};

/*
細かい注意点
0-indexed、区間は[l,r)。配列長N>=1。fは結合則、IDは単位元。
build(v)はv.size()==Nで使用。new_treeは全要素IDの根をO(1)で作る。
updateは新しい根を返し、古い根を変更しない。根の代入は状態の共有O(1)。
NODESは全バージョンで確保するノード数の上限。余裕を持って指定する。
既定NODES=10000000。構築時の領域確保・初期化はO(NODES)、buildはO(N)。
max_rightの述語は単調で単位元を受理すること。木本体の浅いコピーはしない。

使える関数一覧
pst.new_tree();                      // Node*: 全要素IDの根 O(1)
pst.build(v);                        // Node*: 初期列の根 O(N)
pst.update(root,i,x);                // Node*: 1点代入後の新しい根 O(log N)
pst.update_apply(root,i,g);          // Node*: 1点にgを適用した新しい根 O(log N)
pst.query(root,l,r);                 // T: 区間の集約値 O(log N)
pst.max_right(root,l,check);         // ll: checkがtrueとなる最大の右端 O(log N)

基本の関数
auto op = [](ll a,ll b){return a+b;};
PersistentSegmentTree<ll,decltype(op),1024> pst(3,op,0LL);
auto root0 = pst.build(vl{1,2,3});
auto root1 = pst.update(root0,1,10);           // 新バージョン
auto old_sum = pst.query(root0,0,3);          // 6
auto new_sum = pst.query(root1,0,3);           // 14
*/
