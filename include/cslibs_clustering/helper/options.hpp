#pragma once

#include <tuple>
#include <type_traits>

namespace cslibs_clustering { namespace option
{
namespace detail
{
// -------------------------------------------------------------------------------------
template<typename option>
using get_tag = typename option::tag;
template<typename option>
using get_type = typename option::type;
// -------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------
template<typename tag, typename... options>
struct tag_checker;

template<typename tag, typename first, typename... options>
struct tag_checker<tag, first, options...> :
        std::integral_constant<bool,
                std::is_same<tag, get_tag<first>>::value || tag_checker<tag, options...>::value
        >
{};

template<typename tag>
struct tag_checker<tag> : std::false_type {};
// -------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------
template<typename tag, typename default_type, typename... options>
struct tag_extractor;

template<typename tag, typename default_type, typename first, typename... options>
struct tag_extractor<tag, default_type, first, options...>
{
    using type = typename std::conditional<
            std::is_same<tag, get_tag<first>>::value,
            get_type<first>,
            typename tag_extractor<tag, default_type, options...>::type
    >::type;
};

template<typename tag, typename default_type>
struct tag_extractor<tag, default_type>
{
    using type = default_type;
};
// -------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------
template<typename tag, typename default_type, typename... options>
struct type_extractor
{
    static constexpr bool exists = tag_checker<tag, options...>::value;
    using type = typename tag_extractor<tag, default_type, options...>::type;
};

template<typename T, T first, T... values>
struct value_holder
{
    static constexpr auto size = 1 + sizeof...(values);
    using storage = typename std::conditional<size == 1, T, std::array<T, size>>::type;

    static constexpr storage value{first, values...};
};
// -------------------------------------------------------------------------------------
}

template<typename tag_, typename type_>
struct define_type_option
{
    using tag = tag_;
    using type = type_;
};

template<typename tag_, typename T, T... values>
struct define_value_option
{
    using tag = tag_;
    using type = detail::value_holder<T, values...>;
};

template<typename tag, typename default_type>
using define_type_extractor = std::tuple<tag, default_type>;

template<typename tag, typename T, T... values>
using define_value_extractor = std::tuple<tag, detail::value_holder<T, values...>>;

template<typename tag, typename default_type, typename... options>
using get_option_extractor_raw = detail::type_extractor<tag, default_type, options...>;

template<typename tag, typename default_type, typename... options>
using get_option_raw = typename get_option_extractor_raw<tag, default_type, options...>::type;

template<typename def, typename... options>
using get_option = typename get_option_extractor_raw<typename std::tuple_element<0, def>::type, typename std::tuple_element<1, def>::type, options...>::type;



}
}
