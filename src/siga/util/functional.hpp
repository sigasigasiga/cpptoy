#pragma once

#include "siga/util/meta.hpp"
#include "siga/util/tuple.hpp"
#include "siga/util/utility.hpp"

namespace siga::util {

template<typename T, bool UseRoundBrackets = true>
class [[nodiscard]] construct_t
{
public:
    template<typename... Args>
    [[nodiscard]] static constexpr T operator()(Args &&...args)
        noexcept(std::is_nothrow_constructible_v<T, Args &&...>)
    {
        return T(std::forward<Args>(args)...);
    }
};

template<typename T>
class [[nodiscard]] construct_t<T, false>
{
public:
    template<typename... Args>
    [[nodiscard]] static constexpr T operator()(Args &&...args)
        noexcept(noexcept(T{std::forward<Args>(args)...}))
    {
        return T{std::forward<Args>(args)...};
    }
};

template<typename T, bool UseRoundBrackets = true>
inline constexpr construct_t<T, UseRoundBrackets> construct;

// -------------------------------------------------------------------------------------------------

class [[nodiscard]] indirect_t
{
public:
    template<typename T>
    [[nodiscard]] static constexpr decltype(auto) operator()(T &&object)
        noexcept(noexcept(*std::forward<T>(object)))
    {
        return *std::forward<T>(object);
    }
};

inline constexpr indirect_t indirect;

// -------------------------------------------------------------------------------------------------

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

template<typename F, typename... Args>
[[nodiscard]] constexpr auto lazy_eval_bind(F &&func, Args &&...args)
    noexcept(is_nothrow_decay_copyable_v<F> && (... && is_nothrow_decay_copyable_v<Args>))
{
    if constexpr(sizeof...(args) == 0) {
        return lazy_eval(std::forward<F>(func));
    } else {
        return lazy_eval(std::bind_front(std::forward<F>(func), std::forward<Args>(args)...));
    }
}

// -------------------------------------------------------------------------------------------------

class [[nodiscard]] get_reference_t
{
public:
    template<typename T>
    [[nodiscard]] static constexpr decltype(auto) operator()(T &&value) noexcept
    {
        return std::forward<T>(value);
    }

    template<typename T>
    [[nodiscard]] static constexpr decltype(auto) operator()(std::reference_wrapper<T> ref) noexcept
    {
        return ref.get();
    }
};

inline constexpr get_reference_t get_reference;

// -------------------------------------------------------------------------------------------------

template<typename F>
class [[nodiscard]] get_reference_wrap
{
public:
    constexpr get_reference_wrap(F fn) noexcept(std::is_nothrow_move_constructible_v<F>)
        : fn_{std::move(fn)}
    {
    }

public:
    // clang-format off
    template<typename Self, typename... Args>
    constexpr decltype(auto) operator()(this Self &&self, Args &&...args)
        noexcept(noexcept(std::invoke(
            std::forward<Self>(self).fn_,
            get_reference(std::forward<Args>(args))...
        )))
    {
        return std::invoke(
            std::forward<Self>(self).fn_,
            get_reference(std::forward<Args>(args))...
        );
    }
    // clang-format on

private:
    F fn_;
};

// -------------------------------------------------------------------------------------------------

class [[nodiscard]] subscript
{
public:
    template<typename L, typename R>
    [[nodiscard]] static constexpr decltype(auto) operator()(L &&lhs, R &&rhs)
        noexcept(noexcept(std::forward<L>(lhs)[std::forward<R>(rhs)]))
    {
        return std::forward<L>(lhs)[std::forward<R>(rhs)];
    }
};

// -------------------------------------------------------------------------------------------------

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

// -------------------------------------------------------------------------------------------------

template<std::invocable F>
class [[nodiscard]] ignore_args_wrap
{
public:
    constexpr ignore_args_wrap(F func) noexcept(std::is_nothrow_move_constructible_v<F>)
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

// -------------------------------------------------------------------------------------------------

template<typename F>
class [[nodiscard]] stored_func_invoker
{
public:
    constexpr stored_func_invoker(F func) noexcept(std::is_nothrow_move_constructible_v<F>)
        : func_{std::move(func)}
    {
    }

public:
    // clang-format off
    template<typename Self, typename... Args>
    requires requires(Self &&self, Args &&...args) {
        std::invoke(std::forward<Self>(self).func_, std::forward<Args>(args)...);
    }
    constexpr decltype(auto) operator()(this Self &&self, Args &&...args)
        noexcept(noexcept(
            std::invoke(std::forward<Self>(self).func_, std::forward<Args>(args)...)
        ))
    {
        return std::invoke(std::forward<Self>(self).func_, std::forward<Args>(args)...);
    }
    // clang-format on

private:
    F func_;
};

template<typename F>
requires std::is_empty_v<F> && (!std::is_final_v<F>)
class [[nodiscard]] stored_func_invoker<F> : public F
{};

template<typename F>
stored_func_invoker(F) -> stored_func_invoker<F>;

// -------------------------------------------------------------------------------------------------

template<typename FsTuple, typename... InnerArgs>
class is_nothrow_fold_invocable
{
private:
    template<typename IndexSequence, typename... Args>
    class impl;

    template<std::size_t I, typename... Args>
    class impl<std::index_sequence<I>, Args...>
    {
    private:
        using current_fn_t = tuple_get_type_t<FsTuple, I>;

    public:
        static constexpr bool value = std::is_nothrow_invocable_v<current_fn_t, Args...>;
    };

    template<std::size_t I, std::size_t... Is, typename... Args>
    class impl<std::index_sequence<I, Is...>, Args...>
    {
    private:
        using current_fn_t = tuple_get_type_t<FsTuple, I>;
        using current_ret_t = std::invoke_result_t<current_fn_t, Args...>;

