#pragma once

#include <cslibs_clustering/helper/void_t.hpp>
#include <utility>

namespace cslibs_clustering
{

namespace detail
{
template<typename>
struct sfinae_true : std::true_type{};

template<typename T, typename... Args>
struct has_variadic_merge
{
private:
    template<typename TT, typename... Argss>
    static constexpr auto check(int) -> sfinae_true<decltype(std::declval<TT>().merge(std::declval<Argss>()...))>;
    template<typename, typename...>
    static constexpr std::false_type check(...);

    using type = decltype(check<T, Args...>(0));

public:
    static constexpr bool value = type::value;
};

template<typename T>
struct has_member_merge
{
private:
    template<typename TT>
    static constexpr auto check(int) -> sfinae_true<decltype(std::declval<TT>().merge(std::declval<TT>()))>;
    template<typename>
    static constexpr std::false_type check(...);

    using type = decltype(check<T>(0));

public:
    static constexpr bool value = type::value;
};

template<typename T, typename... Args>
using has_variadic_merge_t = std::integral_constant<bool, has_variadic_merge<T, Args...>::value>;


template<typename T, typename... Args>
inline constexpr void merge_dispatch(std::true_type, T& self, Args&&... args)
{
    self.merge(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
inline constexpr void merge_dispatch(std::false_type, T& self, Args&&... args)
{
    self.merge(T{std::forward<Args>(args)...});
}
}

template<typename T, typename... Args>
inline constexpr auto merge(T& self, Args&&... args) -> typename std::enable_if<detail::has_variadic_merge<T, Args...>::value || detail::has_member_merge<T>::value>::type
{
    detail::merge_dispatch(detail::has_variadic_merge_t<T, Args...>{}, self, std::forward<Args>(args)...);
}

}
