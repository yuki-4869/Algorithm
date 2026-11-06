using FPSMint=FPS::value_type;
using FPSVector=vector<FPSMint>;

FPS polynomial_product(vector<FPS> factors){
    auto larger=[](const FPS& a,const FPS& b){return a.size()>b.size();};
    vector<FPS> heap;
    heap.reserve(factors.size());
    FPSMint scale=1;
    for(auto& factor:factors){
        factor.shrink();
        if(factor.empty())return {};
        if(factor.size()==1)scale*=factor[0];
        else heap.push_back(move(factor));
    }
    if(heap.empty())return {scale};
    make_heap(heap.begin(),heap.end(),larger);
    auto take=[&]{
        pop_heap(heap.begin(),heap.end(),larger);
        FPS result=move(heap.back());heap.pop_back();return result;
    };
    while(heap.size()>1){
        FPS first=take(),second=take();
        heap.push_back(first*second);
        push_heap(heap.begin(),heap.end(),larger);
    }
    heap[0]*=scale;
    return move(heap[0]);
}

FPS product_linear(const FPSVector& roots){
    auto build=[&](auto&& self,ll left,ll right)->FPS{
        if(right-left<=32){
            FPS result={1};
            for(ll i=left;i<right;i++){
                result.push_back(0);
                for(ll j=(ll)result.size()-1;j>0;j--)result[j]=result[j-1]-roots[i]*result[j];
                result[0]*=-roots[i];
            }
            return result;
        }
        ll middle=left+(right-left)/2;
        return self(self,left,middle)*self(self,middle,right);
    };
    return build(build,0,roots.size());
}

/*
細かい注意点
fps.cppまたはfps_mod1000000007.cppを先に貼る。FPSMint/FPSVectorは選択したFPSの型。
係数は昇順。零多項式を含む積は空。空リストの積は{1}。
polynomial_productは値渡しで元の配列を変更しない。moveで渡せば入力コピーを省略できる。
小さい多項式から掛け、ヒープの取り出しもmoveする。定数因子は先にまとめる。
product_linearは少数の一次因子を直接掛け、それ以上は平衡分割する。
各NTT長は選んだFPSの制限内であること。

使える関数一覧
FPS polynomial_product(fs)    多項式の総積 / O(Slog^2(S+1))
FPS product_linear(a)         Π(X-a[i]) / O(Nlog^2(N+1))
Sは入力の係数数の総和、Nは根の個数。

基本の関数
vector<FPS> factors={{1,1},{1,2},{1,3}};
FPS product=polynomial_product(factors);
FPSVector roots={1,2,3,4};
FPS monic=product_linear(roots);
*/
