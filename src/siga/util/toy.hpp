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
[[clang::always_inline]] void do_not_optimize(T &&value) {
    // `volatile` prevents the instruction from being deleted
    asm volatile(

        "" // empty asm code

        : "+r,m"(value) // input param, where:
                        // `+` -- input/output operand
                        // `r` -- general-purpose register operand
                        // `m` -- memory operand with arbitrary addressing mode

        : // output param, empty

        : "memory" // clobber list, clobbers memory
    );
}

// ------------------------------------------------------------------------------------------------

template<typename T>
class type_tag_t
{
public:
    using type = T;
};

template<auto V>
class value_tag_t
{
public:
    constexpr static auto value = V;
};

template<typename T>
constexpr type_tag_t<T> make_tag() noexcept {
    return {};
}

template<auto V>
constexpr value_tag_t<V> make_tag() noexcept {
    return {};
}

// ------------------------------------------------------------------------------------------------

template<typename... Ts>
class type_list_t
{};

} // namespace siga::util
