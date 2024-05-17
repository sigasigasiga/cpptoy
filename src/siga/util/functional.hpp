#pragma once

namespace siga::util {

template<typename T, bool UseRoundBrackets = true>
class construct_t
{
public:
    template<typename... Args>
    [[nodiscard]] static constexpr T operator()(Args &&...args)
        noexcept(std::is_nothrow_constructible_v<T, Args &&...>) {
        if constexpr(UseRoundBrackets) {
            return T(std::forward<Args>(args)...);
        } else {
            return T{std::forward<Args>(args)...};
        }
    }
};

template<typename T, bool UseRoundBrackets = true>
inline constexpr construct_t<T, UseRoundBrackets> construct;

// ------------------------------------------------------------------------------------------------

class indirect_t
{
public:
    // TODO: noexcept
    template<typename T>
    [[nodiscard]] static constexpr decltype(auto) operator()(T &&object) {
        return *std::forward<T>(object);
    }
};

inline constexpr indirect_t indirect;

// ------------------------------------------------------------------------------------------------

template<typename F>
class lazy_eval_t
{
public:
    [[nodiscard]] constexpr lazy_eval_t(F func) noexcept(std::is_nothrow_move_constructible_v<F>)
        : func_{std::move(func)} {}

public:
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

// TODO: noexcept
template<typename F, typename... Args>
[[nodiscard]] constexpr auto lazy_eval_bind(F &&func, Args &&...args) {
    if constexpr(sizeof...(args) == 0) {
        return lazy_eval_t{std::forward<F>(func)};
    } else {
        return lazy_eval_t{std::bind_front(std::forward<F>(func), std::forward<Args>(args)...)};
    }
}

// ------------------------------------------------------------------------------------------------

class get_reference_t
{
public:
    template<typename T>
    [[nodiscard]] static constexpr decltype(auto) operator()(T &&value) noexcept {
        return std::forward<T>(value);
    }

    template<typename T>
    [[nodiscard]] static constexpr decltype(auto) operator()(std::reference_wrapper<T> value
    ) noexcept {
        return value.get();
    }
};

inline constexpr get_reference_t get_reference;

// ------------------------------------------------------------------------------------------------

template<typename T>
class equal_to_t
{
public:
    [[nodiscard]] constexpr equal_to_t(T data) noexcept(std::is_nothrow_move_constructible_v<T>)
        : data_{std::move(data)} {}

public:
    // TODO: noexcept
    template<typename Self, typename Rhs>
    [[nodiscard]] constexpr decltype(auto) operator()(this Self &&self, Rhs &&rhs) {
        return get_reference(std::forward<Self>(self).data_) == std::forward<Rhs>(rhs);
    }

private:
    T data_;
};

// ------------------------------------------------------------------------------------------------

template<typename T>
class not_equal_to_t
{
public:
    [[nodiscard]] constexpr not_equal_to_t(T data) noexcept(std::is_nothrow_move_constructible_v<T>)
        : data_{std::move(data)} {}

public:
    // TODO: noexcept
    template<typename Self, typename Rhs>
    [[nodiscard]] constexpr decltype(auto) operator()(this Self &&self, Rhs &&rhs) {
        return get_reference(std::forward<Self>(self).data_) == std::forward<Rhs>(rhs);
    }

private:
    T data_;
};

// ------------------------------------------------------------------------------------------------

template<typename T>
class return_t
{
public:
    [[nodiscard]] constexpr return_t(T data) noexcept(std::is_nothrow_move_constructible_v<T>)
        : data_{std::move(data)} {}

public:
    // TODO: noexcept
    template<typename Self>
    [[nodiscard]] constexpr auto operator()(this Self &&self, auto &&...) {
        return get_reference(std::forward<Self>(self).data_);
    }

private:
    T data_;
};

template<>
class return_t<void>
{
public:
    constexpr void operator()(auto &&...) noexcept {}
};

// ------------------------------------------------------------------------------------------------

template<typename F, typename... Rest>
class compose_t
{
private:
    using rest_t = compose_t<Rest...>;

public:
    constexpr compose_t(
        F func,
        Rest... rest
    ) noexcept(std::is_nothrow_constructible_v<F> && std::is_nothrow_constructible_v<rest_t, Rest...>)
        : func_{std::move(func)}
        , rest_{std::move(rest)...} {}

public:
    // TODO: noexcept
    template<typename Self, typename... Args>
    constexpr decltype(auto) operator()(this Self &&self, Args &&...args) {
        return std::invoke(
            std::forward<Self>(self).rest_,
            std::invoke(std::forward<Self>(self).func_, std::forward<Args>(args)...)
        );
    }

private:
    F func_;
    rest_t rest_;
};

template<typename F>
class compose_t<F>
{
public:
    [[nodiscard]] constexpr compose_t(F func) noexcept(std::is_nothrow_constructible_v<F>)
        : func_{std::move(func)} {}

public:
    // TODO: noexcept
    template<typename Self, typename... Args>
    constexpr decltype(auto) operator()(this Self &&self, Args &&...args) {
        return std::invoke(std::forward<Self>(self).func_, std::forward<Args>(args)...);
    }

private:
    F func_;
};

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
