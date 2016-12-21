#pragma once

#include <cslibs_indexed_storage/backend/simple/map_generic.hpp>
#include <unordered_map>

namespace cslibs_indexed_storage
{
namespace backend
{
namespace simple
{

template<typename data_interface_t_, typename index_interface_t_, typename... options_ts_>
class UnorderedMap : public MapGeneric<
        std::unordered_map<typename index_interface_t_::type, typename data_interface_t_::storage_type>,
        data_interface_t_, index_interface_t_, options_ts_...>
{
};

}
}}
