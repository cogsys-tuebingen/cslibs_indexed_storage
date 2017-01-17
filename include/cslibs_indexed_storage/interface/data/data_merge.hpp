#pragma once

#include <cslibs_indexed_storage/helper/void_t.hpp>
#include <utility>

namespace cslibs_indexed_storage
{

namespace detail
{
template<typename>
using sfinae_true = std::true_type;

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
using has_variadic_merge_t = typename std::integral_constant<bool, has_variadic_merge<T, Args...>::value>::type;


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

/**
 * Applies merge operation on an existing data entry
 *
 * The merge partner can be either in the form of (selected in this priority):
 * 1. Variadic arguments (generic)
 * 2. Variadic arguments (for construction)
 * 3. Actual data entry
 *
 * The actual operation can be defines with two methods:
 * 1. Free function (in <code>cslibs_indexed_storage</code> namespace)
 * 2. Member function of data entry
 *
 * This overload dispatches ADL calls to the member function if available.
 *
 * @tparam T data entry type
 * @tparam Args merge argument types
 * @param self existing data entry
 * @param args merge argument values
 */
template<typename T, typename... Args>
inline constexpr auto merge(T& self, Args&&... args) -> typename std::enable_if<detail::has_variadic_merge<T, Args...>::value || detail::has_member_merge<T>::value>::type
{
    detail::merge_dispatch(detail::has_variadic_merge_t<T, Args...>{}, self, std::forward<Args>(args)...);
}

}
