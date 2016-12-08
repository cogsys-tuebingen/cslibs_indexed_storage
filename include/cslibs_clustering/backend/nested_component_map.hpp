#pragma once

#include <cstdint>
#include <unordered_map>
#include <type_traits>

namespace cslibs_clustering
{

namespace backend
{
namespace detail
{
template<typename data_t_, typename index_t_, std::size_t own_index, std::size_t last_index>
class NestedComponentMapStorage
{
public:
    using data_t = data_t_;
    using index_t = index_t_;
    using index_element_t = typename std::decay<decltype(std::declval<index_t_>()[own_index])>::type;
private:
    using lookup_map_t = std::unordered_map<index_element_t, NestedComponentMapStorage<data_t, index_t, own_index + 1, last_index>>;

public:
    inline data_t& access(const index_t& index)
    {
        return lookup_[index[own_index]].access(index);
    }

    inline const data_t& access(const index_t& index) const
    {
        return lookup_[index[own_index]].access(index);
    }

private:
    lookup_map_t lookup_;
};

template<typename data_t_, typename index_t_, std::size_t last_index>
class NestedComponentMapStorage<data_t_, index_t_, last_index, last_index>
{
public:
    using data_t = data_t_;
    using index_t = index_t_;
    using index_element_t = typename std::decay<decltype(std::declval<index_t_>()[last_index])>::type;
private:
    using lookup_map_t = std::unordered_map<index_element_t, data_t_>;

public:
    inline data_t& access(const index_t& index)
    {
        return storage_[index[last_index]];
    }

    inline const data_t& access(const index_t& index) const
    {
        return storage_[index[last_index]];
    }

private:
    lookup_map_t storage_;
};

}

template<typename data_t_, typename index_t_>
class NestedComponentMap
{
public:
    using data_t = data_t_;
    using index_wrapper_t = index_t_;
    using index_t = typename index_wrapper_t::raw_type;
private:
    static constexpr auto index_dimensions = index_wrapper_t::type::dimensions;
    using storage_t = detail::NestedComponentMapStorage<data_t, index_t, 0, index_dimensions - 1>;

public:
    inline data_t& get(const index_t& index)
    {
        return storage_.access(index);
    }

    inline const data_t& get(const index_t& index) const
    {
        return storage_.access(index);
    }

private:
    storage_t storage_;
};

}
}
