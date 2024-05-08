#pragma once

namespace siga::util {

template<typename F>
class lazy_eval_t
{
public:
    explicit constexpr lazy_eval_t(F func) noexcept(std::is_nothrow_move_constructible_v<F>)
        : func_{std::move(func)} {}

    constexpr operator decltype(auto)() & noexcept(std::is_nothrow_invocable_v<F &>) {
        return std::invoke(func_);
    }

    constexpr operator decltype(auto)() const & noexcept(std::is_nothrow_invocable_v<const F &>) {
        return std::invoke(func_);
    }

    constexpr operator decltype(auto)() && noexcept(std::is_nothrow_invocable_v<F &&>) {
        return std::invoke(std::move(func_));
    }

    constexpr operator decltype(auto)() const && noexcept(std::is_nothrow_invocable_v<const F &&>) {
        return std::invoke(std::move(func_));
    }

private:
    F func_;
};

template<typename F, typename... Args>
constexpr auto lazy_eval_bind(F &&func, Args &&...args) {
    if constexpr(sizeof...(args) == 0) {
        return lazy_eval_t{std::forward<F>(func)};
    } else {
        return lazy_eval_t{std::bind_front(std::forward<F>(func), std::forward<Args>(args)...)};
    }
}

} // namespace siga::util

// clang-format off

#define SIGA_UTIL_LIFT(X)                                                      \
    []<typename... Args>(Args &&...args) constexpr                             \
    noexcept(noexcept(X(std::forward<Args>(args)...)))                         \
    -> decltype(auto)                                                          \
    { return X(std::forward<Args>(args)...); }

#define SIGA_UTIL_LIFT_MEM_FN(METHOD)                                          \
    []<typename Object, typename... Args>                                      \
    (Object &&object, Args &&...args) constexpr                                \
    noexcept(noexcept(std::declval<Object>().METHOD(std::declval<Args>()...))) \
    -> decltype(auto)                                                          \
    { return std::forward<Object>(object).METHOD(std::forward<Args>(args)...); }

// clang-format on
