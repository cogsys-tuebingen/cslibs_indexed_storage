#pragma once

#include <cstdint>
#include <tuple>

namespace cslibs_clustering
{
namespace support
{

template<typename... Types>
struct index_wrapper<std::tuple<Types...>>
{
    using type = std::tuple<Types...>;
    static constexpr auto dimensions = sizeof...(Types);

    template<std::size_t I>
    struct get
    {
        static_assert(I < dimensions, "Out of bounds index access");

        using value_type = typename std::tuple_element<I, type>::type;
        inline static constexpr value_type access(const type& index)
        {
            return std::get<I>(index);
        }
    };
};

template<typename Frist, typename Second>
struct index_wrapper<std::pair<Frist, Second>>
{
    using type = std::pair<Frist, Second>;
    static constexpr auto dimensions = 2;

    template<std::size_t I>
    struct get
    {
        static_assert(I < dimensions, "Out of bounds index access");

        using value_type = typename std::tuple_element<I, type>::type;
        inline static constexpr value_type access(const type& index)
        {
            return std::get<I>(index);
        }
    };
};

}
}