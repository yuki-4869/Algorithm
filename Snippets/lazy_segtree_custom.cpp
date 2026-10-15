
//遅延セグ木（汎用テンプレ）
namespace LazyXXX_impl{
    struct S {
      
    };//モノイドの型
    struct F {
      
    };//写像の型

    S op(S a, S b){
      S ret;
      
      return ret;
    }//モノイドの計算する関数
    S e(){ return {}; }
    S mp(F f, S x){
      S ret;
      
      return ret;
    }//モノイドに写像を用いて計算する関数
    F cp(F f, F g){
      F ret;
      
      return ret;
    }//写像同士の合成関数 （fの方が後から作用させる写像）
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

// vector<LazyXXX::S> v(n);
// LazyXXX seg(v);
