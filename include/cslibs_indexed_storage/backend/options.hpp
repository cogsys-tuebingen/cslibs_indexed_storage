#pragma once

#include <cslibs_indexed_storage/helper/void_t.hpp>
#include <cslibs_indexed_storage/helper/options.hpp>

namespace cslibs_indexed_storage {
namespace option {

namespace tags
{
struct on_duplicate_index {};
}

enum class MergeStrategy { MERGE, REPLACE };

template<MergeStrategy strategy>
struct merge_strategy : define_value_option<tags::on_duplicate_index, MergeStrategy, strategy> {};

using merge_strategy_opt = define_value_extractor<tags::on_duplicate_index, MergeStrategy, MergeStrategy::MERGE>;
}
}