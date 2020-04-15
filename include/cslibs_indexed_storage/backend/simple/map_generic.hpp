#pragma once

#include <cslibs_indexed_storage/backend/tags.hpp>
#include <cslibs_indexed_storage/backend/backend_traits.hpp>
#include <cslibs_indexed_storage/interface/data/data_interface.hpp>
#include <cstdint>
#include <type_traits>

namespace cslibs_indexed_storage { namespace backend { namespace simple
{

template<typename map_t, typename data_interface_t_, typename index_if_, typename... options_ts_>
class MapGeneric
{
public:
    using tag = map_tag;

    using data_if = data_interface_t_;
    using data_output_t = typename data_if::output_type;
    using index_if = index_if_;
    using index_t = typename index_if::type;

    static constexpr auto on_duplicate_index_strategy = option::get_option<option::merge_strategy_opt, options_ts_...>::value;

public:
    template<typename... Args>
    inline data_output_t& insert(const index_t& index, Args&&... args)
    {
        auto itr = storage_.find(index);
        if (itr == storage_.end())
        {
            auto result = storage_.emplace(index, data_if::create(std::forward<Args>(args)...));
            return data_if::expose(result.first->second);
        }
        else
        {
            auto& value = itr->second;
            data_if::template merge<on_duplicate_index_strategy>(value, std::forward<Args>(args)...);
            return data_if::expose(value);
        }
    }

    inline data_output_t* get(const index_t& index)
    {
        auto itr = storage_.find(index);
        if (itr == storage_.end())
            return nullptr;
        return &data_if::expose(itr->second);
    }

    inline const data_output_t* get(const index_t& index) const
    {
        auto itr = storage_.find(index);
        if (itr == storage_.end())
            return nullptr;
        return &data_if::expose(itr->second);
    }

    template<typename Fn>
    inline void traverse(const Fn& function)
    {
        for (auto& entry : storage_)
            function(entry.first, data_if::expose(entry.second));
    }

    template<typename Fn>
    inline void traverse(const Fn& function) const
    {
        for (auto& entry : storage_)
            function(entry.first, data_if::expose(entry.second));
    }

    inline void clear()
    {
        storage_.clear();
    }

    virtual inline std::size_t byte_size() const
    {
        return 0;
    }

    inline std::size_t size() const
    {
        return storage_.size();
    }

protected:
    map_t storage_;
};

}}}

namespace cslibs_indexed_storage { namespace backend
{

template<>
struct backend_traits<map_tag>
{
    static constexpr bool IsFixedSize = false;
};

}}
