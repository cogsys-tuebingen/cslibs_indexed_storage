#pragma once

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
        static_assert(I < dimensions, "Out of bounds index access");

        using value_type = T;
        inline static constexpr value_type access(const type& index)
        {
            return index[I];
        }
    };

    template<typename other_t>
    static type add(const type& a, const other_t& b)
    {
        type r;
        for (std::size_t i = 0; i < dimensions; ++i)
            r[i] = a[i] + b[i];
        return r;
    }
};

}
}
