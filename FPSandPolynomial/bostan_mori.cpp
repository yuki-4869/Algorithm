using FPSMint=FPS::value_type;
using FPSVector=vector<FPSMint>;

FPSMint bostan_mori(FPS numerator,FPS denominator,ll n){
    fps_detail::require(n>=0);
    if(n<(ll)numerator.size()-1)numerator.resize(n+1);
    if(n<(ll)denominator.size()-1)denominator.resize(n+1);
    numerator.shrink();denominator.shrink();
    fps_detail::require(!denominator.empty()&&denominator[0]!=FPSMint(0));
    if(!n||denominator.size()==1)
        return n<(ll)numerator.size()?numerator[n]/denominator[0]:FPSMint(0);
    FPSMint answer=0;
    if(numerator.size()>=denominator.size()){
        auto [quotient,remainder]=numerator.divmod(denominator);
        if(n<(ll)quotient.size())answer+=quotient[n];
        numerator=move(remainder);
    }
    while(n&&!numerator.empty()){
        FPS opposite=denominator;
        for(ll i=1;i<(ll)opposite.size();i+=2)opposite[i]=-opposite[i];
        FPS first=numerator*opposite,second=denominator*opposite;
        FPS next_numerator,next_denominator;
        next_numerator.reserve((first.size()+1)/2);
        next_denominator.reserve((second.size()+1)/2);
        for(ll i=n&1;i<(ll)first.size();i+=2)next_numerator.push_back(first[i]);
        for(ll i=0;i<(ll)second.size();i+=2)next_denominator.push_back(second[i]);
        numerator=move(next_numerator);denominator=move(next_denominator);
        numerator.shrink();denominator.shrink();n>>=1;
    }
    return answer+(numerator.empty()?FPSMint(0):numerator[0]/denominator[0]);
}

FPSMint linear_recurrence(const FPSVector& initial,const FPSVector& coefficients,ll n){
    fps_detail::require(n>=0&&initial.size()==coefficients.size());
    if(initial.empty())return 0;
    if(n<(ll)initial.size())return initial[n];
    ll count=initial.size();
    FPS denominator(count+1);
    denominator[0]=1;
    for(ll i=0;i<count;i++)denominator[i+1]=-coefficients[i];
    FPS numerator=(FPS(initial)*denominator).pre(count);
    return bostan_mori(move(numerator),move(denominator),n);
}

FPSVector berlekamp_massey(const FPSVector& sequence){
    FPSVector current={1},previous={1};
    ll order=0,gap=1;
    FPSMint last=1;
    for(ll n=0;n<(ll)sequence.size();n++){
        FPSMint discrepancy=sequence[n];
        for(ll i=1;i<=order;i++)discrepancy+=current[i]*sequence[n-i];
        if(discrepancy==FPSMint(0)){gap++;continue;}
        FPSVector saved=current;
        FPSMint scale=discrepancy/last;
        if(current.size()<previous.size()+gap)current.resize(previous.size()+gap);
        for(ll i=0;i<(ll)previous.size();i++)current[i+gap]-=scale*previous[i];
        if(2*order<=n){order=n+1-order;previous=move(saved);last=discrepancy;gap=1;}
        else gap++;
    }
    current.resize(order+1);
    FPSVector result(order);
    for(ll i=0;i<order;i++)result[i]=-current[i+1];
    return result;
}

/*
細かい注意点
fps.cppまたはfps_mod1000000007.cppを先に貼る。FPSMint/FPSVectorは選択したFPSの係数型・配列型。
bostan_moriはQ[0]!=0、n>=0。[x^n]P(x)/Q(x)を返す。必要な接頭辞だけ使用する。
linear_recurrenceのa,cは同じ長さk。a[t]=c[0]a[t-1]+...+c[k-1]a[t-k] (t>=k)。
aは第0..k-1項。a,cが両方空なら全0列として0を返す。
berlekamp_masseyは既知の有限列に一致する最短漸化式の係数を返す。未知の続きへの成立は保証しない。
空列・全0列の推定結果は空。各畳み込みのNTT長は選んだFPSの制限内であること。
無効な引数はNDEBUG時も標準エラーを出さず停止する。

使える関数一覧
FPSMint   bostan_mori(P,Q,n)       有理式の第n係数 / O(M(K)log n)、初回多項式除算は別途
FPSMint   linear_recurrence(a,c,n) k階漸化式の第n項 / O(M(k)log n)
FPSVector berlekamp_massey(s)      有限列に合う最短漸化式 / O(N^2)
Kは処理するP,Qの項数の最大、M(K)はFPS乗算の計算量。

基本の関数
FPS P={1},Q={1,-1,-1};
FPSMint coefficient=bostan_mori(P,Q,100);
FPSVector a={0,1},c={1,1};
FPSMint fib100=linear_recurrence(a,c,100);
FPSVector sequence={0,1,1,2,3,5,8,13,21,34};
auto recurrence=berlekamp_massey(sequence);
FPSVector first(sequence.begin(),sequence.begin()+recurrence.size());
FPSMint fib1000=linear_recurrence(first,recurrence,1000);
*/
