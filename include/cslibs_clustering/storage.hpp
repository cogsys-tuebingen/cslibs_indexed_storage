#pragma once

#include <cslibs_clustering/index/index.hpp>

namespace cslibs_clustering
{

template<typename data_t_, typename index_t_, template<typename, typename, typename...> class backend_t_, typename... args_t_>
class Storage
{
public:
    using data_t = data_t_;
    using index_wrapper_t = support::index_wrapper<index_t_>;
    using index_t = typename index_wrapper_t::type;
    using backend_t = backend_t_<data_t, index_wrapper_t, args_t_...>;

    template<typename... Args>
    inline data_t_& insert(const index_t& index, Args&&... data)
    {
        return backend_.insert(index, std::forward<Args>(data)...);
    }

    inline data_t& insert(const index_t& index, data_t data)
    {
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
};

}
