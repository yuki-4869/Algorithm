namespace LazyXXX_impl{
    struct S {
    };
    struct F {
    };

    S op(S a, S b){
      S ret;

      return ret;
    }
    S e(){ return {}; }
    S mp(F f, S x){
      S ret;

      return ret;
    }
    F cp(F f, F g){
      F ret;

      return ret;
    }
    F id(){ return {}; }
}

struct LazyXXX:LazySegBase<
    LazyXXX_impl::S,
    LazyXXX_impl::op,
    LazyXXX_impl::e,
    LazyXXX_impl::F,
    LazyXXX_impl::mp,
    LazyXXX_impl::cp,
    LazyXXX_impl::id
>{
    using S=LazyXXX_impl::S;
    using F=LazyXXX_impl::F;
    using B=LazySegBase<S,LazyXXX_impl::op,LazyXXX_impl::e,F,LazyXXX_impl::mp,LazyXXX_impl::cp,LazyXXX_impl::id>;
    using B::B;
};

/*
細かい注意点
S/F/op/e/mp/cp/idを書き換える遅延セグ木の雛形。名前空間とクラス名も適宜変更する。
LazySegBaseが必要なので、lazy_segtree_wrappersを先に貼る。
opは結合則、eは単位元。mpは写像、cp(f,g)はgの後にf、idは恒等写像。
0-indexed、区間は[l,r)。以下は各演算O(1)の場合。
codecfではatcoder/lazysegtreeを追加で読み込む。

使える関数一覧
LazyXXX_impl::op(a,b);    // S: 集約演算
LazyXXX_impl::e();       // S: 単位元
LazyXXX_impl::mp(f,x);   // S: xへ写像fを適用
LazyXXX_impl::cp(f,g);   // F: gの後にfを合成
LazyXXX_impl::id();      // F: 恒等写像
LazyXXX seg(v);         // 構築 O(N)
seg.set(i,s);           // void: 点代入 O(log N)
seg.get(i);             // S: 点取得 O(log N)
seg.apply(i,f);         // void: 点へ写像 O(log N)
seg.apply(l,r,f);       // void: 区間へ写像 O(log N)
seg.prod(l,r);          // S: 区間集約 O(log N)
seg.all_prod();         // S: 全体集約 O(1)
seg.size();             // int: 配列長 O(1)
seg.empty();            // bool: 空配列か O(1)
seg.max_right(l,g);     // int: 単調述語を満たす最大右端 O(log N)
seg.min_left(r,g);      // int: 単調述語を満たす最小左端 O(log N)

基本の関数
vector<LazyXXX::S> values(3);                 // 各葉のSを設定する
LazyXXX seg(values);                         // 構築 O(N)
seg.apply(0,3,LazyXXX::F{});                  // 区間へ写像 O(log N)
auto result = seg.prod(0,3);                  // 集約 O(log N)
*/
