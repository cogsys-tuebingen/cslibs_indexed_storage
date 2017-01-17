#pragma once

#include <cslibs_indexed_storage/backend/kdtree/kdtree_with_allocator.hpp>
#include <cslibs_indexed_storage/backend/kdtree/allocator/ondemand_allocator.hpp>

namespace cslibs_indexed_storage
{
namespace backend
{
namespace kdtree
{

template<typename data_interface_t_, typename index_interface_t_, typename... options_ts_>
using KDTree = KDTreeWithAllocator<detail::OnDemandAllocator, data_interface_t_, index_interface_t_, options_ts_...>;

}
}
}