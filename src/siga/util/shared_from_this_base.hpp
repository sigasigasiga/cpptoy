#pragma once

namespace siga::util {

// `std::enable_shared_from_this` requires its derived classes (let's call one `A`)
// to have some sort of static method that would construct an instance of the class.
//
// this approach has some problems:
// 1. boilerplate
// 2. it doesn't work great if we want to derive from `A`
// 3. it makes `std::make_shared` usage tricky
// 4. if we want to inherit from multiple classes that are derived from esft,
//    everything becomes a huge mess, and there are no ways to fix that
//    because it is impossible to inherit from it virtually
//
// `shared_from_this_base` solves these problems:
// 1. the only thing that sftb-derived classes need to do
//    is to put `access_tag` as the first constructor argument and init the base class with it
// 2. classes that are derived from `A` also need to put `access_tag` as the first constructor arg,
//    and then they should forward it to the derived class
// 3. `util::make_shared` becomes the only way to construct it and it is optimal
// 4. virtual inheritance from sftb is easy
class shared_from_this_base;

template<typename T>
concept sftb_makeable = std::convertible_to<T *, const volatile shared_from_this_base *>;

class shared_from_this_base
{
public:
    template<sftb_makeable T>
    class make_shared_impl;

    class access_tag
    {
    private:
        constexpr explicit access_tag() = default;

        template<sftb_makeable T>
        friend class make_shared_impl;
    };

protected:
    explicit shared_from_this_base(access_tag) noexcept {}

public:
    [[nodiscard]] auto shared_from_this(this auto &&self) { return self.make_sft(); }
    [[nodiscard]] auto weak_from_this(this auto &&self) { return self.make_wft(); }

private:
    template<typename Self>
    std::shared_ptr<Self> make_sft(this Self &self)
    {
        auto ret = std::static_pointer_cast<Self>(self.weak_this_.lock());
        assert(ret && "how have you constructed such object?");
        return ret;
    }

    template<typename Self>
    std::weak_ptr<Self> make_wft(this Self &self)
    {
        return self.make_sft();
    }

private:
    mutable std::weak_ptr<void> weak_this_;
};

template<sftb_makeable T>
class shared_from_this_base::make_shared_impl
{
private:
    using access_tag = shared_from_this_base::access_tag;

public:
    template<typename... Args>
    requires std::constructible_from<T, access_tag, Args &&...>
    [[nodiscard]] static std::shared_ptr<T> operator()(Args &&...args)
    {
        auto s = std::make_shared<T>(access_tag{}, std::forward<Args>(args)...);
        s->weak_this_ = s;
        return s;
    }
};

template<typename T>
class universal_make_shared
{
public:
    template<typename... Args>
    requires std::constructible_from<T, Args &&...>
    [[nodiscard]] static auto operator()(Args &&...args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
};

template<sftb_makeable T>
class universal_make_shared<T> : public shared_from_this_base::make_shared_impl<T>
{};

template<typename T>
inline constexpr universal_make_shared<T> make_shared;

} // namespace siga::util
