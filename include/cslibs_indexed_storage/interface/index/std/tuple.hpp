#pragma once

#include <cslibs_indexed_storage/interface/index/index_interface.hpp>
#include <cstdint>
#include <tuple>

namespace cslibs_indexed_storage
{
namespace interface
{

template<typename... Types>
struct index_interface<std::tuple<Types...>>
{
    using type = std::tuple<Types...>;
    static constexpr auto dimensions = sizeof...(Types);

    template<std::size_t I>
    struct dimension
    {
        static_assert(I < dimensions, "Out of bounds index access");
        using value_type = typename std::tuple_element<I, type>::type;

        static inline constexpr value_type& access(type& index)
        {
            return std::get<I>(index);
        };
        static inline constexpr const value_type& access(const type& index)
        {
            return std::get<I>(index);
        };
    };

    template<typename other_t>
    static inline constexpr type add(const type& a, const other_t& b)
    {
        using other_if = index_interface<other_t>;
        static_assert(other_if::dimensions == dimensions, "dimensions not equal");
        return add_helper(a, b, utility::make_index_sequence<dimensions>{});
    }

private:
    template<typename other_t, std::size_t... i>
    static inline constexpr type add_helper(const type& a, const other_t& b, utility::index_sequence<i...>)
    {
        return std::make_tuple(((void)i, static_cast<typename dimension<i>::value_type>(dimension<i>::access(a) + index_interface<other_t>::template dimension<i>::access(b)))...);
    }
};

template<typename Frist, typename Second>
struct index_interface<std::pair<Frist, Second>>
{
    using type = std::pair<Frist, Second>;
    static constexpr auto dimensions = 2;

    template<std::size_t I>
    struct dimension
    {
        static_assert(I < dimensions, "Out of bounds index access");
        using value_type = typename std::tuple_element<I, type>::type;

        static inline constexpr value_type& access(type& index)
        {
            return std::get<I>(index);
        };
        static inline constexpr const value_type& access(const type& index)
        {
            return std::get<I>(index);
        };
    };

    template<typename other_t>
    static inline constexpr type add(const type& a, const other_t& b)
    {
        using other_if = index_interface<other_t>;
        static_assert(other_if::dimensions == dimensions, "dimensions not equal");
        return std::make_pair(dimension<0>::access(a) + other_if::template dimension<0>::access(b),
                              dimension<1>::access(a) + other_if::template dimension<1>::access(b));
    }
};

}
}
