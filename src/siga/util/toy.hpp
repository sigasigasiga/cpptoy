#pragma once

#include <iostream>

namespace siga::util {

template<typename...>
struct type_print;

template<typename T>
type_print<T> print_type(const T &);

template<auto...>
struct value_print;

// ------------------------------------------------------------------------------------------------

class printing_constructor
{
public:
    printing_constructor() { std::cout << "printing_constructor(): " << this << std::endl; }

    printing_constructor(const printing_constructor &rhs)
    {
        // clang-format off
        std::cout
            << "printing_constructor(const printing_constructor &); "
            << "lhs: " << this << ", "
            << "rhs: " << &rhs
            << std::endl;
        // clang-format on
    }

    printing_constructor(printing_constructor &&rhs)
    {
        // clang-format off
        std::cout
            << "printing_constructor(printing_constructor &&); "
            << "lhs: " << this << ", "
            << "rhs: " << &rhs
            << std::endl;
        // clang-format on
    }

    printing_constructor &operator=(const printing_constructor &rhs)
    {
        // clang-format off
        std::cout
            << "printing_constructor &operator=(const printing_constructor &); " 
            << "lhs: " << this << ", "
            << "rhs: " << &rhs
            << std::endl;
        // clang-format on

        return *this;
    }

    printing_constructor &operator=(printing_constructor &&rhs)
    {
        // clang-format off
        std::cout
            << "printing_constructor &operator=(printing_constructor &&); " 
            << "lhs: " << this << ", "
            << "rhs: " << &rhs
            << std::endl;
        // clang-format on

        return *this;
    }

    ~printing_constructor() { std::cout << "~printing_constructor(): " << this << std::endl; }
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
[[clang::always_inline]] void do_not_optimize(T &&value)
{
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
class [[nodiscard]] type_tag
{
public:
    using type = T;
};

template<auto V>
class [[nodiscard]] value_tag
{
public:
    constexpr static auto value = V;
};

template<typename T>
constexpr type_tag<T> make_tag() noexcept
{
    return {};
}

template<auto V>
constexpr value_tag<V> make_tag() noexcept
{
    return {};
}

} // namespace siga::util
