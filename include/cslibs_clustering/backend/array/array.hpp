#pragma once

#include <cslibs_clustering/backend/options.hpp>
#include <cslibs_clustering/backend/array/array_options.hpp>
#include <cslibs_clustering/backend/data_ops.hpp>
#include <boost/dynamic_bitset.hpp>
#include <limits>

namespace cslibs_clustering
{
namespace backend
{
namespace array
{

template<typename data_t_, typename index_wrapper_t_, typename... options_ts_>
class Array
{
public:
    using data_t = data_t_;
    using data_ops = ops::DataOps<data_t>;

    using index_wrapper_t = index_wrapper_t_;
    using index_t = typename index_wrapper_t::type;

    using array_size_t = std::array<std::size_t, index_wrapper_t::dimensions>;
    using array_offset_t = index_t;

    using on_duplicate_index_opt = helper::get_option_t<
            options::tags::on_duplicate_index,
            options::on_duplicate_index<options::OnDuplicateIndex::REPLACE>,
            options_ts_...>;
    static constexpr auto on_duplicate_index_strategy = on_duplicate_index_opt::value;

    using array_size_opt = helper::get_option_t<
            options::tags::array_size,
            options::array_size<>,
            options_ts_...>;
    static constexpr auto static_array_size = array_size_opt::type::value;

    using array_offset_opt = helper::get_option_t<
            options::tags::array_offset,
            options::array_offset<index_t>,
            options_ts_...>;
    static constexpr auto static_array_offset = array_offset_opt::type::value;

private:
    using internal_index_t = std::size_t;
    static constexpr auto invalid_index_value = std::numeric_limits<internal_index_t>::max();

public:
    ~Array()
    {
        delete[] storage_;
    }

    template<typename... Args>
    inline data_t& insert(const index_t& index, Args&&... args)
    {
        auto internal_index = to_internal_index(index);
        if (internal_index  == invalid_index_value)
            throw std::runtime_error("Invalid index"); //! \todo find better handling?

        if (!valid_.test_set(internal_index))
        {
            auto& value = storage_[internal_index];
            value = data_ops::create(std::forward<Args>(args)...);
            return value;
        }
        else
        {
            auto& value = storage_[internal_index];
            data_ops::template merge<on_duplicate_index_strategy>(value, std::forward<Args>(args)...);
            return value;
        }
    }

    inline data_t* get(const index_t& index)
    {
        auto internal_index = to_internal_index(index);
        if (internal_index  == invalid_index_value)
            return nullptr;

        if (!valid_.test(internal_index))
            return nullptr;
        else
            return &(storage_[internal_index]);
    }

    inline const data_t* get(const index_t& index) const
    {
        auto internal_index = to_internal_index(index);
        if (internal_index  == invalid_index_value)
            return nullptr;

        if (!valid_.test(internal_index))
            return nullptr;
        else
            return &(storage_[internal_index]);
    }

    template<typename Fn>
    inline void traverse(const Fn& function)
    {
        auto index = valid_.find_first();
        while (index != decltype(valid_)::npos)
        {
            function(to_external_index(index), storage_[index]);
            index = valid_.find_next(index);
        }
    }

    template<typename... Args>
    void set(options::tags::array_size, Args&&... new_size)
    {
        if (valid_.any())
            throw std::runtime_error("Resize not allowed with active data");

        size_ = {std::forward<Args>(new_size)...};
        resize();
    }

    template<typename... Args>
    void set(options::tags::array_offset, Args&&... new_offset)
    {
        if (valid_.any())
            throw std::runtime_error("Offset change not allowed with active data");

        offset_ = {std::forward<Args>(new_offset)...};
    }

private:
    void resize()
    {
        std::size_t size = 1;
        for (std::size_t i = 0; i < index_wrapper_t::dimensions; ++i)
            size *= size_[i];

        delete[] storage_;
        storage_ = new data_t[size];

        valid_.clear();
        valid_.resize(size);
    }

    internal_index_t to_internal_index(const index_t& index) const
    {
        internal_index_t internal_index{};
        for (std::size_t i = 0; i < index_wrapper_t::dimensions; ++i)
        {
            const auto value = index[i] - offset_[i];
            if (value < 0 || value > size_[i])
                return invalid_index_value;

            internal_index = internal_index * size_[i] + value;
        }
        return internal_index;
    }

    index_t to_external_index(internal_index_t internal_index) const
    {
        index_t index;
        for (std::size_t i = 0;i < index_wrapper_t::dimensions; ++i)
        {
            index[i] = internal_index % size_[i] + offset_[i];
            internal_index = internal_index / size_[i];
        }
        return index;
    }

    static constexpr std::size_t to_flat_size(/*const array_size_t& array_size*/)
    {
        return 10 * 10;
    }

private:
    array_size_t size_ = static_array_size;
    array_offset_t offset_ = static_array_offset;

    data_t* storage_ = new data_t[to_flat_size(/*static_array_size*/)];
    boost::dynamic_bitset<uint64_t> valid_{to_flat_size(/*static_array_size*/)};
};

}
}
}