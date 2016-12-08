#pragma once

#include "support/index.hpp"

namespace cslibs_clustering
{

template<typename data_t_, typename index_t_, template<typename, typename> class backend_t_>
class Storage
{
public:
    using data_t = data_t_;
    using index_wrapper_t = support::index_wrapper<index_t_>;
    using index_t = typename index_wrapper_t::raw_type;
    using backend_t = backend_t_<data_t, index_wrapper_t>;

    template<typename T>
    inline data_t_& insert(const index_t& index, T&& data)
    {
        return backend_.get(index) = std::forward<T>(data);
    }

    inline data_t& insert(const index_t& index, data_t data)
    {
        return backend_.get(index) = std::move(data);
    }

    inline data_t& get(const index_t& index)
    {
        return backend_.get(index);
    }

    inline const data_t& get(const index_t& index) const
    {
        return backend_.get(index);
    }

private:
    backend_t backend_;
};

}
