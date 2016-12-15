#pragma once

#include <cslibs_clustering/index/index.hpp>

#include <cstdint>
#include <array>

namespace cslibs_clustering
{
namespace support
{

template<typename T, std::size_t N>
struct index_wrapper<std::array<T, N>>
{
    using type = std::array<T, N>;
    static constexpr auto dimensions = N;

    template<std::size_t I>
    struct get
    {
        static_assert(I < dimensions, "out of bounds index access");

        using value_type = T;
        inline static constexpr value_type& access(type& index)
        {
            return index[I];
        }
        inline static constexpr const value_type& access(const type& index)
        {
            return index[I];
        }
    };

    template<typename other_t>
    static type add(const type& a, const other_t& b)
    {
        static_assert(index_wrapper<other_t>::dimensions == dimensions, "dimensions not equal");

        type r;
        for (std::size_t i = 0; i < dimensions; ++i)
            r[i] = a[i] + b[i];
        return r;
    }
};

}
}
