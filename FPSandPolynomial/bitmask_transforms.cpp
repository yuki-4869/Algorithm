class BitmaskTransform{
    BitmaskTransform()=delete;
    template<class T> static void transform_(vector<T>&a,bool upper,bool inverse){
        const size_t n=a.size();
        if(n&&(n&(n-1)))std::abort();
        for(size_t bit=1;bit<n;bit<<=1){
            for(size_t start=0;start<n;start+=bit*2){
                for(size_t j=0;j<bit;++j){
                    T&to=a[start+j+(upper?0:bit)];
                    const T&from=a[start+j+(upper?bit:0)];
                    if(inverse)to-=from;
                    else to+=from;
                }
            }
        }
    }
    template<class T> static vector<T> convolution_(vector<T>a,vector<T>b,bool upper){
        if(a.size()!=b.size())std::abort();
        transform_(a,upper,false);
        transform_(b,upper,false);
        for(size_t i=0;i<a.size();++i)a[i]*=b[i];
        transform_(a,upper,true);
        return a;
    }
public:
    template<class T> static void subset_zeta(vector<T>&a){transform_(a,false,false);}
    template<class T> static void subset_mobius(vector<T>&a){transform_(a,false,true);}
    template<class T> static void superset_zeta(vector<T>&a){transform_(a,true,false);}
    template<class T> static void superset_mobius(vector<T>&a){transform_(a,true,true);}
    template<class T> static vector<T> or_convolution(vector<T>a,vector<T>b){return convolution_(std::move(a),std::move(b),false);}
    template<class T> static vector<T> and_convolution(vector<T>a,vector<T>b){return convolution_(std::move(a),std::move(b),true);}
};

/*
細かい注意点
添字は0-indexedのビット集合。N=配列長。非空ならNは2冪、空の変換は何もしない。
畳み込みは入力2列の長さが同じこと。両方空なら空を返す。自動の0埋めは行わない。
長さ不一致・非2冪長はNDEBUGでもメッセージなしで停止。
Tはコピー・+=・-=に対応する加法可換群。畳み込みにはさらに*=と分配法則が必要。
ll,LL,mintなどで使用可能。途中の和・差・積も型の範囲内であること。vector<bool>は非対応。
除算・逆元を使わないため、合成数modや偶数modのmodintでも使える。
変換は入力を書き換える。畳み込みは値渡しで元の入力を書き換えない。
OR畳み込みは集合の重複を許す。互いに素な部分集合への分割を数えるsubset convolutionとは異なる。
基本テンプレート以外のincludeは不要。クラスの構築は不要で、静的関数を呼び出す。

使える関数一覧
void BitmaskTransform::subset_zeta(a);      // a[S]=Σ_{T⊆S}元a[T] 時間O(1+N log(N+1))・追加領域O(1)
void BitmaskTransform::subset_mobius(a);    // 部分集合ゼータの逆変換 同上
void BitmaskTransform::superset_zeta(a);    // a[S]=Σ_{T⊇S}元a[T] 同上
void BitmaskTransform::superset_mobius(a);  // 上位集合ゼータの逆変換 同上
vector<T> BitmaskTransform::or_convolution(a,b);  // c[S]=Σ_{i|j=S}a[i]b[j] 時間O(1+N log(N+1))・領域O(N+1)
vector<T> BitmaskTransform::and_convolution(a,b); // c[S]=Σ_{i&j=S}a[i]b[j] 同上

基本の関数
vl a={1,2,3,4};
BitmaskTransform::subset_zeta(a);     // {1,3,4,10}
BitmaskTransform::subset_mobius(a);   // {1,2,3,4}
BitmaskTransform::superset_zeta(a);   // {10,6,7,4}
BitmaskTransform::superset_mobius(a); // {1,2,3,4}
auto c=BitmaskTransform::or_convolution(vl{1,2},vl{3,4});  // {3,18}
auto d=BitmaskTransform::and_convolution(vl{1,2},vl{3,4}); // {13,8}
*/
