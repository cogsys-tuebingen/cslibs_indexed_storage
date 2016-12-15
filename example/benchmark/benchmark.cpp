#include "testdata.hpp"
#include "timing.hpp"

#include <cslibs_clustering/storage.hpp>
#include <cslibs_clustering/backend/kdtree/kdtree.hpp>
#include <cslibs_clustering/backend/array/array.hpp>
#include <cslibs_clustering/backend/simple/nested_component_map.hpp>
#include <cslibs_clustering/index/index_std.hpp>
namespace cc = cslibs_clustering;

namespace tests
{
    struct Data
    {
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


    template<typename Storage>
    void create(const data::Points& samples)
    {
        Storage storage;

        for (const data::Point& sample : samples)
        {
            storage.insert(create_index(sample), create_data(sample));
        }
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
            cc::backend::kdtree::KDTree,
            cc::backend::options::on_duplicate_index<cc::backend::options::OnDuplicateIndex::MERGE>
    >;
    using array = cc::Storage<
        tests::Data, tests::Index,
        cc::backend::array::Array,
        cc::backend::options::on_duplicate_index<cc::backend::options::OnDuplicateIndex::MERGE>,
        cc::backend::options::array_size<100ul, 100ul, 100ul>,
        cc::backend::options::array_offset<int, -50, -50, -50>,
        cc::backend::options::array_dynamic_only<false>
    >;
    using paged = cc::Storage<
        tests::Data, tests::Index,
        cc::backend::simple::NestedComponentMap,
        cc::backend::options::on_duplicate_index<cc::backend::options::OnDuplicateIndex::MERGE>
    >;

    timing::Benchmark::timing<500>("kd-tree", std::bind(&tests::create<kd_tree>, std::cref(points)));
    timing::Benchmark::timing<500>("array  ", std::bind(&tests::create<array>, std::cref(points)));
    timing::Benchmark::timing<500>("paged  ", std::bind(&tests::create<paged>, std::cref(points)));
}