#pragma once

#include <cslibs_indexed_storage/utility/void_t.hpp>
#include <cslibs_indexed_storage/utility/concept_check.hpp>
#include <cslibs_indexed_storage/operations/clustering/cluster_op_traits.hpp>

namespace cslibs_indexed_storage
{
namespace operations
{
namespace clustering
{

template<typename storage_t_>
class Clustering
{
public:
    using storage_t = storage_t_;
    using data_t = typename storage_t::data_t;
    using index_if = typename storage_t::index_if;
    using index_t = typename storage_t::index_t;

    Clustering(storage_t& storage) :
            storage_(storage) {}

    template<typename cluster_op_t>
    void cluster(cluster_op_t& clusters_op)
    {
        storage_.traverse([&](const index_t& index, data_t& data)
                          {
                              if (!clusters_op.start(index, data))
                                  return;

                              cluster(clusters_op, index);
                          });
    }

private:
    template<typename cluster_op_t>
    void cluster(cluster_op_t& cluster_op, const index_t& center)
    {
        using traits = cluster_op_traits<cluster_op_t>;

        cluster_op.visit_neighbours(center,
                                    //! \todo can be replaced by auto in lambda with c++14, cannot use member function and std::bind due to performance reasons :/
                                    [this, &center, &cluster_op](const typename traits::visitor_index_t& offset)
                                    {
                                        auto index = do_add(utility::check_feature_exists{}, cluster_op, center, offset);
                                        auto neighbour = storage_.get(index);
                                        if (!neighbour)
                                            return;

                                        if (!cluster_op.extend(center, index, *neighbour))
                                            return;

                                        cluster(cluster_op, index);
                                    });
    }

    template<typename cluster_op_t, typename offset_t>
    auto do_add(utility::feature_exists_tag, const cluster_op_t& cluster_op, const index_t& index, const offset_t& offset)
            -> decltype(cluster_op.add(index, offset))
    {
        return cluster_op.add(index, offset);
    }

    template<typename cluster_op_t, typename offset_t>
    auto do_add(utility::feature_missing_tag, const cluster_op_t&, const index_t& index, const offset_t& offset)
        -> decltype(index_if::add(index, offset))
    {
        return index_if::add(index, offset);
    }

private:
    storage_t& storage_;
};

}
}
}
