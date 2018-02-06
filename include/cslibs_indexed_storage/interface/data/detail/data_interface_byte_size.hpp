#pragma once

#include <cstddef>
#include <type_traits>

namespace cslibs_indexed_storage
{
namespace interface
{
namespace detail
{

template<typename T>
inline constexpr std::size_t byte_size_impl(const T& value, std::true_type)
{
    return sizeof(T);
}
template<typename T>
inline constexpr std::size_t byte_size_impl(const T& value, std::false_type)
{
    return value.byte_size();
}

template<typename T>
inline constexpr std::size_t byte_size(const T& value)
{
    return byte_size_impl(value, std::is_pod<T>{});
}

}
}
}
