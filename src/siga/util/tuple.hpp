#pragma once

#include "siga/util/functional.hpp"
#include "siga/util/meta.hpp"

namespace siga::util {

// ------------------------------------------------------------------------------------------------

class for_each_in_tuple_t
{
public:
    // TODO: noexcept
    template<typename F, tuple_like Tuple>
    static constexpr F operator()(F func, Tuple &&tuple) {
        std::apply(std::bind_front(for_each_in_parameter_pack, std::ref(func)), tuple);
        return func;
    }
};

inline constexpr for_each_in_tuple_t for_each_in_tuple;

} // namespace siga::util
