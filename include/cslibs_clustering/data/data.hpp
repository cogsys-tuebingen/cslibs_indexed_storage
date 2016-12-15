#pragma once

#include <cslibs_clustering/backend/options.hpp>
#include <utility>

namespace cslibs_clustering
{

template<typename data_t>
struct data_ops
{
    template<typename... Args>
    static inline constexpr data_t create(Args&&... args)
    {
        return {std::forward<Args>(args)...};
    }

    template<backend::options::OnDuplicateIndex strategy, typename... Args>
    static inline constexpr void merge(data_t& self, Args&&... args)
    {
        return strategy == backend::options::OnDuplicateIndex::REPLACE ?
               (self = create(std::forward<Args>(args)...), (void) 0) :
               self.merge(std::forward<Args>(args)...);
    }
};

}