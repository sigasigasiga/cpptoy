#pragma once

#include <iostream>

namespace siga::util {

template<typename>
struct type_print_t;

template<typename T>
type_print_t<T> print_type(const T &);

template<auto>
struct value_print_t;

// ------------------------------------------------------------------------------------------------

class printing_constructor_t
{
public:
    printing_constructor_t() { std::cout << "printing_constructor_t()" << std::endl; }

    printing_constructor_t(const printing_constructor_t &) {
        std::cout << "printing_constructor_t(const printing_constructor_t &)" << std::endl;
    }

    printing_constructor_t(printing_constructor_t &&) {
        std::cout << "printing_constructor_t(printing_constructor_t &&)" << std::endl;
    }

    printing_constructor_t &operator=(const printing_constructor_t &) {
        std::cout << "printing_constructor_t &operator=(const printing_constructor_t &)"
                  << std::endl;

        return *this;
    }

    printing_constructor_t &operator=(printing_constructor_t &&) {
        std::cout << "printing_constructor_t &operator=(printing_constructor_t &&)" << std::endl;

        return *this;
    }

    ~printing_constructor_t() { std::cout << "~printing_constructor_t()" << std::endl; }
};

// ------------------------------------------------------------------------------------------------

class ignore_t
{
public:
    constexpr const ignore_t &operator=(auto &&) const noexcept { return *this; }
    constexpr void operator()(auto &&...) const noexcept {}
};

inline constexpr ignore_t ignore;

// ------------------------------------------------------------------------------------------------

template<typename T>
void do_not_optimize(T &&value) {
    asm volatile("" // empty command

                 : "+r,m"(value) // input param, where:
                                 // `+` -- input/output operand
                                 // `r` -- general-purpose register operand
                                 // `m` -- memory operand with arbitrary addressing mode

                 : // output param, empty

                 : "memory" // clobbers memory
    );
}

} // namespace siga::util
