#pragma once

#include <cslibs_clustering/backend/options.hpp>
#include <utility>

namespace cslibs_clustering
{
namespace interface
{
namespace detail
{
template<typename type, option::MergeStrategy strategy>
struct data_merger;

template<typename type>
struct data_merger<type, option::MergeStrategy::REPLACE>
{
    template<typename... Args>
    static constexpr inline void apply(type& self, Args&&... args)
    {
        do_apply(typename std::is_copy_assignable<type>::type{}, self, std::forward<Args>(args)...);
    }

private:
    template<typename... Args>
    static constexpr inline void do_apply(std::true_type, type& self, Args&&... args)
    {
        self = {std::forward<Args>(args)...};
    }

    template<typename... Args>
    static constexpr inline void do_apply(std::false_type, type& self, Args&&... args)
    {
        self.~type();
        new (&self) type(std::forward<Args>(args)...);
    }
};

template<typename type>
struct data_merger<type, option::MergeStrategy::MERGE>
{
    template<typename... Args>
    static constexpr inline void apply(type& self, Args&&... args)
    {
        self.merge(std::forward<Args>(args)...);
    }
};


template<typename base_type>
struct data_merger<base_type*, option::MergeStrategy::REPLACE>
{
    using type = typename std::add_pointer<base_type>::type;

    static constexpr inline void apply(type*& self, type other)
    {
        self = other;
    }
};

template<typename base_type>
struct data_merger<base_type*, option::MergeStrategy::MERGE>
{
    using type = typename std::add_pointer<base_type>::type;

    static constexpr inline void apply(type& self, type other)
    {
        self->merge(*other);
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
        return detail::data_merger<type, strategy>::apply(self, std::forward<Args>(args)...);
    }
};

template<typename data_t>
struct data_interface<data_t*>
{
    using type = data_t*;

    static inline constexpr type create(type self)
    {
        return self;
    }

    template<option::MergeStrategy strategy>
    static inline constexpr void merge(type& self, type other)
    {
        return detail::data_merger<type, strategy>::apply(self, other);
    }
};

}
}