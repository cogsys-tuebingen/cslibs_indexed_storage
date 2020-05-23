#pragma once

#include <cslibs_indexed_storage/backend/options.hpp>
#include <cslibs_indexed_storage/backend/array/array_options.hpp>
#include <cslibs_indexed_storage/interface/data/data_interface.hpp>
#include <cslibs_indexed_storage/interface/data/align/aligned_array.hpp>
#include <cslibs_indexed_storage/backend/tags.hpp>
#include <cslibs_indexed_storage/backend/backend_traits.hpp>
#include <cslibs_indexed_storage/utility/index_sequence.hpp>
#include <boost/dynamic_bitset.hpp>
#include <limits>
#include <memory>

namespace cslibs_indexed_storage { namespace backend { namespace array
{

namespace detail
{
template<typename T, typename TT>
struct empty_array_maker;

template<typename T, std::size_t... idx>
struct empty_array_maker<T, utility::index_sequence<idx...>>
{
    using type = option::detail::value_holder<typename T::value_type, ((void) idx, 0)...>;
};

template<typename T, std::size_t count>
using empty_array = typename empty_array_maker<T, typename utility::make_index_sequence<count>::type>::type;
}

template<typename data_interface_t_, typename index_interface_t_, typename... options_ts_>
class Array
{
public:
    using tag = array_tag;

    using data_if = data_interface_t_;
    using data_storage_t = typename data_if::storage_type;
    using data_output_t = typename data_if::output_type;

    using index_if = index_interface_t_;
    using index_t = typename index_if::type;

    using array_size_t = std::array<std::size_t, index_if::dimensions>;
    using array_offset_t = index_t;

    static constexpr auto on_duplicate_index_strategy = option::get_option<option::merge_strategy_opt, options_ts_...>::value;
    static constexpr auto static_array_size     = option::get_option_raw<option::tags::array_size, detail::empty_array<array_size_t, std::tuple_size<array_size_t>::value>, options_ts_...>::value;
    static constexpr auto static_array_offset   = option::get_option_raw<option::tags::array_offset, detail::empty_array<array_offset_t, std::tuple_size<array_offset_t>::value>, options_ts_...>::value;

private:
    using internal_index_t = std::size_t;
    static constexpr auto invalid_index_value = std::numeric_limits<internal_index_t>::max();
    using internal_storage_t = interface::aligned_array<data_storage_t>;

public:
    Array() :
            size_(static_array_size),
            offset_(static_array_offset),
            storage_(get_internal_size())
    {}

    template<typename... Args>
    inline data_output_t& insert(const index_t& index, Args&&... args)
    {
        auto internal_index = to_internal_index(index);
        if (internal_index  == invalid_index_value)
            throw std::runtime_error("Invalid index"); //! \todo find better handling?

#if BOOST_VERSION >= 105600
        if (!valid_.test_set(internal_index))
        {
            auto& value = storage_[internal_index];
            value = data_if::create(std::forward<Args>(args)...);
            return data_if::expose(value);
        }
#else
        if (!valid_.test(internal_index))
        {
            auto& value = storage_[internal_index];
            value = data_if::create(std::forward<Args>(args)...);
            valid_.set(internal_index);
            return data_if::expose(value);
        }
#endif
        else
        {
            auto& value = storage_[internal_index];
            data_if::template merge<on_duplicate_index_strategy>(value, std::forward<Args>(args)...);
            return data_if::expose(value);
        }
    }

    inline data_output_t* get(const index_t& index)
    {
        auto internal_index = to_internal_index(index);
        if (internal_index  == invalid_index_value)
            return nullptr;

        if (!valid_.test(internal_index))
            return nullptr;
        else
            return &data_if::expose(storage_[internal_index]);
    }

    inline const data_output_t* get(const index_t& index) const
    {
        auto internal_index = to_internal_index(index);
        if (internal_index  == invalid_index_value)
            return nullptr;

        if (!valid_.test(internal_index))
            return nullptr;
        else
            return &data_if::expose(storage_[internal_index]);
    }

    template<typename Fn>
    inline void traverse(const Fn& function)
    {
        using valid_storage = decltype(valid_);

        auto index = valid_.find_first();
        while (index != valid_storage::npos)
        {
            function(to_external_index(index), data_if::expose(storage_[index]));
            index = valid_.find_next(index);
        }
    }

    template<typename Fn>
    inline void traverse(const Fn& function) const
    {
        using valid_storage = decltype(valid_);

        auto index = valid_.find_first();
        while (index != valid_storage::npos)
        {
            function(to_external_index(index), data_if::expose(storage_[index]));
            index = valid_.find_next(index);
        }
    }

    inline void clear()
    {
        valid_.reset();
    }

    inline std::size_t size() const
    {
        return valid_.count(); //!\todo is this a O(1) operation?
    }

    inline std::size_t byte_size() const
    {
        std::size_t content_bytes = 0;
        using valid_storage = decltype(valid_);

        for (std::size_t i=0; i<capacity(); ++i)
            content_bytes += data_if::byte_size(storage_[i]);

        return sizeof(*this) // static self
               + valid_.num_blocks() * sizeof(typename valid_storage::block_type) // dynamic "valid_" size
               + content_bytes;
    }

    inline std::size_t capacity() const
    {
        return get_internal_size();
    }

    template<typename... Args>
    void set(option::tags::array_size, Args&&... new_size)
    {
        if (valid_.any())
            throw std::runtime_error("Resize not allowed with active data");

        size_ = {std::forward<Args>(new_size)...};
        resize();
    }

    template<typename... Args>
    void set(option::tags::array_offset, Args&&... new_offset)
    {
        if (valid_.any())
            throw std::runtime_error("Offset change not allowed with active data");

        offset_ = {std::forward<Args>(new_offset)...};
    }

private:
    void resize()
    {
        const std::size_t size = get_internal_size();

        storage_.resize(size);

        valid_.clear();
        valid_.resize(size);
    }

    internal_index_t to_internal_index(const index_t& index) const
    {
        internal_index_t internal_index{};
        for (std::size_t i = 0; i < index_if::dimensions; ++i)
        {
            const auto value = index_if::access(i, index) - offset_[i];
            if (value < 0 || size_[i] - value <= 0)
                return invalid_index_value;

            internal_index = internal_index * size_[i] + value;
        }
        return internal_index;
    }

    index_t to_external_index(internal_index_t internal_index) const
    {
        index_t index;
        for (std::size_t i = index_if::dimensions; i > 0; --i)
        {
            const std::size_t ri = i - 1;
            index_if::access(ri, index) = internal_index % size_[ri] + offset_[ri];
            internal_index = internal_index / size_[ri];
        }
        return index;
    }

    std::size_t get_internal_size() const
    {
        std::size_t size = 1;
        for (std::size_t i = 0; i < index_if::dimensions; ++i)
            size *= size_[i];

        return size;
    }

private:
    array_size_t size_;
    array_offset_t offset_;

    internal_storage_t storage_;
    boost::dynamic_bitset<uint64_t> valid_ = boost::dynamic_bitset<uint64_t>(get_internal_size());
};

}}}

namespace cslibs_indexed_storage { namespace backend
{

template<>
struct backend_traits<array_tag>
{
    static constexpr bool IsFixedSize = true;
};

}}
