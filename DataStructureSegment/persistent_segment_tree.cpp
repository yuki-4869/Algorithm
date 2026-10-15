//永続セグメント木

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

  // コンストラクタ
  PersistentSegmentTree(const ll &N_, const F &f_, const T &ID_)
      : pid(0), N(N_), f(f_), ID(ID_) {
    pool = new Node[NODES];
    nil = my_new(ID);
    nil->l = nil->r = nil;
  }

  // 【機能4】デストラクタ（メモリの解放）
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
  // build 用の内部再帰関数
  Node *build_(const vector<T> &v, ll l, ll r) {
    if (l + 1 == r) return my_new(v[l]);
    ll m = (l + r) >> 1;
    return merge(build_(v, l, m), build_(v, m, r));
  }

  // update_apply 用の内部再帰関数
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

  // max_right 用の内部再帰関数
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

  // 【機能1】O(N) で初期配列から木を構築
  Node *build(const vector<T> &v) {
    return build_(v, 0, v.size());
  }

  // 通常の一点更新 (上書き)
  Node *update(Node *n, ll k, const T &x) {
    auto g = [&](const T &old_val) { return x; };
    return update_apply_(k, g, n, 0, N);
  }

  // 【機能2】関数による一点更新 (加算などに使用)
  template <typename G>
  Node *update_apply(Node *n, ll k, const G &g) {
    return update_apply_(k, g, n, 0, N);
  }

  // 区間取得
  T query(Node *n, ll a, ll b) { 
    return query_(a, b, n, 0, N); 
  }

  // 【機能3】セグメント木上の二分探索
  // l を左端として、check(query(l, r)) が true となる最大の r を返す
  template <typename C>
  ll max_right(Node *n, ll l, const C &check) {
    if (l == N) return N;
    T sm = ID;
    return max_right_(n, 0, N, l, sm, check);
  }
};
/*使い方
auto f = [](ll a, ll b){ return ここに演算を書く ;}
ll ID =  //ここに単位元を書く
PersistentSegmentTree<long long, decltype(f)> pst(m, f, ID);//mは数列１つあたりの項数
vector<decltype(pst)::Node*> roots(n+1, pst.new_tree());//nは数列の個数
roots[X] = roots[Y];// 配列 X を 配列 Y で上書き
roots[X] = pst.update(roots[X], i, j);//配列Xのi番目をjに変更
pst.query(roots[X], L, R);//配列Xの[L,R)の値を出力

auto add = [](ll x) { return x + 5; };
pst.update_apply(roots[X], Y, add);//配列XのY番目に関数を作用させる

auto check = [](ll val) { return val <= 60; };
 ll r = pst.max_right(roots[X], Y, check);//配列XのY番目から見て関数がtrueになる右端のindexを返す
*/