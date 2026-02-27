// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/binomial_coefficient_prime_mod
#include <iostream>
#include <ranges>

#include <atcoder/all>

#include "comprog/enumerative-combinatorics/binomial-prime.hpp"

int main() {
    using namespace std;
    int t, m;
    cin >> t >> m;
    using mint = atcoder::modint;
    mint::set_mod(m);
    comprog::enumerative_combinatorics::BinomialPrime<mint> binom;
    for (int _ : views::iota(0, t)) {
        int n, k;
        cin >> n >> k;
        cout << binom(n, k).val() << "\n";
    }
}
