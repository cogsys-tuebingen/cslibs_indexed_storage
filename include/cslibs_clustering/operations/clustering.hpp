#pragma once

namespace cslibs_clustering
{
namespace operations
{

template<typename storage_t_>
class Clustering
{
public:
    using storage_t = storage_t_;
    using data_t = typename storage_t::data_t;
    using index_wrapper_t = typename storage_t::index_wrapper_t;
    using index_t = typename storage_t::index_t;

    Clustering(storage_t& storage) :
            storage_(storage)
    {}

    template<typename cluster_op_t>
    void cluster(cluster_op_t& clusters_op)
    {
        storage_.traverse([&](const index_t& index, data_t& data)
                          {
                              if (!clusters_op.start(data))
                                  return;

                              cluster(clusters_op, index);
                          });
    }

private:
    template<typename cluster_op_t>
    void cluster(cluster_op_t& cluster_op, const index_t& center)
    {
        cluster_op.visit_neighbours([this, &center, &cluster_op](const index_t& offset)
                                    {
                                        auto index = index_wrapper_t::add(center, offset);
                                        auto neighbour = storage_.get(index);
                                        if (!neighbour)
                                            return;

                                        if (!cluster_op.extend(*neighbour))
                                            return;

                                        cluster(cluster_op, index);
                                    });
    }

private:
    storage_t& storage_;
};

}
}
