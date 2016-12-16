#pragma once

#include <cslibs_clustering/interface/data/data.hpp>
#include <cstdint>
#include <type_traits>

namespace cslibs_clustering
{
namespace backend
{
namespace simple
{

template<typename map_t, typename data_interface_t_, typename index_if_, typename... options_ts_>
class MapGeneric
{
public:
    using data_if = data_interface_t_;
    using data_t = typename data_if::type;
    using index_if = index_if_;
    using index_t = typename index_if::type;

    static constexpr auto on_duplicate_index_strategy = option::get_option<option::merge_strategy_opt, options_ts_...>::value;

public:
    template<typename... Args>
    inline data_t& insert(const index_t& index, Args&&... args)
    {
        auto itr = storage_.find(index);
        if (itr == storage_.end())
        {
            auto result = storage_.emplace(index, data_if::create(std::forward<Args>(args)...));
            return result.first->second;
        }
        else
        {
            auto& value = itr->second;
            data_if::template merge<on_duplicate_index_strategy>(value, std::forward<Args>(args)...);
            return value;
        }
    }

    inline data_t* get(const index_t& index)
    {
        auto itr = storage_.find(index);
        if (itr == storage_.end())
            return nullptr;
        return &(itr->second);
    }

    inline const data_t* get(const index_t& index) const
    {
        auto itr = storage_.find(index);
        if (itr == storage_.end())
            return nullptr;
        return &(itr->second);
    }

    template<typename Fn>
    inline void traverse(const Fn& function)
    {
        for (auto& entry : storage_)
            function(entry.first, entry.second);
    }

    template<typename Fn>
    inline void traverse(const Fn& function) const
    {
        for (auto& entry : storage_)
            function(entry.first, entry.second);
    }

private:
    map_t storage_;
};

}
}}
