using Mint=atcoder::modint1000000007;
using vMint=vector<Mint>;
namespace fps_detail {
using coefficient=Mint;
using coefficient_vector=vector<coefficient>;
inline void require(bool ok){if(!ok)std::abort();}
inline const coefficient_vector& inverses(ll n){
    require(0<=n&&n<coefficient::mod());
    static coefficient_vector table={0,1};
    for(ll i=table.size();i<=n;i++)
        table.push_back(coefficient(coefficient::mod()-(ll)(coefficient::mod()/i)*table[coefficient::mod()%i].val()%coefficient::mod()));
    return table;
}
struct Factorials{
    coefficient_vector fac={1},ifac={1};
    void ensure(ll n){
        const auto& inverse=inverses(n);
        for(ll i=fac.size();i<=n;i++){
            fac.push_back(fac.back()*i);
            ifac.push_back(ifac.back()*inverse[i]);
        }
    }
};
inline const Factorials& factorials(ll n){
    static Factorials table;
    table.ensure(n);
    return table;
}
inline coefficient_vector naive(const coefficient_vector& a,const coefficient_vector& b){
    coefficient_vector result(a.size()+b.size()-1);
    if(a.size()<b.size()){
        for(ll j=0;j<(ll)b.size();j++)if(b[j]!=coefficient(0))
            for(ll i=0;i<(ll)a.size();i++)result[i+j]+=a[i]*b[j];
    }else{
        for(ll i=0;i<(ll)a.size();i++)if(a[i]!=coefficient(0))
            for(ll j=0;j<(ll)b.size();j++)result[i+j]+=a[i]*b[j];
    }
    return result;
}
template<int Mod>
vector<atcoder::static_modint<Mod>> ntt_product(const coefficient_vector& a,const coefficient_vector& b){
    using M=atcoder::static_modint<Mod>;
    vector<M> first(a.size()),second(b.size());
    for(ll i=0;i<(ll)a.size();i++)first[i]=a[i].val();
    for(ll i=0;i<(ll)b.size();i++)second[i]=b[i].val();
    return atcoder::convolution(move(first),move(second));
}
inline coefficient_vector convolve(const coefficient_vector& a,const coefficient_vector& b){
    if(a.empty()||b.empty())return {};
    require(a.size()<=INT_MAX&&b.size()<=INT_MAX&&a.size()+b.size()-1<=INT_MAX);
    if(min(a.size(),b.size())<=60)return naive(a,b);
    ll count=a.size()+b.size()-1;
    require(count<=(1LL<<24));
    constexpr ll P1=167772161,P2=469762049,P3=754974721;
    const auto first=ntt_product<P1>(a,b);
    const auto second=ntt_product<P2>(a,b);
    const auto third=ntt_product<P3>(a,b);
    static const ll inverse12=atcoder::static_modint<P2>(P1).inv().val();
    static const ll inverse123=atcoder::static_modint<P3>(P1*P2%P3).inv().val();
    constexpr ll modulus=coefficient::mod(),product12=P1*P2%modulus;
    coefficient_vector result(count);
    for(ll i=0;i<count;i++){
        ll x1=first[i].val();
        ll x2=(second[i].val()-x1+P2)%P2*inverse12%P2;
        ll x12=x1+P1*x2;
        ll x3=(third[i].val()-x12%P3+P3)%P3*inverse123%P3;
        result[i]=(x1+(P1%modulus)*x2+product12*x3)%modulus;
    }
    return result;
}
}
vMint convolution_1e9p7(const vMint& a,const vMint& b){return fps_detail::convolve(a,b);}

