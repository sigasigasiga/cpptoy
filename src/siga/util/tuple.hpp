#pragma once

#include "siga/util/functional.hpp"
#include "siga/util/meta.hpp"

namespace siga::util {

// ------------------------------------------------------------------------------------------------

class [[nodiscard]] for_each_in_tuple_t
{
public:
    // see `noexcept` policy for `for_each_in_parameter_pack`
    template<typename F, tuple_like Tuple>
    static constexpr F operator()(F func, Tuple &&tuple)
    {
        return std::apply(
            std::bind_front(for_each_in_parameter_pack, std::move(func)),
            std::forward<Tuple>(tuple)
        );
    }
};

inline constexpr for_each_in_tuple_t for_each_in_tuple;

} // namespace siga::util
