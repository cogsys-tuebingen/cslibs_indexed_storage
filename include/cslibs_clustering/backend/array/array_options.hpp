#pragma once

#include <cslibs_clustering/helper/parameter.hpp>

namespace cslibs_clustering { namespace backend {
namespace options {

namespace tags
{
struct array_size {};
struct array_offset {};
struct array_dynamic_only{};
}

template<std::size_t... sizes>
struct array_size
{
    using tag = tags::array_size;

    struct ValueHolder
    {
        static constexpr std::array<std::size_t, sizeof...(sizes)> value = {sizes...};
    };
    using type = ValueHolder;
};

//! \todo: make this independent of index_value_t (maybe wrap in constexpr std::tuple member)
template<typename index_value_t, index_value_t... values_>
struct array_offset
{
    using tag = tags::array_offset;
    struct ValueHolder
    {
        static constexpr std::array<index_value_t, sizeof...(values_)> value = {values_...};
    };
    using type = ValueHolder;
};

template<bool dynamic_only = true>
struct array_dynamic_only
{
    using tag = tags::array_offset;
    using type = std::integral_constant<bool, dynamic_only>;
};

}
}}