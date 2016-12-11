#pragma once

#include <cslibs_clustering/helper/static_warning.hpp>
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
template<typename data_t_, typename index_wrapper_t_, std::size_t own_index, std::size_t last_index>
class NestedComponentMapStorage
{
public:
    using data_t = data_t_;
    using index_wrapper_t = index_wrapper_t_;
    using index_t = typename index_wrapper_t::type;
    using index_accessor_t = typename index_wrapper_t::template get<own_index>;
    using index_element_t = typename index_accessor_t::value_type;
private:
    using lookup_map_t = std::unordered_map<index_element_t, NestedComponentMapStorage<data_t, index_wrapper_t, own_index + 1, last_index>>;

public:
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

private:
    lookup_map_t lookup_;
};

template<typename data_t_, typename index_wrapper_t_, std::size_t last_index>
class NestedComponentMapStorage<data_t_, index_wrapper_t_, last_index, last_index>
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

private:
    lookup_map_t storage_;
};

}

template<typename data_t_, typename index_wrapper_t_, typename... options_t_>
class NestedComponentMap
{
public:
    using data_t = data_t_;
    using index_wrapper_t = index_wrapper_t_;
    using index_t = typename index_wrapper_t::type;
private:
    static constexpr auto index_dimensions = index_wrapper_t::dimensions;
    using storage_t = detail::NestedComponentMapStorage<data_t, index_wrapper_t, 0, index_dimensions - 1>;

    STATIC_WARNING_TEMPLATE(UNIQUE, sizeof...(options_t_) == 0, "Has no supported options");

public:
    inline data_t* get(const index_t& index)
    {
        return storage_.access(index);
    }

    inline const data_t* get(const index_t& index) const
    {
        return storage_.access(index);
    }

private:
    storage_t storage_;
};

}
}}
