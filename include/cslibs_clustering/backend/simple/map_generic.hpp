#pragma once

#include <cslibs_clustering/data/data.hpp>
#include <cstdint>
#include <type_traits>

namespace cslibs_clustering
{
namespace backend
{
namespace simple
{

template<typename map_t, typename data_t_, typename index_wrapper_t_, typename... options_ts_>
class MapGeneric
{
public:
    using data_t = data_t_;
    using index_wrapper_t = index_wrapper_t_;
    using index_t = typename index_wrapper_t::type;

    using on_duplicate_index_opt = helper::get_option_t<
            options::tags::on_duplicate_index,
            options::on_duplicate_index<options::OnDuplicateIndex::REPLACE>,
            options_ts_...>;
    static constexpr auto on_duplicate_index_strategy = on_duplicate_index_opt::value;

public:
    template<typename... Args>
    inline data_t& insert(const index_t& index, Args&&... args)
    {
        auto itr = storage_.find(index);
        if (itr == storage_.end())
        {
            auto result = storage_.emplace(index, data_ops<data_t>::create(std::forward<Args>(args)...));
            return result.first->second;
        }
        else
        {
            auto& value = itr->second;
            data_ops<data_t>::template merge<on_duplicate_index_strategy>(value, std::forward<Args>(args)...);
            return value;
        }
    }

    inline data_t* get(const index_t& index)
    {
        auto itr = storage_.find(index);
        if (itr == storage_.end())
            return nullptr;
        return itr.second;
    }

    inline const data_t* get(const index_t& index) const
    {
        auto itr = storage_.find(index);
        if (itr == storage_.end())
            return nullptr;
        return itr.second;
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