        static constexpr bool current = std::is_nothrow_invocable_v<current_fn_t, Args...>;

    public:
        static constexpr bool value =
            current && impl<std::index_sequence<Is...>, current_ret_t>::value;
    };

private:
    static constexpr std::size_t tuple_size = std::tuple_size_v<std::decay_t<FsTuple>>;

public:
    static constexpr bool value = impl<std::make_index_sequence<tuple_size>, InnerArgs...>::value;
};

template<typename FsTuple, typename... Args>
inline constexpr bool is_nothrow_fold_invocable_v =
    is_nothrow_fold_invocable<FsTuple, Args...>::value;

class [[nodiscard]] fold_invoke_t
{
public:
    template<typename FuncTuple, typename... Args>
    static constexpr decltype(auto) operator()(FuncTuple &&funcs, Args &&...args)
        noexcept(is_nothrow_fold_invocable_v<FuncTuple &&, Args &&...>)
    {
        return impl(
            std::make_index_sequence<std::tuple_size_v<std::decay_t<FuncTuple>>>{},
            std::forward<FuncTuple>(funcs),
            std::forward<Args>(args)...
        );
    }

private:
    // clang-format off
    template<typename FsTuple, std::size_t I, std::size_t... Is, typename... Args>
    requires std::invocable<tuple_get_type_t<FsTuple &&, I>, Args &&...>
    static constexpr decltype(auto) impl(
        std::index_sequence<I, Is...>,
        FsTuple &&fs,
        Args &&...args
    ) {
        if constexpr(sizeof...(Is) == 0) {
            return std::invoke(get<I>(std::forward<FsTuple>(fs)), std::forward<Args>(args)...);
        } else {
            return impl(
                std::index_sequence<Is...>{},
                std::forward<FsTuple>(fs),
                std::invoke(get<I>(std::forward<FsTuple>(fs)), std::forward<Args>(args)...)
            );
        }
    }
    // clang-format on
};

inline constexpr fold_invoke_t fold_invoke;

template<typename... Fs>
[[nodiscard]] constexpr auto compose(Fs &&...fs) noexcept((... && is_nothrow_decay_copyable_v<Fs>))
{
    return std::bind_front(fold_invoke, std::tuple{std::forward<Fs>(fs)...});
}

// -------------------------------------------------------------------------------------------------

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

// -------------------------------------------------------------------------------------------------

template<typename... Ts>
class [[nodiscard]] overload : public Ts...
{
public:
    using Ts::operator()...;
};

// -------------------------------------------------------------------------------------------------

template<typename F>
class [[nodiscard]] make_bind_expression : public stored_func_invoker<F>
{};

template<typename F>
make_bind_expression(F) -> make_bind_expression<F>;

// -------------------------------------------------------------------------------------------------

} // namespace siga::util

template<typename F>
class std::is_bind_expression<siga::util::make_bind_expression<F>> : public std::true_type
{};

// -------------------------------------------------------------------------------------------------

// clang-format off
#define SIGA_UTIL_LIFT(X)                                                                          \
    []<typename... Args>(Args &&...args)                                                           \
        constexpr                                                                                  \
        static                                                                                     \
        noexcept(noexcept(X(std::forward<Args>(args)...)))                                         \
        -> decltype(auto)                                                                          \
        requires requires { X(std::forward<Args>(args)...); }                                      \
    {                                                                                              \
        return X(std::forward<Args>(args)...);                                                     \
    }
// clang-format on

// -------------------------------------------------------------------------------------------------

// clang-format off

// Known limitations:
// 1. Resulting lambda never produces an overload set.
//    That is, if passed to `util::overload`, the result may be surprising
// 2. If `MEMBER` is a niebloid, it'd be called,
//    despite the fact that `std::invoke` would return a reference to it.
//    While this is probably fixable, I'm not sure if it's worth the effort
// 
// TODO: should we also specify a class name? If so, don't forget that types must be _compatible_
#define SIGA_UTIL_LIFT_MEMBER(MEMBER)                                                              \
    []<typename T, typename... Args>                                                               \
        (T &&value, Args &&...args)                                                                \
        constexpr                                                                                  \
        static                                                                                     \
        noexcept(                                                                                  \
            !requires { std::forward<T>(value).MEMBER(std::forward<Args>(args)...); } ||           \
            requires {                                                                             \
                requires(noexcept(std::forward<T>(value).MEMBER(std::forward<Args>(args)...)));    \
            }                                                                                      \
        )                                                                                          \
        -> decltype(auto)                                                                          \
        requires requires { std::forward<T>(value).MEMBER(std::forward<Args>(args)...); } ||       \
                 requires { std::forward<T>(value).MEMBER; }                                       \
    {                                                                                              \
        constexpr bool is_method = requires {                                                      \
            std::forward<T>(value).MEMBER(std::forward<Args>(args)...);                            \
        };                                                                                         \
                                                                                                   \
        constexpr bool is_field = requires {                                                       \
            std::forward<T>(value).MEMBER;                                                         \
        };                                                                                         \
                                                                                                   \
        if constexpr(is_method) {                                                                  \
            return std::forward<T>(value).MEMBER(std::forward<Args>(args)...);                     \
        } else if constexpr(is_field) {                                                            \
            /* Parenthesis around needed for `decltype(auto)` */                                   \
            return (std::forward<T>(value).MEMBER);                                                \
        } else {                                                                                   \
            static_assert(                                                                         \
                false,                                                                             \
                "Must be unreachable (`" #MEMBER "`)"                                              \
            );                                                                                     \
        }                                                                                          \
    }
// clang-format on
