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
[[nodiscard]] auto compose(Fs &&...fs)
{
    return std::bind_front(fold_invoke, std::tuple{std::forward<Fs>(fs)...});
}

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
