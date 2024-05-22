#pragma once

namespace siga::util {

template<typename T, bool UseRoundBrackets = true>
class [[nodiscard]] construct_t
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

class [[nodiscard]] indirect_t
{
public:
    template<typename T>
    static constexpr decltype(auto) operator()(T &&object)
        noexcept(noexcept(*std::forward<T>(object))) {
        return *std::forward<T>(object);
    }
};

inline constexpr indirect_t indirect;

// ------------------------------------------------------------------------------------------------

template<typename F>
class [[nodiscard]] lazy_eval_t
{
public:
    constexpr lazy_eval_t(F func) noexcept(std::is_nothrow_move_constructible_v<F>)
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

class [[nodiscard]] get_reference_t
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
class [[nodiscard]] equal_to_t
{
public:
    constexpr equal_to_t(T data) noexcept(std::is_nothrow_move_constructible_v<T>)
        : data_{std::move(data)} {}

public:
    template<typename Self, typename Rhs>
    [[nodiscard]] constexpr decltype(auto) operator()(this Self &&self, Rhs &&rhs)
        noexcept(noexcept(get_reference(std::forward<Self>(self).data_) == std::forward<Rhs>(rhs))
        ) {
        return get_reference(std::forward<Self>(self).data_) == std::forward<Rhs>(rhs);
    }

private:
    T data_;
};

// ------------------------------------------------------------------------------------------------

template<typename T>
class [[nodiscard]] not_equal_to_t
{
public:
    constexpr not_equal_to_t(T data) noexcept(std::is_nothrow_move_constructible_v<T>)
        : data_{std::move(data)} {}

public:
    template<typename Self, typename Rhs>
    [[nodiscard]] constexpr decltype(auto) operator()(this Self &&self, Rhs &&rhs)
        noexcept(noexcept(get_reference(std::forward<Self>(self).data_) != std::forward<Rhs>(rhs))
        ) {
        return get_reference(std::forward<Self>(self).data_) != std::forward<Rhs>(rhs);
    }

private:
    T data_;
};

// ------------------------------------------------------------------------------------------------

template<typename T>
class [[nodiscard]] return_t
{
public:
    constexpr return_t(T data) noexcept(std::is_nothrow_move_constructible_v<T>)
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
class [[nodiscard]] return_t<void>
{
public:
    constexpr void operator()(auto &&...) noexcept {}
};

// ------------------------------------------------------------------------------------------------

template<typename F, typename... RestF>
class [[nodiscard]] compose_t
{
private:
    using rest_t = compose_t<RestF...>;

public:
    constexpr compose_t(F func, RestF... rest_funcs) noexcept(
        std::is_nothrow_move_constructible_v<F> &&
        (... && std::is_nothrow_move_constructible_v<RestF>)
    )
        : func_{std::move(func)}
        , rest_{std::move(rest_funcs)...} {}

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
class [[nodiscard]] compose_t<F>
{
public:
    constexpr compose_t(F func) noexcept(std::is_nothrow_move_constructible_v<F>)
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

// ------------------------------------------------------------------------------------------------

template<typename T>
class [[nodiscard]] typed_get_t
{
public:
    template<typename Gettable>
    [[nodiscard]] static constexpr decltype(auto) operator()(Gettable &&gettable)
        noexcept(noexcept(get<T>(std::declval<Gettable>()))) //
    {
        return get<T>(std::forward<Gettable>(gettable));
    }
};

template<auto V>
class [[nodiscard]] valued_get_t
{
public:
    template<typename Gettable>
    [[nodiscard]] static constexpr decltype(auto) operator()(Gettable &&gettable)
        noexcept(noexcept(get<V>(std::declval<Gettable>()))) //
    {
        return get<V>(std::forward<Gettable>(gettable));
    }
};

template<typename T>
constexpr typed_get_t<T> make_get() noexcept {
    return {};
}

template<auto V>
constexpr valued_get_t<V> make_get() noexcept {
    return {};
}

inline constexpr auto get_key = make_get<0>();
inline constexpr auto get_value = make_get<1>();

} // namespace siga::util

#define SIGA_UTIL_LIFT(X)                                                                          \
    []<typename... Args>(Args &&...args                                                            \
    ) constexpr noexcept(noexcept(X(std::forward<Args>(args)...))) -> decltype(auto) {             \
        return X(std::forward<Args>(args)...);                                                     \
    }

#define SIGA_UTIL_LIFT_MEM_FN(METHOD)                                                              \
    []<typename Object, typename... Args>(Object &&object, Args &&...args) constexpr noexcept(     \
        noexcept(std::declval<Object>().METHOD(std::declval<Args>()...))                           \
    ) -> decltype(auto) {                                                                          \
        return std::forward<Object>(object).METHOD(std::forward<Args>(args)...);                   \
    }
