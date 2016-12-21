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
struct sparse_data_merger;

template<typename type>
struct sparse_data_merger<type, option::MergeStrategy::REPLACE>
{
    template<typename... Args>
    static constexpr inline void apply(type& self, Args&&... args)
    {
        delete self;
        self = new type{std::forward<Args>(args)...};
    }
};

template<typename type>
struct sparse_data_merger<type, option::MergeStrategy::MERGE>
{
    template<typename... Args>
    static constexpr inline void apply(type& self, Args&&... args)
    {
        using ::cslibs_indexed_storage::merge;
        merge(*self, std::forward<Args>(args)...);
    }
};
}

template<typename data_t>
struct sparse_data_interface
{
    using hinted_type = sparse<data_t>;
    using base_type = data_t;
    using storage_type = typename std::add_pointer<data_t>::type;
    using input_type = data_t;
    using output_type = data_t;

    template<typename... Args>
    static inline constexpr storage_type create(Args&&... args)
    {
        return new base_type{ std::forward<Args>(args)... };
    }

    template<option::MergeStrategy strategy, typename... Args>
    static inline constexpr void merge(storage_type& self, Args&&... args)
    {
        return detail::sparse_data_merger<storage_type, strategy>::apply(self, std::forward<Args>(args)...);
    }

    static inline constexpr output_type& expose(storage_type& storage)
    {
        return *storage;
    }
    static inline constexpr const output_type& expose(const storage_type& storage)
    {
        return *storage;
    }
};
}
}
