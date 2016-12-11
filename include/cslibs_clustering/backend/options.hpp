#pragma once

#include <cslibs_clustering/helper/void_t.hpp>
#include <cslibs_clustering/helper/parameter.hpp>

namespace cslibs_clustering { namespace backend {
namespace options {

namespace tags
{
struct on_duplicate_index {};
}

enum class OnDuplicateIndex { MERGE, REPLACE };

template<OnDuplicateIndex method>
struct on_duplicate_index
{
    using tag = tags::on_duplicate_index;
    using type = std::integral_constant<OnDuplicateIndex, method>;
};

template<typename T, typename = void>
struct is_on_duplicate_index_merge_available : std::false_type {};
template<typename T>
struct is_on_duplicate_index_merge_available<T, helper::void_t<decltype(std::declval<T>().merge(std::declval<T>()))>> : std::true_type {};

}
}}