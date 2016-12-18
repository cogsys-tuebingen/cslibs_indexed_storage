#pragma once

#include <cslibs_clustering/storage/generic_storage.hpp>

namespace cslibs_clustering
{

template<typename data_t>
struct auto_index;


template<typename data_interface_t_, template<typename, typename, typename...> class backend_t_, typename... args_t_>
class Storage<data_interface_t_, auto_index<data_interface_t_>, backend_t_, args_t_...>
{
public:
    using data_if = interface::data_interface<data_interface_t_>;
    using data_t = typename data_if::type;

    using auto_index_t = auto_index<typename std::remove_pointer<data_t>::type>;
    using auto_index_return_t = typename std::decay<decltype(std::declval<auto_index_t>().index(std::declval<typename std::remove_pointer<data_t>::type>()))>::type;
    using index_if = interface::index_interface<auto_index_return_t>;
    using index_t = typename index_if::type;

    using backend_t = backend_t_<data_if, index_if, args_t_...>;

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
    inline data_interface_t_& insert(Args&& ... args)
    {
        return insert(data_if::create(std::forward<Args>(args)...));
    }

    inline data_t& insert(data_t data)
    {
        auto index = extract_index(typename std::is_pointer<data_t>::type{}, data);
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
    inline constexpr index_t extract_index(std::false_type, const data_t& data)
    {
        return indexer_.index(data);
    }
    inline constexpr index_t extract_index(std::true_type, const data_t& data)
    {
        return indexer_.index(*data);
    }

private:
    backend_t backend_;
    auto_index_t indexer_;
};

template<typename data_t, template<typename, typename, typename...> class backend_t_, typename... args_t_>
using AutoIndexStorage = Storage<data_t, auto_index<data_t>, backend_t_, args_t_...>;

}