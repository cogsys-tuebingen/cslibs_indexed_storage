#pragma once

#include <cslibs_clustering/helper/static_warning.hpp>
#include <cslibs_clustering/data/data.hpp>
#include <cstdint>
#include <unordered_map>
#include <type_traits>

namespace cslibs_clustering
{
namespace backend
{
namespace simple
{
namespace detail
{
template<typename data_t_, typename index_wrapper_t_, std::size_t own_index, std::size_t last_index, option::MergeStrategy on_duplicate_index_strategy>
class UnorderedComponentMapStorage
{
public:
    using data_t = data_t_;
    using index_wrapper_t = index_wrapper_t_;
    using index_t = typename index_wrapper_t::type;
    using index_accessor_t = typename index_wrapper_t::template get<own_index>;
    using index_element_t = typename index_accessor_t::value_type;
private:
    using lookup_map_t = std::unordered_map<index_element_t, UnorderedComponentMapStorage<data_t, index_wrapper_t, own_index + 1, last_index, on_duplicate_index_strategy>>;

public:
    template<typename... Args>
    inline data_t& insert(const index_t& index, Args&&... args)
    {
        return lookup_[index_accessor_t::access(index)].insert(index, std::forward<Args>(args)...);
    }

    inline data_t* access(const index_t& index)
    {
        auto itr = lookup_.find(index_accessor_t::access(index));
        if (itr == lookup_.end())
            return nullptr;
        return itr->second.access(index);
    }

    inline const data_t* access(const index_t& index) const
    {
        auto itr = lookup_.find(index_accessor_t::access(index));
        if (itr == lookup_.end())
            return nullptr;
        return itr->second.access(index);
    }

    template<typename Fn>
    inline void traverse(index_t& index, const Fn& function)
    {
        for (auto&& entry : lookup_)
        {
            index_accessor_t::access(index) = entry.first;
            entry.second.traverse(index, function);
        }
    }

    template<typename Fn>
    inline void traverse(index_t& index, const Fn& function) const
    {
        for (auto&& entry : lookup_)
        {
            index_accessor_t::access(index) = entry.first;
            entry.second.traverse(index, function);
        }
    }

private:
    lookup_map_t lookup_;
};

template<typename data_t_, typename index_wrapper_t_, std::size_t last_index, option::MergeStrategy on_duplicate_index_strategy>
class UnorderedComponentMapStorage<data_t_, index_wrapper_t_, last_index, last_index, on_duplicate_index_strategy>
{
public:
    using data_t = data_t_;

    using index_wrapper_t = index_wrapper_t_;
    using index_t = typename index_wrapper_t::type;
    using index_accessor_t = typename index_wrapper_t::template get<last_index>;
    using index_element_t = typename index_accessor_t::value_type;
private:
    using lookup_map_t = std::unordered_map<index_element_t, data_t_>;

public:
    template<typename... Args>
    inline data_t& insert(const index_t& index, Args&&... args)
    {
        const auto index_value = index_accessor_t::access(index);
        auto itr = storage_.find(index_value);
        if (itr == storage_.end())
        {
            auto result = storage_.emplace(index_value, data_ops<data_t>::create(std::forward<Args>(args)...));
            return result.first->second;
        }
        else
        {
            auto& value = itr->second;
            data_ops<data_t>::template merge<on_duplicate_index_strategy>(value, std::forward<Args>(args)...);
            return value;
        }
    }

    inline data_t* access(const index_t& index)
    {
        auto itr = storage_.find(index_accessor_t::access(index));
        if (itr == storage_.end())
            return nullptr;
        return &(itr->second);
    }

    inline const data_t* access(const index_t& index) const
    {
        auto itr = storage_.find(index_accessor_t::access(index));
        if (itr == storage_.end())
            return nullptr;
        return &(itr->second);
    }

    template<typename Fn>
    inline void traverse(index_t& index, const Fn& function)
    {
        for (auto&& entry : storage_)
        {
            index_accessor_t::access(index) = entry.first;
            function(index, entry.second);
        }
    }

    template<typename Fn>
    inline void traverse(index_t& index, const Fn& function) const
    {
        for (auto&& entry : storage_)
        {
            index_accessor_t::access(index) = entry.first;
            function(index, entry.second);
        }
    }

private:
    lookup_map_t storage_;
};

}

template<typename data_t_, typename index_wrapper_t_, typename... options_ts_>
class UnorderedComponentMap
{
public:
    using data_t = data_t_;
    using index_wrapper_t = index_wrapper_t_;
    using index_t = typename index_wrapper_t::type;

    static constexpr auto on_duplicate_index_strategy = option::get_option<option::merge_strategy_opt, options_ts_...>::value;

private:
    static constexpr auto index_dimensions = index_wrapper_t::dimensions;
    using storage_t = detail::UnorderedComponentMapStorage<data_t, index_wrapper_t, 0, index_dimensions - 1, on_duplicate_index_strategy>;

public:
    template<typename... Args>
    inline data_t& insert(const index_t& index, Args&&... args)
    {
        return storage_.insert(index, std::forward<Args>(args)...);
    }

    inline data_t* get(const index_t& index)
    {
        return storage_.access(index);
    }

    inline const data_t* get(const index_t& index) const
    {
        return storage_.access(index);
    }

    template<typename Fn>
    inline void traverse(const Fn& function)
    {
        index_t index{};
        return storage_.traverse(index, function);
    }

    template<typename Fn>
    inline void traverse(const Fn& function) const
    {
        index_t index{};
        return storage_.traverse(index, function);
    }

private:
    storage_t storage_;
};

}
}}
