#pragma once

#include <cslibs_clustering/helper/options.hpp>

namespace cslibs_clustering {
namespace option {

namespace tags
{
struct array_size {};
struct array_offset {};
}

template<std::size_t... sizes>
struct array_size : define_value_option<tags::array_size, std::size_t, sizes...> {};

template<typename offset_t, offset_t... offsets>
struct array_offset : define_value_option<tags::array_offset, offset_t, offsets...> {};


}
}