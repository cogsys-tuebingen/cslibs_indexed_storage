#pragma once

namespace cslibs_indexed_storage
{
namespace operations
{
namespace clustering
{

template<typename cluster_op_t>
struct cluster_op_traits
{
    using visitor_index_t = typename cluster_op_t::visitor_index_t;
};

}
}
}