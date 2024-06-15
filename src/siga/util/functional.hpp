#pragma once

#include "siga/util/meta.hpp"
#include "siga/util/tuple.hpp"

namespace siga::util {

template<typename T, bool UseRoundBrackets = true>
class [[nodiscard]] construct_t
{
public:
    template<typename... Args>
    [[nodiscard]] static constexpr T operator()(Args &&...args)
        noexcept(std::is_nothrow_constructible_v<T, Args &&...>)
    {
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
        noexcept(noexcept(*std::forward<T>(object)))
    {
        return *std::forward<T>(object);
    }
};

inline constexpr indirect_t indirect;

// ------------------------------------------------------------------------------------------------

template<typename F>
class [[nodiscard]] lazy_eval
{
public:
    constexpr lazy_eval(F func) noexcept(std::is_nothrow_move_constructible_v<F>)
        : func_{std::move(func)}
    {
    }

public:
    constexpr operator decltype(auto)() & noexcept(std::is_nothrow_invocable_v<F &>)
    {
        return std::invoke(func_);
    }

    constexpr operator decltype(auto)() const & noexcept(std::is_nothrow_invocable_v<const F &>)
    {
        return std::invoke(func_);
    }

    constexpr operator decltype(auto)() && noexcept(std::is_nothrow_invocable_v<F &&>)
    {
        return std::invoke(std::move(func_));
    }

    constexpr operator decltype(auto)() const && noexcept(std::is_nothrow_invocable_v<const F &&>)
    {
        return std::invoke(std::move(func_));
    }

private:
    F func_;
};

// TODO: noexcept
template<typename F, typename... Args>
[[nodiscard]] constexpr auto lazy_eval_bind(F &&func, Args &&...args)
{
    if constexpr(sizeof...(args) == 0) {
        return lazy_eval{std::forward<F>(func)};
    } else {
        return lazy_eval{std::bind_front(std::forward<F>(func), std::forward<Args>(args)...)};
    }
}

// ------------------------------------------------------------------------------------------------

class [[nodiscard]] get_reference_t
{
public:
    template<typename T>
    [[nodiscard]] static constexpr decltype(auto) operator()(T &&value) noexcept
    {
        return std::forward<T>(value);
    }

