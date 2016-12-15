#pragma once

#include <cslibs_clustering/storage/generic_storage.hpp>
#include <cslibs_clustering/data/data.hpp>

namespace cslibs_clustering
{

template<typename data_t>
struct auto_index;


template<typename data_t_, template<typename, typename, typename...> class backend_t_, typename... args_t_>
class Storage<data_t_, auto_index<data_t_>, backend_t_, args_t_...>
{
public:
    using data_t = data_t_;

    using auto_index_t = auto_index<data_t>;
    using index_wrapper_t = support::index_wrapper<typename auto_index_t::index_t>;
    using index_t = typename index_wrapper_t::type;
    using backend_t = backend_t_<data_t, index_wrapper_t, args_t_...>;

public:
    Storage() = default;
    Storage(const auto_index_t& indexer) : indexer_(indexer) {}

    const auto_index_t& get_indexer() const
    {
        return indexer_;
    }

    void set_indexer(const auto_index_t& indexer)
    {
        indexer_ = indexer;
    }

    template<typename... Args>
    inline data_t_& insert(Args&& ... args)
    {
        return insert(data_ops<data_t>::create(std::forward<Args>(args)...));
    }

    inline data_t& insert(data_t data)
    {
        auto index = indexer_.index(data);
        return backend_.insert(index, std::move(data));
    }

    inline data_t* get(const index_t& index)
    {
        return backend_.get(index);
    }

    inline const data_t* get(const index_t& index) const
    {
        return backend_.get(index);
    }

    template<typename Fn>
    inline void traverse(const Fn& function)
    {
        return backend_.traverse(function);
    }

    template<typename Fn>
    inline void traverse(const Fn& function) const
    {
        return backend_.traverse(function);
    }

    template<typename tag, typename... Args>
    inline void set(Args&&... args)
    {
        return set(tag{}, std::forward<Args>(args)...);
    };

    template<typename tag, typename... Args>
    inline void set(tag, Args&&... args)
    {
        return backend_.set(tag{}, std::forward<Args>(args)...);
    };

private:
    backend_t backend_;
    auto_index_t indexer_;
};

template<typename data_t, template<typename, typename, typename...> class backend_t_, typename... args_t_>
using AutoIndexStorage = Storage<data_t, auto_index<data_t>, backend_t_, args_t_...>;

}