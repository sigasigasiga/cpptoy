#pragma once

namespace siga::util {

// `util/utility.hpp` sounds funny, but I was trying to somehow resemble the `<utility>` STL header

class [[nodiscard]] cut_rvalue_ref_t
{
public:
    template<typename T>
    [[nodiscard]] static constexpr decltype(auto) operator()(T &&v) noexcept
    {
        // cut_rvalue_ref(0) -> cut_rvalue_ref<int>(int &&);
        // cut_rvalue_ref(x) -> cut_rvalue_ref<int &>(int &);
        //
        // so if the passed value is lvalue, we return the lvalue,
        // and if the passed value is rvalue, we return the value
        return static_cast<T>(std::forward<T>(v));
    }
};

inline constexpr cut_rvalue_ref_t cut_rvalue_ref;

} // namespace siga::util
