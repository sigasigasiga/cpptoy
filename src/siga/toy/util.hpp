#pragma once

#include <iostream>

namespace siga::toy {

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

} // namespace siga::toy

// clang-format off

#define SIGA_TOY_LIFT(X)                                                       \
    []<typename... Args>(Args &&...args) constexpr                             \
    noexcept(noexcept(X(std::forward<Args>(args)...)))                         \
    -> decltype(auto)                                                          \
    { return X(std::forward<Args>(args)...); }

// clang-format on
