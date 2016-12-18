#include "testdata.hpp"
#include "timing.hpp"

#include <cslibs_clustering/storage.hpp>
#include <cslibs_clustering/backend/kdtree/kdtree.hpp>
#include <cslibs_clustering/backend/array/array.hpp>
#include <cslibs_clustering/backend/simple/unordered_component_map.hpp>
#include <cslibs_clustering/interface/index/index_std.hpp>
#include <cslibs_clustering/operations/clustering.hpp>

#include <map>

namespace cc = cslibs_clustering;

namespace tests
{
    struct Data
    {
        int cluster = -1;
        std::vector<const data::Point*> samples;
        double weight;

        inline void merge(const Data& other)
        {
            samples.insert(samples.end(), other.samples.begin(), other.samples.end());
            weight += other.weight;
        }
    };
    inline Data create_data(const data::Point& point)
    {
        Data data;
        data.samples.emplace_back(&point);
        data.weight = point.weight;
        return data;
    }

    using Index = std::array<int, 3>;
    inline Index create_index(const data::Point& point)
    {
        static double bin_sizes[3] = {0.5, 0.5, (10 * M_PI / 180.0)};
        return { static_cast<int>(std::floor(point.x / bin_sizes[0])),
                 static_cast<int>(std::floor(point.y / bin_sizes[1])),
                 static_cast<int>(std::floor(point.z / bin_sizes[2])) };
    }

    struct ClusterOp
    {
        //! called when a new cluster should be started
        bool start(const Index&, Data& data)
        {
            if (data.cluster != -1)
                return false;

            current_cluster +=1;
            clusters.emplace(current_cluster, data.samples);

            data.cluster = current_cluster;

            return true;
        }

        //! called when a cluster is extended due to found neighbors
        bool extend(const Index&, const Index&, Data& data)
        {
            if (data.cluster != -1)
                return false;

            auto& cluster = clusters[current_cluster];
            cluster.insert(cluster.end(), data.samples.begin(), data.samples.end());

            data.cluster = current_cluster;
            return true;
        }

        //! used neighborhood, look at direct neighbors only
        using neighborhood_t = cc::operations::clustering::GridNeighborhoodStatic<std::tuple_size<Index>::value, 3>;
        using visitor_index_t = neighborhood_t::offset_t;   //!< currently needed by the clustering API

        //! vistor implementation for neighbors
        template<typename visitor_t>
        void visit_neighbours(const Index&, const visitor_t& visitior)
        {
            static constexpr auto neighborhood = neighborhood_t{};
            neighborhood.visit(visitior);
        }

        int current_cluster = -1;   //!< keep track of the current cluster index
        std::unordered_map<int, std::vector<const data::Point*>> clusters;
    };


    template<typename Storage>
    void create(const data::Points& samples)
    {
        Storage storage;

        for (const data::Point& sample : samples)
        {
            storage.insert(create_index(sample), create_data(sample));
        }
    }

    template<typename Storage>
    void cluster(const data::Points& samples)
    {
        Storage storage;

        for (const data::Point& sample : samples)
            storage.insert(create_index(sample), create_data(sample));

        ClusterOp clusters;
        cc::operations::clustering::Clustering<Storage> clusterer(storage);
        clusterer.cluster(clusters);
//        printf("%lu", clusters.clusters.size());
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <sample_data>" << std::endl;
        return 1;
    }

    std::string path = argv[1];
    data::Points points = data::getTestdata(path);
    data::Points points_small = data::getTestdataSmall(path);
    std::cout << "Testdata : " << std::endl
              << "\tFile   : " << path << std::endl
              << "\tSamples: " << points.size() << " / " << points_small.size() << std::endl
              << std::endl;

    using kd_tree = cc::Storage<
            tests::Data, tests::Index,
            cc::backend::kdtree::KDTree
    >;
    using array = cc::Storage<
        tests::Data, tests::Index,
        cc::backend::array::Array,
        cc::option::array_size<100ul, 100ul, 100ul>,
        cc::option::array_offset<int, -50, -50, -50>
    >;
    using paged = cc::Storage<
        tests::Data, tests::Index,
        cc::backend::simple::UnorderedComponentMap
    >;

    std::cout << "Create:" << std::endl;
    timing::Benchmark::timing<500>("\tkd-tree", std::bind(&tests::create<kd_tree>, std::cref(points)));
    timing::Benchmark::timing<500>("\tarray  ", std::bind(&tests::create<array>, std::cref(points)));
    timing::Benchmark::timing<500>("\tpaged  ", std::bind(&tests::create<paged>, std::cref(points)));

    std::cout << "Create & Cluster:" << std::endl;
    timing::Benchmark::timing<500>("\tkd-tree", std::bind(&tests::cluster<kd_tree>, std::cref(points)));
    timing::Benchmark::timing<500>("\tarray  ", std::bind(&tests::cluster<array>, std::cref(points)));
    timing::Benchmark::timing<500>("\tpaged  ", std::bind(&tests::cluster<paged>, std::cref(points)));
}