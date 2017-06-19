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
};

}
}}
