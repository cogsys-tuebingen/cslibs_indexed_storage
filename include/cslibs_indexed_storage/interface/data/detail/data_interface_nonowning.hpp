#pragma once

#include <cslibs_indexed_storage/interface/data/data_storage_hints.hpp>
#include <cslibs_indexed_storage/interface/data/data_merge.hpp>
#include <utility>

namespace cslibs_indexed_storage
{
namespace interface
{
namespace detail
{
template<typename type, option::MergeStrategy strategy>
struct nonowning_data_merger;

template<typename type>
struct nonowning_data_merger<type, option::MergeStrategy::REPLACE>
{
    static constexpr inline void apply(type& self, type other)
    {
        self = other;
    }
};

template<typename type>
struct nonowning_data_merger<type, option::MergeStrategy::MERGE>
{
    static constexpr inline void apply(type& self, type other)
    {
        using ::cslibs_indexed_storage::merge;
        merge(*self, *other);
    }

    template<typename... Args>
    static constexpr inline void apply(type& self, Args&&... args)
    {
        using ::cslibs_indexed_storage::merge;
        merge(*self, std::forward<Args>(args)...);
    }
};
}

template<typename data_t>
struct nonowning_data_interface
{
    using hinted_type = non_owning<data_t>;
    using base_type = data_t;
    using storage_type = typename std::add_pointer<data_t>::type;
    using input_type = typename std::add_pointer<data_t>::type;
    using output_type = data_t;

    template<typename... Args>
    static inline constexpr storage_type create(input_type input)
    {
        return input;
    }

    template<option::MergeStrategy strategy, typename... Args>
    static inline constexpr void merge(storage_type& self, Args&&... args)
    {
        return detail::nonowning_data_merger<storage_type, strategy>::apply(self, std::forward<Args>(args)...);
    }

    static inline constexpr output_type& expose(storage_type& storage)
    {
        return *storage;
    }
    static inline constexpr const output_type& expose(const storage_type& storage)
    {
        return *storage;
    }

    static inline constexpr std::size_t byte_size(const storage_type& storage)
    {
        return sizeof(storage_type); // we do not own the data -> not accumulated in byte size
    }

    static inline void deallocate(storage_type& storage)
    {
        // we do not own the data -> data needs to be deallocated externally
    }
};
}
}