    template<typename T>
    [[nodiscard]] static constexpr decltype(auto) operator()(std::reference_wrapper<T> value
    ) noexcept
    {
        return value.get();
    }
};

inline constexpr get_reference_t get_reference;

// ------------------------------------------------------------------------------------------------

template<typename T>
class [[nodiscard]] equal_to
{
public:
    constexpr equal_to(T data) noexcept(std::is_nothrow_move_constructible_v<T>)
        : data_{std::move(data)}
    {
    }

public:
    template<typename Self, typename Rhs>
    [[nodiscard]] constexpr decltype(auto) operator()(this Self &&self, Rhs &&rhs)
        noexcept(noexcept(get_reference(std::forward<Self>(self).data_) == std::forward<Rhs>(rhs)))
    {
        return get_reference(std::forward<Self>(self).data_) == std::forward<Rhs>(rhs);
    }

private:
    T data_;
};

// ------------------------------------------------------------------------------------------------

template<typename T>
class [[nodiscard]] not_equal_to
{
public:
    constexpr not_equal_to(T data) noexcept(std::is_nothrow_move_constructible_v<T>)
        : data_{std::move(data)}
    {
    }

public:
    template<typename Self, typename Rhs>
    [[nodiscard]] constexpr decltype(auto) operator()(this Self &&self, Rhs &&rhs)
        noexcept(noexcept(get_reference(std::forward<Self>(self).data_) != std::forward<Rhs>(rhs)))
    {
        return get_reference(std::forward<Self>(self).data_) != std::forward<Rhs>(rhs);
    }

private:
    T data_;
};

// ------------------------------------------------------------------------------------------------

template<typename T>
class [[nodiscard]] return_value
{
public:
    using return_type = std::decay_t<std::unwrap_reference_t<T>>;

public:
    constexpr return_value(T data) noexcept(std::is_nothrow_move_constructible_v<T>)
        : data_{std::move(data)}
    {
    }

public:
    template<typename Self>
    [[nodiscard]] constexpr return_type operator()(this Self &&self)
        noexcept(std::is_nothrow_constructible_v<return_type, copy_cv_ref_t<Self, T>>)
    {
        return std::forward<Self>(self).data_;
    }

private:
    T data_;
};

template<>
class [[nodiscard]] return_value<void>
{
public:
    using return_type = void;

public:
    constexpr return_type operator()() noexcept {}
};

// ------------------------------------------------------------------------------------------------

template<std::invocable F>
class [[nodiscard]] ignore_args
{
public:
    constexpr ignore_args(F func) noexcept(std::is_nothrow_move_constructible_v<F>)
        : func_{std::move(func)}
    {
    }

public:
    template<typename Self>
    constexpr decltype(auto) operator()(this Self &&self, auto &&...)
        noexcept(std::is_nothrow_invocable_v<copy_cv_ref_t<Self, F>>)
    {
        return std::invoke(std::forward<Self>(self).func_);
    }

private:
    F func_;
};

// ------------------------------------------------------------------------------------------------

template<typename F>
class [[nodiscard]] stored_func_invoker
{
public:
    constexpr stored_func_invoker(F func) noexcept(std::is_nothrow_move_constructible_v<F>)
        : func_{std::move(func)}
    {
    }

public:
    template<typename Self, typename... Args>
    constexpr decltype(auto) operator()(this Self &&self, Args &&...args)
        noexcept(std::is_nothrow_invocable_v<copy_cv_ref_t<Self, F>, Args &&...>)
    {
        return std::invoke(std::forward<Self>(self).func_, std::forward<Args>(args)...);
    }

private:
    F func_;
};

// ------------------------------------------------------------------------------------------------

// TODO: noexcept
template<typename ArgsTuple, typename F, typename... RestF>
requires tuple_like<std::decay_t<ArgsTuple>> && applyable<F, ArgsTuple>
constexpr decltype(auto) invoke_sequentially(ArgsTuple &&args, F &&func, RestF &&...rest)
{
    if constexpr(sizeof...(rest) == 0) {
        return std::apply(std::forward<F>(func), std::forward<ArgsTuple>(args));
    } else {
        return invoke_sequentially(
            std::forward_as_tuple(std::apply(std::forward<F>(func), std::forward<ArgsTuple>(args))),
            std::forward<RestF>(rest)...
        );
    }
}

template<typename... Fs>
class [[nodiscard]] compose
{
public:
    constexpr compose(Fs... funcs) noexcept((... && std::is_nothrow_move_constructible_v<Fs>))
        : funcs_{std::move(funcs)...}
    {
    }

public:
    // TODO: noexcept
    template<typename Self, typename... Args>
    constexpr decltype(auto) operator()(this Self &&self, Args &&...args)
    {
        return std::forward<Self>(self).invoke_sequentially_impl(
            std::forward_as_tuple(std::forward<Args>(args)...),
            std::index_sequence_for<Fs...>()
        );
    }

private:
    // clang-format off
    template<typename Self, typename ArgsTuple, std::size_t... Is>
    constexpr decltype(auto) invoke_sequentially_impl(
        this Self &&self,
        ArgsTuple &&args,
        std::index_sequence<Is...>
    ) {
        return invoke_sequentially(
            std::forward<ArgsTuple>(args),
            std::get<Is>(std::forward<Self>(self).funcs_)...
        );
    }
    // clang-format on

private:
    std::tuple<Fs...> funcs_;
};

// ------------------------------------------------------------------------------------------------

template<typename T>
class [[nodiscard]] typed_get
{
public:
    template<typename Gettable>
    [[nodiscard]] static constexpr decltype(auto) operator()(Gettable &&gettable)
        noexcept(noexcept(get<T>(std::declval<Gettable>())))
    {
        return get<T>(std::forward<Gettable>(gettable));
    }
};

template<auto V>
class [[nodiscard]] valued_get
{
public:
    template<typename Gettable>
    [[nodiscard]] static constexpr decltype(auto) operator()(Gettable &&gettable)
        noexcept(noexcept(get<V>(std::declval<Gettable>())))
    {
        return get<V>(std::forward<Gettable>(gettable));
    }
};

template<typename T>
constexpr typed_get<T> make_get() noexcept
{
    return {};
}

template<auto V>
constexpr valued_get<V> make_get() noexcept
{
    return {};
}

inline constexpr auto get_key = make_get<0>();
inline constexpr auto get_value = make_get<1>();

// ------------------------------------------------------------------------------------------------

template<typename... Ts>
class [[nodiscard]] overload : public Ts...
{
public:
    using Ts::operator()...;
};

// ------------------------------------------------------------------------------------------------

template<typename F>
class [[nodiscard]] make_bind_expression : public stored_func_invoker<F>
{};

template<typename F>
make_bind_expression(F) -> make_bind_expression<F>;

// ------------------------------------------------------------------------------------------------

} // namespace siga::util

template<typename F>
class std::is_bind_expression<siga::util::make_bind_expression<F>> : public std::true_type
{};

// ------------------------------------------------------------------------------------------------

// clang-format off
#define SIGA_UTIL_LIFT(X)                                                                          \
    []<typename... Args>(Args &&...args)                                                           \
        constexpr                                                                                  \
        static                                                                                     \
        noexcept(noexcept(X(std::forward<Args>(args)...)))                                         \
        -> decltype(auto)                                                                          \
    {                                                                                              \
        return X(std::forward<Args>(args)...);                                                     \
    }
// clang-format on

// ------------------------------------------------------------------------------------------------

// clang-format off
#define SIGA_UTIL_LIFT_MEM_FN(METHOD)                                                              \
    []<typename Object, typename... Args>(Object &&object, Args &&...args)                         \
        constexpr                                                                                  \
        static                                                                                     \
        noexcept(noexcept(std::declval<Object>().METHOD(std::declval<Args>()...)))                 \
        -> decltype(auto)                                                                          \
    {                                                                                              \
        return std::forward<Object>(object).METHOD(std::forward<Args>(args)...);                   \
    }
// clang-format on
