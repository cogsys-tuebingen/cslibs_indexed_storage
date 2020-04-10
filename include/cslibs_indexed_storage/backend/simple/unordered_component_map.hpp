#pragma once

#include <cslibs_indexed_storage/backend/tags.hpp>
#include <cslibs_indexed_storage/backend/backend_traits.hpp>
#include <cslibs_indexed_storage/interface/data/data_interface.hpp>
#include <cslibs_indexed_storage/interface/data/align/aligned_allocator.hpp>
#include <cstdint>
#include <unordered_map>
#include <type_traits>
#include <algorithm>
#include <numeric>

namespace cslibs_indexed_storage { namespace backend { namespace simple
{
namespace detail
{
template<typename data_interface_t_, typename index_interface_t_, std::size_t own_index, std::size_t last_index, option::MergeStrategy on_duplicate_index_strategy>
class UnorderedComponentMapStorage
{
public:
    using data_if = data_interface_t_;
    using data_output_t = typename data_if::output_type;
    using index_if = index_interface_t_;
    using index_t = typename index_if::type;
    using index_accessor_t = typename index_if::template dimension<own_index>;
    using index_element_t = typename index_accessor_t::value_type;
private:
    using lookup_map_t = std::unordered_map<index_element_t, UnorderedComponentMapStorage<data_if, index_if, own_index + 1, last_index, on_duplicate_index_strategy>>;

public:
    template<typename... Args>
    inline data_output_t& insert(const index_t& index, Args&&... args)
    {
        return lookup_[index_accessor_t::access(index)].insert(index, std::forward<Args>(args)...);
    }

    inline data_output_t* access(const index_t& index)
    {
        auto itr = lookup_.find(index_accessor_t::access(index));
        if (itr == lookup_.end())
            return nullptr;
        return itr->second.access(index);
    }

    inline const data_output_t* access(const index_t& index) const
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

    inline void clear()
    {
        lookup_.clear();
    }

    inline std::size_t byte_size() const
    {
        return sizeof(*this) +
                std::accumulate(lookup_.begin(), lookup_.end(), std::size_t(0),
                                [](std::size_t before, const typename lookup_map_t::value_type& entry)
                                {
                                    return before + entry.second.byte_size();
                                });
    }

    inline std::size_t size() const
    {
        return std::accumulate(lookup_.begin(), lookup_.end(), std::size_t(0),
                               [](std::size_t before, const typename lookup_map_t::value_type& entry)
                               {
                                   return before + entry.second.size();
                               });
    }

private:
    lookup_map_t lookup_;
};

template<typename data_interface_t_, typename index_interface_t_, std::size_t last_index, option::MergeStrategy on_duplicate_index_strategy>
class UnorderedComponentMapStorage<data_interface_t_, index_interface_t_, last_index, last_index, on_duplicate_index_strategy>
{
public:
    using data_if = data_interface_t_;
    using data_output_t = typename data_if::output_type;
    using data_storage_t = typename data_if::storage_type;

    using index_if = index_interface_t_;
    using index_t = typename index_if::type;
    using index_accessor_t = typename index_if::template dimension<last_index>;
    using index_element_t = typename index_accessor_t::value_type;
private:
    using lookup_map_t = std::unordered_map<index_element_t,
                                            data_storage_t,
                                            std::hash<index_element_t>,
                                            std::equal_to<index_element_t>,
                                            interface::aligned_allocator<std::pair<const index_element_t, data_storage_t>>>;

public:
    template<typename... Args>
    inline data_output_t& insert(const index_t& index, Args&&... args)
    {
        const auto index_value = index_accessor_t::access(index);
        auto itr = storage_.find(index_value);
        if (itr == storage_.end())
        {
            auto result = storage_.emplace(index_value, data_if::create(std::forward<Args>(args)...));
            return data_if::expose(result.first->second);
        }
        else
        {
            auto& value = itr->second;
            data_if::template merge<on_duplicate_index_strategy>(value, std::forward<Args>(args)...);
            return data_if::expose(value);
        }
    }

    inline data_output_t* access(const index_t& index)
    {
        auto itr = storage_.find(index_accessor_t::access(index));
        if (itr == storage_.end())
            return nullptr;
        return &data_if::expose(itr->second);
    }

    inline const data_output_t* access(const index_t& index) const
    {
        auto itr = storage_.find(index_accessor_t::access(index));
        if (itr == storage_.end())
            return nullptr;
        return &data_if::expose(itr->second);
    }

    template<typename Fn>
    inline void traverse(index_t& index, const Fn& function)
    {
        for (auto&& entry : storage_)
        {
            index_accessor_t::access(index) = entry.first;
            function(index, data_if::expose(entry.second));
        }
    }

    template<typename Fn>
    inline void traverse(index_t& index, const Fn& function) const
    {
        for (auto&& entry : storage_)
        {
            index_accessor_t::access(index) = entry.first;
            function(index, data_if::expose(entry.second));
        }
    }

    inline void clear()
    {
        storage_.clear();
    }

    inline std::size_t byte_size() const
    {
        return sizeof(*this) +
                std::accumulate(storage_.begin(), storage_.end(), std::size_t(0),
                                [](std::size_t before, const typename lookup_map_t::value_type& entry)
                                {
                                    return before + data_if::byte_size(entry.second);
                                });
    }

    inline std::size_t size() const
    {
        return storage_.size();
    }

private:
    lookup_map_t storage_;
};

}

template<typename data_interface_t_, typename index_interface_t_, typename... options_ts_>
class UnorderedComponentMap
{
public:
    using tag = component_map_tag;

    using data_if = data_interface_t_;
    using data_output_t = typename data_if::output_type;
    using index_if = index_interface_t_;
    using index_t = typename index_if::type;

    static constexpr auto on_duplicate_index_strategy = option::get_option<option::merge_strategy_opt, options_ts_...>::value;

private:
    static constexpr auto index_dimensions = index_if::dimensions;
    using storage_t = detail::UnorderedComponentMapStorage<data_if, index_if, 0, index_dimensions - 1, on_duplicate_index_strategy>;

public:
    template<typename... Args>
    inline data_output_t& insert(const index_t& index, Args&&... args)
    {
        return storage_.insert(index, std::forward<Args>(args)...);
    }

    inline data_output_t* get(const index_t& index)
    {
        return storage_.access(index);
    }

    inline const data_output_t* get(const index_t& index) const
    {
        return storage_.access(index);
    }

    template<typename Fn>
    inline void traverse(const Fn& function)
    {
        index_t index;
        return storage_.traverse(index, function);
    }

    template<typename Fn>
    inline void traverse(const Fn& function) const
    {
        index_t index;
        return storage_.traverse(index, function);
    }

    inline void clear()
    {
        storage_.clear();
    }

    inline std::size_t byte_size() const
    {
        return sizeof(*this) + storage_.byte_size();
    }

    inline std::size_t size() const
    {
        return storage_.size();
    }

private:
    storage_t storage_;
};

}}}

namespace cslibs_indexed_storage { namespace backend
{

template<>
struct backend_traits<component_map_tag>
{
    static constexpr bool IsFixedSize = false;
};

}}
