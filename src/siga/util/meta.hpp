#pragma once

namespace siga::util {

// idea from Ed Catmur. usage example: `conceptify<std::is_enum>`
template<typename T, template<typename...> typename Trait>
concept conceptify = Trait<T>::value;

} // namespace siga::util
