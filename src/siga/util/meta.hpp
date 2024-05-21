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

} // namespace siga::util