struct FPS:vMint{
    using vMint::vMint;
    FPS(const vMint& a):vMint(a){}
    FPS(vMint&& a):vMint(move(a)){}
private:
    using C=Mint;
    static void precision(ll n){fps_detail::require(0<=n&&n<=C::mod());}
    static FPS multiply_prefix(const FPS& a,const FPS& b,ll n){
        if(!n||a.empty()||b.empty())return {};
        return (a.pre(n)*b.pre(n)).pre(n);
    }
public:
    FPS pre(ll n)const{
        fps_detail::require(n>=0);
        return FPS(begin(),begin()+min<ll>(n,size()));
    }
    FPS rev()const{
        FPS result=*this;
        reverse(result.begin(),result.end());
        return result;
    }
    FPS& shrink(){
        while(!empty()&&back()==C(0))pop_back();
        return *this;
    }
    FPS diff()const{
        if(size()<2)return {};
        FPS result(size()-1);
        for(ll i=1;i<(ll)size();i++)result[i-1]=(*this)[i]*i;
        return result;
    }
    FPS integral()const{
        fps_detail::require(size()<(size_t)C::mod());
        const auto& inverse=fps_detail::inverses(size());
        FPS result(size()+1);
        for(ll i=0;i<(ll)size();i++)result[i+1]=(*this)[i]*inverse[i+1];
        return result;
    }
    FPS& operator+=(const FPS& other){
        if(size()<other.size())resize(other.size());
        for(ll i=0;i<(ll)other.size();i++)(*this)[i]+=other[i];
        return *this;
    }
    FPS& operator-=(const FPS& other){
        if(size()<other.size())resize(other.size());
        for(ll i=0;i<(ll)other.size();i++)(*this)[i]-=other[i];
        return *this;
    }
    FPS& operator*=(const FPS& other){return *this=*this*other;}
    FPS& operator+=(C value){
        if(empty())resize(1);
        (*this)[0]+=value;
        return *this;
    }
    FPS& operator-=(C value){
        if(empty())resize(1);
        (*this)[0]-=value;
        return *this;
    }
    FPS& operator*=(C value){
        for(auto& x:*this)x*=value;
        return *this;
    }
    FPS& operator/=(C value){
        fps_detail::require(value!=C(0));
        return *this*=value.inv();
    }
    FPS operator-()const{
        FPS result=*this;
        for(auto& x:result)x=-x;
        return result;
    }
    friend FPS operator+(FPS a,const FPS& b){return a+=b;}
    friend FPS operator-(FPS a,const FPS& b){return a-=b;}
    friend FPS operator*(const FPS& a,const FPS& b){
        return FPS(fps_detail::convolve(a,b));
    }
    friend FPS operator+(FPS a,C b){return a+=b;}
    friend FPS operator+(C a,FPS b){return b+=a;}
    friend FPS operator-(FPS a,C b){return a-=b;}
    friend FPS operator-(C a,const FPS& b){return -b+a;}
    friend FPS operator*(FPS a,C b){return a*=b;}
    friend FPS operator*(C a,FPS b){return b*=a;}
    friend FPS operator/(FPS a,C b){return a/=b;}
    FPS operator<<(ll k)const{
        fps_detail::require(k>=0&&(size_t)k<=max_size()-size());
        FPS result((size_t)k+size());
        copy(begin(),end(),result.begin()+k);
        return result;
    }
    FPS operator>>(ll k)const{
        fps_detail::require(k>=0);
        if((size_t)k>=size())return {};
        return FPS(begin()+k,end());
    }
    FPS& operator<<=(ll k){return *this=*this<<k;}
    FPS& operator>>=(ll k){return *this=*this>>k;}
    FPS inv(ll n)const{
        precision(n);
        if(!n)return {};
        fps_detail::require(!empty()&&(*this)[0]!=C(0));
        FPS f=pre(n);
        f.shrink();
        C inverse=f[0].inv();
        if(f.size()==1){FPS result(n);result[0]=inverse;return result;}
        ll initial=min(n,32LL);
        FPS result(initial);
        result[0]=inverse;
        for(ll i=1;i<initial;i++){
            C sum=0;
            for(ll j=1;j<=i&&j<(ll)f.size();j++)sum+=f[j]*result[i-j];
            result[i]=-sum*inverse;
        }
        for(ll m=initial;m<n;m<<=1){
            ll k=min(n,m<<1);
            FPS error=multiply_prefix(f,result,k);
            error.resize(k);
            FPS tail(error.begin()+m,error.end());
            FPS correction=multiply_prefix(result,tail,k-m);
            correction.resize(k-m);
            result.resize(k);
            for(ll i=m;i<k;i++)result[i]=-correction[i-m];
        }
        return result;
    }
    FPS log(ll n)const{
        precision(n);
        if(!n)return {};
        fps_detail::require(!empty()&&(*this)[0]==C(1));
        FPS f=pre(n);
        f.shrink();
        if(n==1||f.size()==1)return FPS(n);
        FPS result=multiply_prefix(f.diff(),f.inv(n-1),n-1).integral();
        result.resize(n);
        return result;
    }
    FPS exp(ll n)const{
        precision(n);
        if(!n)return {};
        fps_detail::require(empty()||(*this)[0]==C(0));
        FPS f=pre(n);
        f.shrink();
        if(f.empty()){FPS result(n);result[0]=1;return result;}
        FPS result={1};
        for(ll m=1;m<n;m<<=1){
            ll k=min(n,m<<1);
            FPS correction=f.pre(k)-result.log(k);
            correction.resize(k);
            correction[0]+=1;
            result=multiply_prefix(result,correction,k);
            result.resize(k);
        }
        return result;
    }
    FPS pow(ll exponent,ll n)const{
        precision(n);
        fps_detail::require(exponent>=0);
        if(!n)return {};
        if(!exponent){FPS result(n);result[0]=1;return result;}
        FPS f=pre(n);
        f.shrink();
        if(f.empty())return FPS(n);
        if(exponent==1){f.resize(n);return f;}
        ll order=0;
        while(f[order]==C(0))order++;
        if(order&&exponent>(n-1)/order)return FPS(n);
        ll shift=order*exponent;
        C leading=f[order];
        FPS unit=FPS(f.begin()+order,f.end()).pre(n-shift)/leading;
        FPS result=(unit.log(n-shift)*C(exponent)).exp(n-shift)*leading.pow(exponent);
        result<<=shift;
        result.resize(n);
        return result;
    }
    static pair<bool,C> mod_sqrt(C value){
        const ll modulus=C::mod();
        if(value==C(0))return {true,0};
        if(value.pow((modulus-1)/2)!=C(1))return {false,0};
        if(modulus%4==3){
            C root=value.pow((modulus+1)/4);
            if(root.val()>(-root).val())root=-root;
            return {true,root};
        }
        ll odd=modulus-1,power=0;
        while(!(odd&1)){odd>>=1;power++;}
        static const C initial=[odd,modulus]{
            C nonresidue=2;
            while(nonresidue.pow((modulus-1)/2)==C(1))nonresidue++;
            return nonresidue.pow(odd);
        }();
        C base=initial,root=value.pow((odd+1)/2),rest=value.pow(odd);
        ll rank=power;
        while(rest!=C(1)){
            ll index=1;
            C squared=rest*rest;
            while(index<rank&&squared!=C(1)){squared*=squared;index++;}
            fps_detail::require(index<rank);
            C correction=base.pow(1LL<<(rank-index-1));
            root*=correction;
            base=correction*correction;
            rest*=base;
            rank=index;
        }
        if(root.val()>(-root).val())root=-root;
        return {true,root};
    }
    FPS sqrt(ll n)const{
        precision(n);
        if(!n)return {};
        FPS f=pre(n);
        f.shrink();
        if(f.empty())return FPS(n);
        ll order=0;
        while(f[order]==C(0))order++;
        if(order&1)return {};
        auto [ok,leading]=mod_sqrt(f[order]);
        if(!ok)return {};
        ll shift=order/2,terms=n-order;
        FPS unit=FPS(f.begin()+order,f.end())/f[order];
        FPS result={1};
        C inverse_two=C(2).inv();
        for(ll m=1;m<terms;m<<=1){
            ll k=min(terms,m<<1);
            result=(result+multiply_prefix(unit,result.inv(k),k))*inverse_two;
            result.resize(k);
        }
        result*=leading;
        result<<=shift;
        result.resize(n);
        return result;
    }
    pair<FPS,FPS> divmod(FPS divisor)const{
        FPS dividend=*this;
        dividend.shrink();divisor.shrink();
        fps_detail::require(!divisor.empty());
        if(dividend.size()<divisor.size())return {{},move(dividend)};
        if(divisor.size()==1){dividend/=divisor[0];return {move(dividend),{}};}
        ll count=dividend.size()-divisor.size()+1;
        FPS quotient;
        if(min<ll>(count,divisor.size())<=32){
            quotient.resize(count);
            C inverse=divisor.back().inv();
            for(ll i=count;i--;){
                C value=dividend[i+divisor.size()-1]*inverse;
                quotient[i]=value;
                if(value!=C(0))for(ll j=0;j<(ll)divisor.size();j++)dividend[i+j]-=value*divisor[j];
            }
            dividend.resize(divisor.size()-1);
        }else{
            quotient=multiply_prefix(dividend.rev().pre(count),divisor.rev().inv(count),count);
            quotient.resize(count);
            reverse(quotient.begin(),quotient.end());
            dividend=(dividend-quotient*divisor).pre(divisor.size()-1);
        }
        quotient.shrink();dividend.shrink();
        return {move(quotient),move(dividend)};
    }
    FPS& operator/=(const FPS& divisor){return *this=divmod(divisor).first;}
    FPS& operator%=(const FPS& divisor){return *this=divmod(divisor).second;}
    friend FPS operator/(FPS a,const FPS& b){return a/=b;}
    friend FPS operator%(FPS a,const FPS& b){return a%=b;}
    FPS taylor_shift(C value)const{
        if(empty()||value==C(0))return *this;
        ll length=size();
        FPS f=*this;
        f.shrink();
        if(f.size()<=1)return *this;
        ll count=f.size();
        const auto& table=fps_detail::factorials(count-1);
        FPS a(count),b(count);
        C power=1;
        for(ll i=0;i<count;i++){
            a[count-1-i]=f[i]*table.fac[i];
            b[i]=power*table.ifac[i];power*=value;
        }
        FPS product=a*b,result(length);
        for(ll i=0;i<count;i++)result[i]=product[count-1-i]*table.ifac[i];
        return result;
    }
    C eval(C value)const{
        C result=0;
        for(ll i=size();i--;)result=result*value+(*this)[i];
        return result;
    }
};

