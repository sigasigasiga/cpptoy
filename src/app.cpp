#include "main.hpp"

namespace siga {

namespace {

template<auto MainFPtr>
constexpr auto adjust_args(int argc, const char *argv[]) {
    if constexpr(requires { std::invoke(MainFPtr, argc, argv); }) {
        return std::invoke(MainFPtr, argc, argv);
    } else if constexpr(requires { std::invoke(MainFPtr); }) {
        return std::invoke(MainFPtr);
    } else {
        static_assert(false);
    }
}

template<auto MainFPtr>
constexpr int adjust(int argc, const char *argv[]) {
    constexpr bool returns_void = requires() {
        { adjust_args<MainFPtr>(argc, argv) } -> util::conceptify<std::is_void>;
    };

    if constexpr(returns_void) {
        adjust_args<MainFPtr>(argc, argv);
        return 0;
    } else {
        return adjust_args<MainFPtr>(argc, argv);
    }
}

} // anonymous namespace

} // namespace siga

int main(int argc, const char *argv[]) {
    return siga::adjust<&siga::main>(argc, argv);
}
