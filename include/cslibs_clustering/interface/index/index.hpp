#pragma once

#include <cstdint>
#include <tuple>
#include <cslibs_clustering/helper/index_sequence.hpp>

namespace cslibs_clustering
{
namespace interface
{
//! \todo exend by comparison interface (currently needed < and != for map/kdtree support)

template<typename T> struct index_interface;

template<typename T>
struct index_interface_common
{
private:
    using type = T;
    using base = index_interface<T>;

public:

    template<std::size_t I>
    struct dimension
    {
        static_assert(I < base::dimensions, "index out of bounds");

        using value_type = typename std::tuple_element<I, T>::type;

        static inline constexpr value_type& access(type& index)
        {
            return base::access(I, index);
        };
        static inline constexpr const value_type& access(const type& index)
        {
            return base::access(I, index);
        };
    };

    template<typename other_t>
    static inline constexpr type add(const type& a, const other_t& b)
    {
        using other_if = index_interface<other_t>;
        static_assert(other_if::dimensions == base::dimensions, "dimensions not equal");
        return add_helper(a, b, helper::make_index_sequence<base::dimensions>{});

//        type r;
//        for (std::size_t i = 0; i < base::dimensions; ++i)
//            base::access(i, r) = base::access(i, a) + other_if::access(i, b);
//        return r;
    }

private:
    template<typename other_t, std::size_t... i>
    static inline constexpr type add_helper(const type& a, const other_t& b, helper::index_sequence<i...>)
    {
        return {((void)i, static_cast<typename dimension<i>::value_type>(dimension<i>::access(a) + index_interface<other_t>::template dimension<i>::access(b)))...};
    }
};

template<typename T>
struct index_interface : index_interface_common<T>
{
    using type = T;
    static constexpr std::size_t dimensions = std::tuple_size<type>::value;

    static constexpr auto access(std::size_t i, type& index) -> decltype(index[i])
    {
        return index[i];
    }
    static constexpr auto access(std::size_t i, const type& index) -> decltype(index[i])
    {
        return index[i];
    }
};

}
}
