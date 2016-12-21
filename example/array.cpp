#include <cslibs_clustering/storage.hpp>
#include <cslibs_clustering/backend/array/array.hpp>
#include <cslibs_clustering/operations/clustering.hpp>

#include <vector>
#include <iostream>
#include <map>

using namespace cslibs_clustering;
using namespace cslibs_clustering::backend;
using namespace cslibs_clustering::operations;

namespace
{
//! we simuate 2D points, discreticed by their integer value
using Index = std::array<int, 2>;

//! define the data
struct Data
{
    Index index;
    int cluster = -1;           //!< cluster index of this chunk
    std::vector<int> indices;   //!< indices of the points stored in this chunk

    Data() = default;           //!< \todo Data type must be default constructbile
    Data(Index index, int id) :   //!< Actual data constructor
            index(index)
    {
        indices.push_back(id);
    }

    void merge(const Data& other)       //!< merge operation between old and to-be-inserted data
    {
        indices.insert(indices.end(), other.indices.begin(), other.indices.end());
    }
};

//! clustering method we are using
struct ClusterOp
{
    //! called when a new cluster should be started
    bool start(const Index&, Data& data)
    {
        if (data.cluster != -1)
            return false;

        current_cluster +=1;

        data.cluster = current_cluster;

        return true;
    }

    //! called when a cluster is extended due to found neighbors
    bool extend(const Index&, const Index&, Data& data)
    {
        if (data.cluster != -1)
            return false;

        data.cluster = current_cluster;
        return true;
    }

    //! used neighborhood, look at direct neighbors only
    using neighborhood_t = operations::clustering::GridNeighborhoodStatic<std::tuple_size<Index>::value, 3>;
    using visitor_index_t = neighborhood_t::offset_t;   //!< currently needed by the clustering API

    //! vistor implementation for neighbors
    template<typename visitor_t>
    void visit_neighbours(const Index&, const visitor_t& visitior)
    {
        static constexpr auto neighborhood = neighborhood_t{};
        neighborhood.visit(visitior);
    }

    int current_cluster = -1;   //!< keep track of the current cluster index
};
}

namespace cslibs_clustering
{
template<>
struct auto_index<Data>
{
    using index_t = std::array<int, 2>;

    inline index_t index(const Data& data) const
    {
        return data.index;
    }
};
}

int main()
{
    std::vector<Data> datas;
    {
        int i = 0;

        // 3 points at (0, 0)
        datas.emplace_back(Index{0, 0}, i++);
        datas.emplace_back(Index{0, 0}, i++);
        datas.emplace_back(Index{0, 0}, i++);

        // 2 points at (0, 1), should be joined to (0,0)
        datas.emplace_back(Index{0, 1}, i++);
        datas.emplace_back(Index{0, 1}, i++);

        // 3 points at (2, 0), should be separted
        datas.emplace_back(Index{2, 0}, i++);
        datas.emplace_back(Index{2, 0}, i++);
        datas.emplace_back(Index{2, 0}, i++);

        // 1 point at (3, -1), should be joined to (2, 0)
        datas.emplace_back(Index{3, -1}, i++);
    }

    using StorageType = AutoIndexStorage<interface::non_owning<Data>, array::Array>;

    // generate some test data
    StorageType storage;

    // dynamically calculate index bounds with storage.get_indexer();
    storage.set<option::tags::array_size>(10ul, 10ul);
    storage.set<option::tags::array_offset>(-5, -5);

    {
        for (auto& data : datas)
            storage.insert(&data);
    }

    // do the actual clustering
    ClusterOp clusters;
    {
        clustering::Clustering<StorageType> clusterer(storage);
        clusterer.cluster(clusters);
    }

    // print clustering results
    {
        std::cout << "Clusters Found: " << clusters.current_cluster + 1 << std::endl;

        // this map may be also generated in ClusterOp during start(...) and extend(...)
        std::map<int, std::vector<int>> cluster_indices;
        storage.traverse([&cluster_indices](const Index& index, const Data& data)
                         {
                             std::cout << "Entry at (" << index[0] << ", " << index[1] << "): indices = ";
                             std::copy(data.indices.begin(), data.indices.end(), std::ostream_iterator<int>(std::cout, ", "));
                             std::cout << " cluster = " << data.cluster << std::endl;

                             auto& cluster_idxs = cluster_indices[data.cluster];
                             cluster_idxs.insert(cluster_idxs.end(), data.indices.begin(), data.indices.end());
                         });

        for (const auto& entry : cluster_indices)
        {
            std::cout << "Cluster: index = " << entry.first << ", indices = ";
            std::copy(entry.second.begin(), entry.second.end(), std::ostream_iterator<int>(std::cout, ", "));
            std::cout << std::endl;
        }
    }
}
