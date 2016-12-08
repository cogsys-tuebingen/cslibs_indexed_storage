#pragma once

#include <cstdint>
#include <array>

namespace cslibs_clustering
{
namespace support
{

template<typename T>
struct index_wrapper
{
    using type = T;
    using raw_type = T;
};

template<typename T, std::size_t N>
struct index_wrapper<std::array<T, N>>
{
    struct ArrayIndex
    {
        using value_type = T;
        static constexpr auto dimensions = N;
    };
    using type = ArrayIndex;
    using raw_type = std::array<T, N>;
};

}
}
