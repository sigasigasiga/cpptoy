#pragma once

namespace siga::util {

// idea from Ed Catmur. usage example: `conceptify<std::is_enum>`
template<typename T, template<typename...> typename Trait>
concept conceptify = Trait<T>::value;

// ------------------------------------------------------------------------------------------------

template<typename T, template<typename...> typename Trait, template<typename...> typename... Rest>
class combine
{
public:
    using type = combine<typename Trait<T>::type, Rest...>::type;
};

template<typename T, template<typename...> typename Trait>
class combine<T, Trait>
{
public:
    using type = Trait<T>::type;
};

template<typename T, template<typename...> typename... Traits>
using combine_t = combine<T, Traits...>::type;

#if 0
static_assert(std::same_as<combine_t<const int &, std::remove_reference, std::remove_cv>, int>);
#endif

// ------------------------------------------------------------------------------------------------

template<typename From, typename To>
class copy_const
{
public:
    using type = To;
};

template<typename From, typename To>
class copy_const<const From, To>
{
public:
    using type = const To;
};

template<typename From, typename To>
using copy_const_t = copy_const<From, To>::type;

// ------------------------------------------------------------------------------------------------

template<typename From, typename To>
class copy_volatile
{
public:
    using type = To;
};

template<typename From, typename To>
class copy_volatile<volatile From, To>
{
public:
    using type = volatile To;
};

template<typename From, typename To>
using copy_volatile_t = copy_volatile<From, To>::type;

} // namespace siga::util
