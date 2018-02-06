#pragma once

#include <cslibs_indexed_storage/interface/data/data_storage_hints.hpp>
#include <cslibs_indexed_storage/interface/data/data_merge.hpp>
#include <cslibs_indexed_storage/interface/data/detail/data_interface_byte_size.hpp>
#include <utility>

namespace cslibs_indexed_storage
{
namespace interface
{
namespace detail
{

template<typename type, option::MergeStrategy strategy>
struct dense_data_merger;

template<typename type>
struct dense_data_merger<type, option::MergeStrategy::REPLACE>
{
    template<typename... Args>
    static constexpr inline void apply(type& self, Args&&... args)
    {
        self.~type();
        new (&self) type(std::forward<Args>(args)...);
    }
};

template<typename type>
struct dense_data_merger<type, option::MergeStrategy::MERGE>
{
    template<typename... Args>
    static constexpr inline void apply(type& self, Args&&... args)
    {
        using ::cslibs_indexed_storage::merge;
        merge(self, std::forward<Args>(args)...);
    }
};

}

template<typename data_t>
struct dense_data_interface
{
    using hinted_type = dense<data_t>;
    using base_type = data_t;
    using storage_type = data_t;
    using input_type = data_t;
    using output_type = data_t;

    template<typename... Args>
    static inline constexpr storage_type create(Args&&... args)
    {
#if __GNUC__ > 5
        return { std::forward<Args>(args)... };
#else
        return storage_type(std::forward<Args>(args)...);
#endif
    }

    template<option::MergeStrategy strategy, typename... Args>
    static inline constexpr void merge(storage_type& self, Args&&... args)
    {
        return detail::dense_data_merger<storage_type, strategy>::apply(self, std::forward<Args>(args)...);
    }

    static inline constexpr output_type& expose(storage_type& storage)
    {
        return storage;
    }
    static inline constexpr const output_type& expose(const storage_type& storage)
    {
        return storage;
    }

    static inline constexpr std::size_t byte_size(const storage_type& storage)
    {
        return detail::byte_size(expose(storage));
    }
};
}
}
