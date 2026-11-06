using FPSMint=FPS::value_type;
using FPSVector=vector<FPSMint>;

FPSMint lagrange_consecutive(const FPSVector& values,FPSMint x){
    ll count=values.size();
    if(!count)return 0;
    fps_detail::require(count<=FPSMint::mod());
    if(x.val()<count)return values[x.val()];
    const auto& table=fps_detail::factorials(count-1);
    FPSVector prefix(count+1,1),suffix(count+1,1);
    for(ll i=0;i<count;i++)prefix[i+1]=prefix[i]*(x-i);
    for(ll i=count;i--;)suffix[i]=suffix[i+1]*(x-i);
    FPSMint answer=0;
    for(ll i=0;i<count;i++){
        FPSMint term=values[i]*prefix[i]*suffix[i+1]*table.ifac[i]*table.ifac[count-1-i];
        if((count-1-i)&1)term=-term;
        answer+=term;
    }
    return answer;
}

namespace fps_combinatorial_detail {
inline FPSVector binomial(const FPSVector& a,bool inverse){
    ll count=a.size();
    if(!count)return {};
    const auto& table=fps_detail::factorials(count-1);
    FPS first(count),second(count);
    for(ll i=0;i<count;i++){
        first[i]=a[i]*table.ifac[i];second[i]=table.ifac[i];
        if(inverse&&(i&1))second[i]=-second[i];
    }
    FPS result=(first*second).pre(count);
    for(ll i=0;i<count;i++)result[i]*=table.fac[i];
    return result;
}
}

FPSVector binomial_transform(const FPSVector& a){return fps_combinatorial_detail::binomial(a,false);}
FPSVector inverse_binomial_transform(const FPSVector& a){return fps_combinatorial_detail::binomial(a,true);}

FPSVector stirling2_row(ll n){
    fps_detail::require(0<=n&&n<FPSMint::mod());
    const auto& table=fps_detail::factorials(n);
    FPS first(n+1),second(n+1);
    for(ll i=0;i<=n;i++){
        first[i]=FPSMint(i).pow(n)*table.ifac[i];second[i]=table.ifac[i];
        if(i&1)second[i]=-second[i];
    }
    return (first*second).pre(n+1);
}

FPSVector stirling1_signed(ll n){
    fps_detail::require(0<=n&&n<FPSMint::mod());
    auto build=[&](auto&& self,ll count)->FPS{
        if(count<=32){
            FPS result={1};
            for(ll i=0;i<count;i++){
                result.push_back(0);
                for(ll j=(ll)result.size()-1;j>0;j--)result[j]=result[j-1]-FPSMint(i)*result[j];
                result[0]*=-FPSMint(i);
            }
            return result;
        }
        ll half=count/2;
        FPS lower=self(self,half);
        FPS result=lower*lower.taylor_shift(-FPSMint(half));
        if(count&1){
            result.push_back(0);
            for(ll j=(ll)result.size()-1;j>0;j--)result[j]=result[j-1]-FPSMint(count-1)*result[j];
            result[0]*=-FPSMint(count-1);
        }
        return result;
    };
    return build(build,n);
}

FPSVector stirling1_unsigned(ll n){
    FPSVector result=stirling1_signed(n);
    for(ll i=0;i<(ll)result.size();i++)if((n-i)&1)result[i]=-result[i];
    return result;
}

FPSVector bernoulli_numbers(ll n){
    fps_detail::require(0<=n&&n<FPSMint::mod()-1);
    const auto& table=fps_detail::factorials(n+1);
    FPS denominator(n+1);
    for(ll i=0;i<=n;i++)denominator[i]=table.ifac[i+1];
    FPS result=denominator.inv(n+1);
    for(ll i=0;i<=n;i++)result[i]*=table.fac[i];
    return result;
}

FPSVector bell_numbers(ll n){
    fps_detail::require(0<=n&&n<FPSMint::mod());
    const auto& table=fps_detail::factorials(n);
    FPS exponent(n+1);
    for(ll i=1;i<=n;i++)exponent[i]=table.ifac[i];
    FPS result=exponent.exp(n+1);
    for(ll i=0;i<=n;i++)result[i]*=table.fac[i];
    return result;
}

FPSVector partition_numbers(ll n){
    fps_detail::require(0<=n&&n<FPSMint::mod());
    FPS denominator(n+1);
    denominator[0]=1;
    for(ll k=1;;k++){
        ll first=k*(3*k-1)/2,second=k*(3*k+1)/2;
        if(first>n)break;
        FPSMint sign=(k&1)?-1:1;
        denominator[first]=sign;
        if(second<=n)denominator[second]=sign;
    }
    return denominator.inv(n+1);
}

/*
細かい注意点
fps.cppまたはfps_mod1000000007.cppを先に貼る。返り値の法・型は選択したFPSに従う。
lagrange_consecutiveはy[i]=f(i)、deg f<y.size()。空配列は0、点数は法以下。
二項変換はb[n]=Σ C(n,k)a[k]。配列長は法以下。
Stirling数とBell数・整数分割数は0<=n<mod、Bernoulli数は0<=n<mod-1。
階乗・逆階乗・整数逆元はFPSとキャッシュを共有し、最大使用サイズ分のメモリを保持する。
第一種signedはX(X-1)...(X-n+1)の係数。unsignedは符号を除いた第一種Stirling数。
第一種は倍長化とTaylor shift、整数分割はEulerの五角数定理とFPS逆元を用いる。
Bernoulli数はX/(exp(X)-1)の定義、B_1=-1/2。Bell(0)=partition(0)=1。
各NTT長は選択したFPSの制限内であること。無効な引数はNDEBUG時も無言で停止。

使える関数一覧
FPSMint   lagrange_consecutive(y,x)      連続点0..N-1からの評価 / O(N)
FPSVector binomial_transform(a)         二項変換 / O(M(N))
FPSVector inverse_binomial_transform(a) 逆二項変換 / O(M(N))
FPSVector stirling2_row(n)              S(n,0..n) / O(M(n)+nlog(n+1))
FPSVector stirling1_signed(n)           符号付き第一種Stirling数 / O(M(n))
FPSVector stirling1_unsigned(n)         符号なし第一種Stirling数 / O(M(n))
FPSVector bernoulli_numbers(n)          B_0..B_n / O(M(n))
FPSVector bell_numbers(n)               Bell(0)..Bell(n) / O(M(n))
FPSVector partition_numbers(n)          p(0)..p(n) / O(M(n)+sqrt(n))
M(N)は選択したFPSの乗算計算量。キャッシュの追加前計算は最大使用サイズまで合計O(N)。

基本の関数
FPSVector y={1,4,9,16};
FPSMint evaluated=lagrange_consecutive(y,1000000000000LL);
FPSVector a={1,2,3,4};
auto transformed=binomial_transform(a);
auto restored=inverse_binomial_transform(transformed);
auto first=stirling1_signed(5);
auto second=stirling2_row(5);
auto bernoulli=bernoulli_numbers(10);
auto bell=bell_numbers(10);
auto partitions=partition_numbers(100);
*/
