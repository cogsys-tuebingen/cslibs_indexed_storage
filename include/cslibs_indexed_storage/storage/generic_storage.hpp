#pragma once

#include <cslibs_indexed_storage/interface/index/index_interface.hpp>
#include <cslibs_indexed_storage/interface/data/data_interface.hpp>

namespace cslibs_indexed_storage
{

/**
 * Generic wrapper type to abstract away the actual storage implementation.
 *
 * The complete access should be mananged through this class.
 *
 * @tparam hinted_data_t_ actual data that is stored
 * @tparam index_t_ index for addressing
 * @tparam backend_t_ used backend implemenation
 * @tparam args_t_ [optional] configuration arguments for the backend
 */
template<typename hinted_data_t_, typename index_t_, template<typename, typename, typename...> class backend_t_, typename... args_t_>
class Storage
{
public:
    using data_if = interface::data_interface<hinted_data_t_>;
    using data_input_t = typename data_if::input_type;
    using data_output_t = typename data_if::output_type;
    using data_t = data_output_t;

    using index_if = interface::index_interface<index_t_>;
    using index_t = typename index_if::type;

    using backend_t = backend_t_<data_if, index_if, args_t_...>;

    template<typename... Args>
    inline data_output_t& insert(const index_t& index, Args&& ... data)
    {
        return backend_.insert(index, std::forward<Args>(data)...);
    }

    inline data_output_t& insert(const index_t& index, data_input_t data)
    {
        return backend_.insert(index, std::move(data));
    }

    inline data_output_t* get(const index_t& index)
    {
        return backend_.get(index);
    }

    inline const data_output_t* get(const index_t& index) const
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

    inline void clear()
    {
        return backend_.clear();
    }

    template<typename tag, typename... Args>
    inline void set(Args&&... args)
    {
        return set(tag{}, std::forward<Args>(args)...);
    }

    template<typename tag, typename... Args>
    inline void set(tag, Args&&... args)
    {
        return backend_.set(tag{}, std::forward<Args>(args)...);
    }

private:
    backend_t backend_;
};

}