#pragma once

#include <cslibs_clustering/interface/data/data_storage_hints.hpp>
#include <utility>

namespace cslibs_clustering
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
        self.merge(std::forward<Args>(args)...);
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
        return { std::forward<Args>(args)... };
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
};
}
}
