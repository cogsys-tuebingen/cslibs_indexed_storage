#pragma once

#include <utility>

namespace cslibs_clustering
{

template<typename T, typename... Args>
inline constexpr void merge(T& self, Args&&... args)
{
    self.merge(std::forward<Args>(args)...);
}

}
