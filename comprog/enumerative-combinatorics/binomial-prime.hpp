#ifndef WASD314_COMPROG_ENUMERATIVE_COMBINATORICS_BINOMIAL_PRIME_HPP_
#define WASD314_COMPROG_ENUMERATIVE_COMBINATORICS_BINOMIAL_PRIME_HPP_

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <ranges>
#include <utility>
#include <vector>

#include "comprog/utility/types.hpp"

namespace comprog::enumerative_combinatorics {
template <typename T>
struct BinomialPrime {
    std::vector<T> _fact, _inv_fact, _inv;

    constexpr BinomialPrime(int max = 0)
        : _fact{T{1}}, _inv_fact{T{1}}, _inv{T{1}} {
        if (max > 0) extend(max + 1);
    };
    constexpr void extend(int max = -1) {
        int n = _fact.size();
        if (max == -1 || max < n * 2) max = n * 2;
        max = std::min(max + 1, T::mod());
        if (max <= n) return;
        _fact.resize(max);
        _inv_fact.resize(max);
        _inv.resize(max);
        for (int i : std::views::iota(n, max)) _fact[i] = _fact[i - 1] * T{i};
        _inv_fact[max - 1] = T{1} / _fact[max - 1];
        _inv[max - 1] = _inv_fact[max - 1] * _fact[max - 2];
        for (int i : std::views::iota(n, max - 1) | std::views::reverse) {
            _inv_fact[i] = _inv_fact[i + 1] * T{i + 1};
            _inv[i] = _inv_fact[i] * _fact[i - 1];
        }
    }
    constexpr T fact(int i) {
        if (i < 0) return T{0};
        extend(i);
        return _fact[i];
    };
    constexpr T inv_fact(int i) {
        if (i < 0) return T{0};
        extend(i);
        return _inv_fact[i];
    };
    constexpr T inv(int i) {
        assert(i != 0);
        if (i < 0) return -inv(-i);
        extend(i);
        return _inv[i];
    };

    // cf. https://hos-lyric.hatenablog.com/entry/2021/06/10/004947
    // - C(n, k) == C(n, n - k) for all (n, k)
    // - k * C(n, k) == n * C(n - 1, k - 1) for all (n, k)
    // - C(n, k) == C(n - 1, k - 1) + C(n - 1, k) for all (n, k) except (0, 0)
    constexpr T c(int n, int k) {
        if (n < 0) {
            if (k >= 0) return (k % 2 ? T{-1} : T{1}) * c(-n + k - 1, k);
            if (k <= n) return ((n - k) % 2 ? T{-1} : T{1}) * c(-k - 1, n - k);
            return T{0};
        }
        if (0 <= k && k <= n) return fact(n) * inv_fact(k) * inv_fact(n - k);
        return T{0};
    }
    constexpr inline T operator()(int n, int k) { return c(n, k); }
    constexpr T c_naive(i64 n, int k) {
        if (n < 0) {
            if (k >= 0) return (k % 2 ? T{-1} : T{1}) * c(-n + k - 1, k);
            if (k <= n) return ((n - k) % 2 ? T{-1} : T{1}) * c(-k - 1, n - k);
            return T{0};
        }
        k = std::min<i64>(k, n - k);
        if (k < 0) return T{0};
        T ans = inv_fact(k);
        for (int i : std::views::iota(0, k)) ans *= T{n - i};
        return ans;
    }

    constexpr T p(int n, int k) {
        if (n < 0 || k < 0 || n < k) return T{0};
        return fact(n) * inv_fact(n - k);
    }
    // [x^k] 1 / (1-x)^n
    constexpr T h(int n, int k) {
        if (k < 0) return T{0};
        return c(n + k - 1, k);
    }
    // [x^k] 1 / (1-x)^n
    constexpr T h_naive(int n, int k) {
        if (k < 0) return T{0};
        return c_naive(n + k - 1, k);
    }

    template <std::ranges::input_range R>
        requires std::convertible_to<std::ranges::range_reference_t<R>, int>
    constexpr T multinomial(R &&rg) {
        T den{1};
        int n = 0;
        for (int k : rg) {
            den *= inv_fact(k);
            n += k;
        }
        return den * fact(n);
    }
    constexpr T multinomial(std::initializer_list<int> il) {
        return multinomial(std::ranges::subrange(il));
    }
    template <std::ranges::input_range R>
        requires std::convertible_to<std::ranges::range_reference_t<R>, int>
    constexpr inline T operator()(R &&rg) {
        return multinomial(std::forward(rg));
    }
    constexpr inline T operator()(std::initializer_list<int> il) {
        return multinomial(il);
    }
};
}  // namespace comprog::enumerative_combinatorics
namespace comprog {
template <typename T>
constexpr enumerative_combinatorics::BinomialPrime<T> binom;
}

#endif  // WASD314_COMPROG_ENUMERATIVE_COMBINATORICS_BINOMIAL_PRIME_HPP_
