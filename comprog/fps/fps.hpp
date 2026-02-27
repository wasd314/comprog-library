#ifndef WASD314_COMPROG_FPS_FPS_HPP_
#define WASD314_COMPROG_FPS_FPS_HPP_

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <ranges>
#include <utility>
#include <vector>

#include "comprog/enumerative-combinatorics/binomial-prime.hpp"
#include "comprog/fps/dispatch-tag.hpp"
#include "comprog/fps/fps-declare.hpp"
#include "comprog/utility/types.hpp"

namespace comprog::fps {
template <typename T>
struct Fps {
    using Vec = std::vector<T>;
    Vec _f;

    using reference = Vec::reference;
    using const_reference = Vec::const_reference;
    using iterator = Vec::iterator;
    using const_iterator = Vec::const_iterator;
    using size_type = i64;
    using difference_type = i64;
    using value_type = Vec::value_type;

    constexpr Fps() = default;
    constexpr explicit Fps(size_t n) : _f(n) {}
    constexpr explicit Fps(size_t n, const T &value) : _f(n, value) {}
    template <typename InputIter>
    constexpr Fps(InputIter first, InputIter last) : _f(first, last) {}
    constexpr Fps(const Vec &f) : _f(f) {}
    constexpr Fps(Vec &&f) : _f(std::move(f)) {}
    constexpr Fps(std::initializer_list<T> il) : Fps(il.begin(), il.end()) {}

    template <std::ranges::input_range R>
        requires std::convertible_to<std::ranges::range_reference_t<R>, T>
    constexpr Fps(std::from_range_t, R &&rg)
        : _f(std::from_range, std::forward<R>(rg)) {}

    constexpr auto begin() { return _f.begin(); }
    constexpr auto begin() const { return _f.begin(); }
    constexpr auto end() { return _f.end(); }
    constexpr auto end() const { return _f.end(); }

    constexpr i64 size() const { return _f.size(); }
    constexpr bool empty() const { return _f.empty(); }
    constexpr void reserve(size_t n) { _f.reserve(n); }
    constexpr void resize(size_t n) { _f.resize(n); }
    constexpr void resize(size_t n, const T &c) { _f.resize(n, c); }

    constexpr decltype(auto) operator[](i64 n) {
        return _f[n >= 0 ? n : size() + n];
    }
    constexpr decltype(auto) operator[](i64 n) const {
        return _f[n >= 0 ? n : size() + n];
    }
    constexpr auto &front() { return _f.front(); }
    constexpr auto &front() const { return _f.front(); }
    constexpr auto &back() { return _f.back(); }
    constexpr auto &back() const { return _f.back(); }

    constexpr void push_back(const T &x) { _f.push_back(x); }
    constexpr void push_back(T &&x) { _f.push_back(std::move(x)); }
    template <class... Args>
    constexpr decltype(auto) emplace_back(Args &&...args) {
        return _f.emplace_back(std::forward<Args>(args)...);
    }
    constexpr void pop_back() { _f.pop_back(); }
    template <class... Args>
    decltype(auto) erase(Args &&...args) {
        return _f.erase(std::forward<Args>(args)...);
    }
    template <class... Args>
    decltype(auto) insert(Args &&...args) {
        return _f.insert(std::forward<Args>(args)...);
    }
    constexpr void clear() { _f.clear(); }

    constexpr Fps &operator+=(const Fps &r) {
        if (size() < r.size()) resize(r.size());
        for (i64 i : std::views::iota(0LL, size())) _f[i] += r[i];
        return *this;
    }
    constexpr Fps &operator+=(const T &r) {
        if (empty()) resize(1);
        _f[0] += r;
        return *this;
    }
    constexpr Fps &operator-=(const Fps &r) {
        if (size() < r.size()) resize(r.size());
        for (i64 i : std::views::iota(0LL, size())) _f[i] -= r[i];
        return *this;
    }
    constexpr Fps &operator-=(const T &r) {
        if (empty()) resize(1);
        _f[0] -= r;
        return *this;
    }
    constexpr Fps &operator*=(const Fps &) {
        static_assert(false, "Not implemented: Fps *= Fps");
    }
    constexpr Fps &operator*=(const T &r) {
        if (r == T{1}) {
        } else if (r == T{-1}) {
            for (auto &e : _f) e = -e;
        } else {
            for (auto &e : _f) e *= r;
        }
        return *this;
    }
    constexpr Fps &operator/=(const T &r) { return *this *= (T{1} / r); }
    constexpr Fps operator-() const & {
        Fps ans(*this);
        for (auto &e : ans._f) e - -e;
        return ans;
    }
    constexpr Fps operator-() && {
        Fps ans{std::move(*this)};
        for (auto &e : ans._f) e - -e;
        return ans;
    }
    constexpr Fps operator+(const Fps &r) const & { return Fps(*this) += r; }
    constexpr Fps operator+(const Fps &r) && { return std::move(*this) += r; }
    constexpr Fps operator+(Fps &&r) const & { return std::move(r) += *this; }
    constexpr Fps operator+(Fps &&r) && {
        return size() >= r.size() ? std::move(*this) + r : std::move(r) + *this;
    }
    constexpr Fps operator-(const Fps &r) const & { return Fps(*this) -= r; }
    constexpr Fps operator-(const Fps &r) && { return std::move(*this) -= r; }
    constexpr Fps operator-(Fps &&r) const & { return -std::move(r) += *this; }
    constexpr Fps operator-(Fps &&r) && {
        return size() >= r.size() ? std::move(*this) -= r
                                  : -std::move(r) + *this;
    }
    // constexpr Fps operator*(const Fps &r) const & { return Fps(*this) -= r; }

