#include <cslibs_clustering/storage.hpp>
#include <cslibs_clustering/backend/kdtree/kdtree.hpp>
#include <cslibs_clustering/backend/array/array.hpp>
#include <cslibs_clustering/backend/simple/unordered_component_map.hpp>
#include <cslibs_clustering/index/index_std.hpp>
#include <cslibs_clustering/operations/clustering.hpp>

#include <vector>
#include <iostream>
#include <iterator>
#include <map>


using namespace cslibs_clustering;
using namespace cslibs_clustering::backend;
using namespace cslibs_clustering::operations;

namespace
{

//! define the data
struct Data
{
    int cluster = -1;           //!< cluster index of this chunk
    std::vector<int> indices;   //!< indices of the points stored in this chunk

    Data() = default;           //!< \todo Data type must be default constructbile
    Data(int index)             //!< Actual data constructor
    {
        indices.push_back(index);
    }

    void merge(const Data& other)       //!< merge operation between old and to-be-inserted data
    {
        indices.insert(indices.end(), other.indices.begin(), other.indices.end());
    }
};

//! we simuate 2D points, discreticed by their integer value
using Index = std::array<int, 2>;

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


int main()
{
//    using StorageType = Storage<Data, Index, kdtree::KDTree, backend::options::on_duplicate_index<backend::options::OnDuplicateIndex::MERGE>>;
//    using StorageType = Storage<Data, Index, simple::UnorderedComponentMap, backend::options::on_duplicate_index<backend::options::OnDuplicateIndex::MERGE>>;
    using StorageType = Storage<Data, Index, array::Array, backend::options::on_duplicate_index<backend::options::OnDuplicateIndex::MERGE>, options::array_size<11, 11>, options::array_offset<int, -5, -5>>;

    // generate some test data
    StorageType storage;
    {
        int i = 0;

        // 3 points at (0, 0)
        storage.insert({0, 0}, i++);
        storage.insert({0, 0}, i++);
        storage.insert({0, 0}, i++);

        // 2 points at (0, 1), should be joined to (0,0)
        storage.insert({0, 1}, i++);
        storage.insert({0, 1}, i++);

        // 3 points at (2, 0), should be separted
        storage.insert({2, 0}, i++);
        storage.insert({2, 0}, i++);
        storage.insert({2, 0}, i++);

        // 1 point at (3, -1), should be joined to (2, 0)
        storage.insert({3, -1}, i++);
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