/*
細かい注意点
法1000000007専用。Mint=atcoder::modint1000000007、vMint=vector<Mint>。
基本テンプレートのll、vector等を使用する。code/codetではACLが利用可能。
codecfでは <atcoder/convolution> のincludeを有効にする。ライブラリ本体にはincludeを置かない。
998244353版と1000000007版はともにFPSを定義するので、どちらか一方を選ぶ。
係数は昇順で、添字iはX^iの係数。空配列も零多項式を表す。
pre(n)は先頭min(n,size())項の切り出しで、0埋めしない。nとシフト量は非負。
inv/log/exp/pow/sqrtのnは返す項数で、mod X^nを計算する。0<=n<=mod。
n=0なら空を返す。n>0ではinvはf[0]!=0、logはf[0]=1、expはf[0]=0（空も可）。
powの指数kは非負ll。sqrtは解がなければ空、零級数ならn項の零列を返す。
sqrtは一つの解を返し、先頭非零係数はmod_sqrtで得る小さい方の平方根を使う。
divmodと多項式に対する /, % は多項式除算。除数は非零。商・余りの末尾0は除去する。
integralは入力項数<mod。Taylor shiftは実際の次数<mod。整数の逆元が必要な範囲に注意。
長い乗算は3素数NTTとCRTを使用する。整数係数の復元は下記のNTT長制限内で安全。
NTTを使う各中間畳み込みの必要長（両入力項数の和-1）は2^24以下。
この上限は返り値の項数の上限ではない。次数打切り前の中間積にも適用する。
短い方の入力が60項以下の乗算は直接計算するので、NTT長の制限を受けない。
整数逆元・階乗・逆階乗は共有キャッシュ。最大使用サイズまで合計O(N)で拡張し、O(N)メモリを保持。
無効な引数はNDEBUG時も標準出力・標準エラーを出さず停止する。
vectorを継承し、size/resize、[]、begin/end、push_back、clear等は標準の意味で使える。

使える関数一覧
返り値         呼び出し方                  内容 / 計算量
構築           FPS(), FPS(n), FPS(n,c)     空・n項の0・n項のc / O(1), O(n), O(n)
構築           FPS{a,b,...}, FPS(l,r)      初期化リスト・反復子範囲 / O(項数)
構築           FPS(v), FPS(move(v))        vMintからコピー・移動 / O(項数), O(1)
FPS            f.pre(n)                   先頭最大n項 / O(min(n,N))
FPS            f.rev()                    係数反転 / O(N)
FPS&           f.shrink()                 末尾0を除去 / O(N)
FPS            f.diff()                   微分 / O(N)
FPS            f.integral()               積分定数0の積分 / O(N)
FPS            f.inv(n)                   逆数 mod X^n / O(M(n))
FPS            f.log(n)                   対数 mod X^n / O(M(n))
FPS            f.exp(n)                   指数 mod X^n / O(M(n))
FPS            f.pow(k,n)                 k乗 mod X^n / O(M(n)+log k)
FPS            f.sqrt(n)                  平方根 mod X^n / O(M(n)+log mod+v2(mod-1)^2)
pair<bool,Mint> FPS::mod_sqrt(a)        定数の平方根の有無と値 / O(log mod+v2(mod-1)^2)
pair<FPS,FPS>  f.divmod(g)                多項式の商・余り / O(M(A+B))
FPS            f.taylor_shift(c)          f(X+c)、項数を保持 / O(M(N))
Mint           f.eval(x)                  Horner法でf(x) / O(N)
FPS            f+g, f-g, -f               加減算・符号反転 / O(max(A,B)), O(N)
FPS            f*g                        畳み込み / O(M(A+B))
FPS            f/g, f%g                   多項式の商・余り / O(M(A+B))
FPS            f+c, c+f, f-c, c-f         定数項との加減算 / O(N)
FPS            f*c, c*f, f/c              スカラー乗除算 / O(N)
FPS            f<<k, f>>k                 X^k倍・低次k項削除 / O(N+k), O(N)
FPS&           f+=g, f-=g                 加減算して自己更新 / O(max(A,B))
FPS&           f*=g, f/=g, f%=g           積・多項式の商・余りで自己更新 / O(M(A+B))
FPS&           f+=c, f-=c                 定数項を自己更新 / O(1)
FPS&           f*=c, f/=c                 スカラーで自己更新 / O(N)
FPS&           f<<=k, f>>=k               シフトして自己更新 / O(N+k), O(N)
vMint          convolution_1e9p7(a,b)    畳み込み / O(M(A+B))
N=f.size()、AとBは二つの多項式の項数。M(n)=O(n log n)は乗算の上界。
法を固定した体演算での計算量。スカラーの逆元計算にはO(log mod)が加わる。
v2(t)はtを割る2の指数。空入力・定数・小さい積や除算は直接処理する。

基本の関数
FPS f={1,2,3},g={1,1};
FPS product=f*g;
FPS derivative=f.diff(),integrated=derivative.integral();
FPS inverse=f.inv(10);
FPS logarithm=f.log(10);
FPS exponential=FPS{0,1,2}.exp(10);
FPS power=f.pow(1000000000000LL,10);
FPS root=FPS{1,2,1}.sqrt(10);
auto [quotient,remainder]=f.divmod(g);
FPS shifted=f.taylor_shift(5);
Mint value=f.eval(3);
*/