    constexpr void shrink() {
        while (!empty() && back() == T{0}) pop_back();
    }
    constexpr void reverse() { std::ranges::reverse(_f); }
    constexpr Fps reversed() {
        Fps ans(_f);
        ans.reverse();
        return ans;
    }
    constexpr Fps slice(i64 left, i64 right) const {
        if (left < 0) left += size();
        if (right < 0) right += size();
        left = std::min(left >= 0 ? left : left + size(), size());
        right = std::clamp(right >= 0 ? right : right + size(), left, size());
        return {begin() + left, begin() + right};
    }
    // 先頭 `len` 項． `len >= 0` かつ足りない場合末尾を 0 で埋める．
    constexpr Fps prefix(i64 len) const {
        Fps ans = slice(0, len);
        if (len >= 0 && ans.size() < len) ans.resize(len);
        return ans;
    }
    // `from` 次以降．足りない場合空列．
    constexpr Fps suffix(i64 from) const { return slice(from, size()); }

    constexpr int count_nonzero() const {
        return std::ranges::count_if(_f, [](T e) { return e != T{0}; });
    }

    constexpr void differentiate() {
        if (empty()) return;
        for (i64 i : std::views::iota(1LL, size())) _f[i] *= i;
        erase(begin());
    }
    constexpr void integrate() {
        insert(begin(), T{0});
        for (i64 i : std::views::iota(1LL, size())) _f[i] *= binom<T>.inv(i);
    }
    constexpr Fps differentiated() const {
        Fps ans(*this);
        ans.differentiate();
        return ans;
    }
    constexpr Fps integrated() const {
        Fps ans(*this);
        ans.integrate();
        return ans;
    }

    constexpr void fact_mul() {
        for (i64 i : std::views::iota(0LL, size())) _f[i] *= binom<T>.fact(i);
    }
    constexpr void fact_div() {
        for (i64 i : std::views::iota(0LL, size()))
            _f[i] *= binom<T>.inv_fact(i);
    }
    constexpr Fps fact_muled() const {
        Fps ans(*this);
        ans.fact_mul();
        return ans;
    }
    constexpr Fps fact_dived() const {
        Fps ans(*this);
        ans.fact_div();
        return ans;
    }

    Fps inv(i64 len = -1) const { return inv(Auto{}, len); }
    template <typename Tag>
    Fps inv(Tag, i64 len = -1) const {
        using namespace impl;
        return impl_inv(Auto{}, *this, len);
    }

    Fps log(i64 len = -1) const { return log(Auto{}, len); }
    template <typename Tag>
    Fps log(Tag, i64 len = -1) const {
        using namespace impl;
        return impl_log(Auto{}, *this, len);
    }

    Fps exp(i64 len = -1) const { return exp(Auto{}, len); }
    template <typename Tag>
    Fps exp(Tag, i64 len = -1) const {
        using namespace impl;
        return impl_exp(Auto{}, *this, len);
    }

    Fps pow(i64 e, i64 len = -1) const { return pow(Auto{}, e, len); }
    template <typename Tag>
    Fps pow(Tag, i64 e, i64 len = -1) const {
        using namespace impl;
        return impl_pow(Auto{}, *this, e, len);
    }

    Fps sqrt(i64 len = -1) const { return sqrt(Auto{}, len); }
    template <typename Tag>
    Fps sqrt(Tag, i64 len = -1) const {
        using namespace impl;
        return impl_sqrt(Auto{}, *this, len);
    }
};
}  // namespace comprog::fps

#endif  // WASD314_COMPROG_FPS_FPS_HPP_
