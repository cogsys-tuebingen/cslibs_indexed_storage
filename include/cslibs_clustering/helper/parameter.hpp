#pragma once

#include <tuple>
#include <type_traits>

namespace cslibs_clustering { namespace helper {
namespace detail
{
    struct null_t {};

    template<typename T>
    struct is_integral_constant : std::false_type
    {};

    template<typename T, T value>
    struct is_integral_constant<std::integral_constant<T, value>> : std::true_type
    {};

    template<typename tag_, typename... types_>
    struct contains_tag :
        std::conditional<
                std::is_same<
                        std::tuple<typename std::conditional<std::is_same<tag_, typename types_::tag>::value, null_t, types_>::type...>,
                        std::tuple<types_...>
                >::value,
                std::false_type,
                std::true_type
        >::type
    {};

    template<typename tag, typename first, typename... rest>
    struct extract_arg
    {
        using arg =
        typename std::conditional<
                std::is_same<tag, typename first::tag>::value,
                first,
                typename extract_arg<tag, rest...>::arg
        >::type;
    };

    template<typename tag, typename first>
    struct extract_arg<tag, first>
    {
        using arg = first;
    };

    template<bool contains, typename tag, typename default_, typename... options>
    struct get_option_helper
    {
        using opt_value = typename extract_arg<tag, options...>::arg;
    };

    template<typename tag, typename default_, typename... options>
    struct get_option_helper<false, tag, default_, options...>
    {
        using opt_value = default_;
    };

}

template<typename tag, typename default_, typename... options>
struct get_option
{
    using opt_value_raw =
    typename detail::get_option_helper<
            detail::contains_tag<tag, options...>::value,
            tag, default_, options...
    >::opt_value;

    using opt_value =
    typename std::conditional<
            detail::is_integral_constant<typename opt_value_raw::type>::value,
            typename opt_value_raw::type,
            opt_value_raw
    >::type;
};

template<typename tag, typename default_, typename... options>
using get_option_t = typename get_option<tag, default_, options...>::opt_value;

}
}