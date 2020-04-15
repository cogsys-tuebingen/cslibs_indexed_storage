#pragma once

#include <cslibs_indexed_storage/backend/simple/map_generic.hpp>
#include <cslibs_indexed_storage/interface/data/align/aligned_allocator.hpp>
#include <map>

namespace cslibs_indexed_storage
{
namespace backend
{
namespace simple
{

template<typename data_interface_t_, typename index_interface_t_, typename... options_ts_>
class Map : public MapGeneric<
        std::map<typename index_interface_t_::type,
                 typename data_interface_t_::storage_type,
                 std::less<typename index_interface_t_::type>,
                 interface::aligned_allocator<std::pair<const typename index_interface_t_::type, typename data_interface_t_::storage_type>>>,
        data_interface_t_, index_interface_t_, options_ts_...>
{
public:
    virtual inline std::size_t byte_size() const override
    {
        // real content size
        std::size_t content_bytes = 0;
        for (auto& entry : this->storage_)
            content_bytes += data_interface_t_::byte_size(entry.second);

        using key_type = typename index_interface_t_::type;
        using value_type = typename data_interface_t_::storage_type;
        using pair_type = std::pair<const key_type, value_type>;
        using node_type = std::_Rb_tree_node<pair_type>;

        // map structure:
        // each entry is a RB tree node
        // 'value_type' needs to be subtracted because it is the base type of the contents
        // and the real size of the contents are counted in 'content_bytes'
        return sizeof(*this) + content_bytes +
               this->storage_.size() * (sizeof(node_type) - sizeof(value_type));
    }
};

}
}}
