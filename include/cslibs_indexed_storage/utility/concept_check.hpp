#pragma once

#include <cstddef>

namespace cslibs_indexed_storage { namespace utility
{

template<std::size_t I> struct feature_priority_tag : feature_priority_tag<I - 1> {};
template<> struct feature_priority_tag<0> {};

using feature_missing_tag = feature_priority_tag<0>;
using feature_exists_tag =  feature_priority_tag<1>;

using check_feature_exists = feature_exists_tag;

}}
