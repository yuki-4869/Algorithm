namespace SegXXX {
    struct S{
    };

    S op(S a,S b){
    S ret;

        return ret;
    }

    S e(){
        return {
        };
    }

    using Seg=segtree<S,op,e>;
}

/*
細かい注意点
S/op/eを書き換える汎用セグ木の雛形。名前空間SegXXXも用途に合わせて変更する。
opは結合則、eは単位元。以下の計算量は演算をO(1)とした場合。
0-indexed、区間は[l,r)。codecfではatcoder/segtreeを追加で読み込む。

使える関数一覧
SegXXX::op(a,b);           // S: 2つの集約情報を結合する利用者定義演算
SegXXX::e();              // S: 単位元
SegXXX::Seg seg(v);       // 構築 O(N)
seg.set(i,s);             // void: 点代入 O(log N)
seg.get(i);               // S: 点取得 O(1)
seg.prod(l,r);            // S: 区間集約 O(log N)
seg.all_prod();           // S: 全体集約 O(1)
seg.max_right(l,g);       // int: 単調述語を満たす最大右端 O(log N)
seg.min_left(r,g);        // int: 単調述語を満たす最小左端 O(log N)

基本の関数
vector<SegXXX::S> values(3);                  // 各葉のSを設定する
SegXXX::Seg seg(values);                     // 構築 O(N)
seg.set(1,SegXXX::S{});                       // 点代入 O(log N)
auto result = seg.prod(0,3);                  // 集約 O(log N)
*/
