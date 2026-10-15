
//汎用セグ木テンプレ

namespace SegXXX {
    struct S{
        
    };// 情報

    S op(S a,S b){
	S ret;


        return ret;
    }// merge

    S e(){
        return {
            
        };
    }// 単位元

    using Seg=segtree<S,op,e>;
}

//vector<SegXXX::S> v(n);
//SegXXX::Seg seg(v);
