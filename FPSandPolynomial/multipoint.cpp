using FPSMint=FPS::value_type;
using FPSVector=vector<FPSMint>;

struct FPSPointSet{
private:
    ll n,sz;
    FPSVector x;
    vector<FPS> p;
    mutable bool weights_ready=false;
    mutable FPSVector weights;

    void prepare_weights()const{
        if(weights_ready)return;
        FPSVector derivative=eval(p[1].diff()),prefix(n+1,1);
        for(ll i=0;i<n;i++){
            fps_detail::require(derivative[i]!=FPSMint(0));
            prefix[i+1]=prefix[i]*derivative[i];
        }
        weights.resize(n);
        FPSMint inverse=prefix[n].inv();
        for(ll i=n;i--;){weights[i]=inverse*prefix[i];inverse*=derivative[i];}
        weights_ready=true;
    }
public:
    explicit FPSPointSet(const FPSVector& points):n(points.size()),sz(1),x(points){
        fps_detail::require(n<=(ll)INT_MAX/2);
        while(sz<n)sz<<=1;
        p.assign(sz<<1,FPS{1});
        for(ll i=0;i<n;i++)p[sz+i]={-x[i],1};
        for(ll i=sz-1;i;i--)p[i]=p[i<<1]*p[i<<1|1];
    }
    FPS product()const{return p[1];}
    FPSVector eval(const FPS& f)const{
        FPSVector answer(n);
        if(!n)return answer;
        auto visit=[&](auto&& self,ll node,ll left,ll right,const FPS& polynomial)->void{
            if(left>=n||polynomial.empty())return;
            ll last=min(right,n);
            if(last-left<=32||polynomial.size()<=32){
                for(ll i=left;i<last;i++)answer[i]=polynomial.eval(x[i]);
                return;
            }
            ll middle=left+(right-left)/2;
            self(self,node<<1,left,middle,polynomial%p[node<<1]);
            self(self,node<<1|1,middle,right,polynomial%p[node<<1|1]);
        };
        if(f.size()<p[1].size())visit(visit,1,0,sz,f);
        else visit(visit,1,0,sz,f%p[1]);
        return answer;
    }
    FPS interpolate(const FPSVector& values)const{
        fps_detail::require((ll)values.size()==n);
        if(!n)return {};
        prepare_weights();
        vector<FPS> partial(sz<<1);
        for(ll i=0;i<n;i++)if(values[i]!=FPSMint(0))partial[sz+i]={values[i]*weights[i]};
        for(ll i=sz-1;i;i--)
            partial[i]=partial[i<<1]*p[i<<1|1]+partial[i<<1|1]*p[i<<1];
        partial[1].shrink();
        return move(partial[1]);
    }
};

FPSVector multipoint_eval(const FPS& f,const FPSVector& x){return FPSPointSet(x).eval(f);}
FPS interpolate(const FPSVector& x,const FPSVector& y){return FPSPointSet(x).interpolate(y);}

/*
細かい注意点
fps.cppまたはfps_mod1000000007.cppを先に貼る。FPSMint/FPSVectorは選択したFPSの型。
多点評価は重複点にも対応。補間は法上で全評価点が相異なり、x.size()==y.size()であること。
空点集合のproductは{1}、evalとinterpolateは空。補間結果の末尾0は除去する。
評価点は構築時に固定。積木は内部情報として保持し、外部から変更できない。
同じ点集合での補間は微分値の逆元を初回にまとめて求めてキャッシュする。
少数の点・低次数の部分問題はHorner法へ切り替える。各NTT長はFPSの制限内。
無効な引数はNDEBUG時も標準エラーを出さず停止する。

使える関数一覧
構造体    FPSPointSet T(x)     固定評価点の積木を構築 / O(M(N)log N)、メモリO(N log N)
FPS       T.product()          Π(X-x[i])のコピー / O(N)
FPSVector T.eval(f)            点ごとのf(x[i]) / O(M(D+N)log(D+N))
FPS       T.interpolate(y)     次数<Nの補間 / O(M(N)log N)、初回は逆元表も構築
FPSVector multipoint_eval(f,x) 一度きりの多点評価 / 構築+eval
FPS       interpolate(x,y)     一度きりの補間 / 構築+interpolate
Nは評価点数、Dは入力多項式の項数。Mは選択したFPSの乗算計算量。

基本の関数
FPSVector x={1,2,3,4};
FPS f={5,3,2};
FPSPointSet tree(x);
auto y=tree.eval(f);
FPS restored=tree.interpolate(y);
FPS roots=tree.product();
auto y2=multipoint_eval(f,x);
FPS restored2=interpolate(x,y);
*/
