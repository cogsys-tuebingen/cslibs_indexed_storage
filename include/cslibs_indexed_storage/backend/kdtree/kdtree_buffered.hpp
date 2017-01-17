#pragma once

#include <cslibs_indexed_storage/backend/kdtree/kdtree_with_allocator.hpp>
#include <cslibs_indexed_storage/backend/kdtree/allocator/buffered_allocator.hpp>

namespace cslibs_indexed_storage
{
namespace backend
{
namespace kdtree
{

template<typename data_interface_t_, typename index_interface_t_, typename... options_ts_>
class KDTreeBuffered : public KDTreeWithAllocator<detail::BufferedAllocator, data_interface_t_, index_interface_t_, options_ts_...>
{
    using base_type = KDTreeWithAllocator<detail::BufferedAllocator, data_interface_t_, index_interface_t_, options_ts_...>;

public:
    void set(option::tags::node_allocator_chunk_size, std::size_t chunk_size)
    {
        this->set_node_allocator(typename base_type::node_allocator_t(chunk_size));
    }
};

}
}
}