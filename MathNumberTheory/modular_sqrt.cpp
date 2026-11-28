long long modular_sqrt(long long y, long long prime) {
    if (prime < 2) std::abort();
    y %= prime;
    if (y < 0) y += prime;
    if (y <= 1 || prime == 2) return y;
    auto power = [&](long long x, long long exp) { return powmod(x,exp,prime); };
    auto multiply = [&](long long x, long long z) {
        return static_cast<long long>(static_cast<__int128>(x)*z%prime);
    };
    if (power(y,(prime-1)/2) != 1) return -1;
    if (prime%4 == 3) return power(y,prime/4+1);

    long long odd = prime-1;
    int height = std::countr_zero(static_cast<unsigned long long>(odd));
    odd >>= height;
    long long nonresidue = 2;
    while (power(nonresidue,(prime-1)/2) != prime-1) ++nonresidue;
    long long root = power(y,odd/2+1);
    long long rest = power(y,odd);
    long long factor = power(nonresidue,odd);
    while (rest != 1) {
        int i = 1;
        for (auto x = multiply(rest,rest); x != 1; x = multiply(x,x)) ++i;
        const long long b = power(factor,1LL<<(height-i-1));
        root = multiply(root,b);
        factor = multiply(b,b);
        rest = multiply(rest,factor);
        height = i;
    }
    return root;
}

/*
細かい注意点
法primeはlong longの範囲の素数。yは任意のlong long。素数判定は呼び出し側で行う。
2つの根がある場合は一方を返す。戻り値は[0,prime)または-1。
基本テンプレートのpowmodを使用。Zは平方非剰余の探索で試した個数。

使える関数一覧
modular_sqrt(y,prime); // long long: x^2=y (mod prime) の根を1つ。存在しなければ-1
                       // 時間O((Z+log prime)log prime)、追加領域O(log prime)

基本の関数
auto root=modular_sqrt(10,13);
if(root<0 || root*root%13!=10 || modular_sqrt(2,13)!=-1)std::abort();
*/
