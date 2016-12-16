#pragma once

#include <cslibs_clustering/backend/options.hpp>
#include <utility>

namespace cslibs_clustering
{
namespace interface
{
namespace detail
{
template<option::MergeStrategy strategy>
struct data_merger;

template<>
struct data_merger<option::MergeStrategy::REPLACE>
{
    template<typename type, typename... Args>
    static constexpr inline auto apply(type& self, Args&&... args) -> typename std::enable_if<std::is_copy_assignable<type>::value>::type
    {
        self = {std::forward<Args>(args)...};
    }

    template<typename type, typename... Args>
    static constexpr inline auto apply(type& self, Args&&... args) -> typename std::enable_if<!std::is_copy_assignable<type>::value>::type
    {
        self.~type();
        new (&self) type(std::forward<Args>(args)...);
    }
};

template<>
struct data_merger<option::MergeStrategy::MERGE>
{
    template<typename type, typename... Args>
    static constexpr inline void apply(type& self, Args&&... args)
    {
        self.merge(std::forward<Args>(args)...);
    }
};

}

template<typename data_t>
struct data_interface
{
    using type = data_t;

    template<typename... Args>
    static inline constexpr type create(Args&&... args)
    {
        return { std::forward<Args>(args)... };
    }

    template<option::MergeStrategy strategy, typename... Args>
    static inline constexpr void merge(type& self, Args&&... args)
    {
        return detail::data_merger<strategy>::apply(self, std::forward<Args>(args)...);
    }
};

}
}