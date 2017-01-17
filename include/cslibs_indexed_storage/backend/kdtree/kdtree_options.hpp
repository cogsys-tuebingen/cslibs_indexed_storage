#pragma once

#include <cslibs_indexed_storage/helper/options.hpp>

namespace cslibs_indexed_storage {
namespace option
{

namespace tags
{
struct split_index_type {};
struct node_allocator_chunk_size {};
}

template<typename split_index_type_>
struct split_value_type : define_type_option<tags::split_index_type, split_index_type_> {};

using split_value_type_opt = define_type_extractor<tags::split_index_type, float>;
}
}