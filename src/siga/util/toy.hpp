#pragma once

#include <iostream>

namespace siga::util {

template<typename>
struct type_print_t;

template<typename T>
type_print_t<T> print_type(const T &);

template<auto>
struct value_print_t;

class printing_constructor_t
{
public:
    printing_constructor_t() {
        std::cout << "printing_constructor_t()" << std::endl;
    }

    printing_constructor_t(const printing_constructor_t &) {
        std::cout << "printing_constructor_t(const printing_constructor_t &)"
                  << std::endl;
    }

    printing_constructor_t(printing_constructor_t &&) {
        std::cout << "printing_constructor_t(printing_constructor_t &&)"
                  << std::endl;
    }

    printing_constructor_t &operator=(const printing_constructor_t &) {
        std::cout
            << "printing_constructor_t &operator=(const printing_constructor_t &)"
            << std::endl;

        return *this;
    }

    printing_constructor_t &operator=(printing_constructor_t &&) {
        std::cout
            << "printing_constructor_t &operator=(printing_constructor_t &&)"
            << std::endl;

        return *this;
    }

    ~printing_constructor_t() {
        std::cout << "~printing_constructor_t()" << std::endl;
    }
};

class ignore_t
{
public:
    constexpr ignore_t &operator=(auto &&) noexcept { return *this; }
    constexpr void operator()(auto &&...) noexcept {}
};

constexpr inline ignore_t ignore;

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
