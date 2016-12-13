#pragma once

#include <cslibs_clustering/helper/parameter.hpp>

namespace cslibs_clustering { namespace backend {
namespace options {

namespace tags
{
struct split_index_type {};
}

template<typename split_index_type_>
struct split_value_type
{
    using tag = tags::split_index_type;
    using type = split_index_type_;
};

}
}}