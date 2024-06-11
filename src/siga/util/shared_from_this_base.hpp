#pragma once

namespace siga::util {

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
    template<typename Self>
    std::shared_ptr<Self> shared_from_this(this Self &self)
    {
        auto ret = std::static_pointer_cast<Self>(self.weak_this_.lock());
        assert(ret && "how have you constructed such object?");
        return ret;
    }

    template<typename Self>
    std::weak_ptr<Self> weak_from_this(this Self &self) noexcept
    {
        return std::static_pointer_cast<Self>(self.weak_this_.lock());
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
    static std::shared_ptr<T> operator()(Args &&...args)
    {
        auto s = std::make_shared<T>(access_tag{}, std::forward<Args>(args)...);
        s->weak_this_ = s;
        return s;
    }
};

template<sftb_makeable T>
inline constexpr shared_from_this_base::make_shared_impl<T> make_shared;

} // namespace siga::util
