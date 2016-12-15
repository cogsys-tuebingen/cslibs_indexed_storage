#include <cslibs_clustering/storage.hpp>
#include <cslibs_clustering/backend/simple/unordered_component_map.hpp>
#include <cslibs_clustering/backend/kdtree/kdtree.hpp>
#include <cslibs_clustering/backend/array/array.hpp>
#include <cslibs_clustering/index/index_std.hpp>
#include <cslibs_clustering/operations/clustering.hpp>

#include <iostream>

namespace cc = cslibs_clustering;

namespace
{
struct DataType
{
    float x;
    float y;
    bool seen;

    void merge(const DataType& /*data*/) {
        std::cout << "merge" << std::endl;
    }
};

using IndexType = std::array<int, 2>;

struct ClusterOp
{
    bool start(const IndexType& index, DataType& data)
    {
        if (data.seen)
            return false;
        data.seen = true;
        std::cout << "Start cluster: " << index[0] << ", " << index[1] << std::endl;
        return true;
    }

    bool extend(const IndexType& center, const IndexType& index, DataType& data)
    {
        if (data.seen)
            return false;
        data.seen = true;
        std::cout << "Extend cluster: " << index[0] << ", " << index[1] << " (around " << center[0] << ", " << center[1] << ")" << std::endl;
        return true;
    }

    using neighborhood_t = cc::operations::clustering::GridNeighborhoodStatic<std::tuple_size<IndexType>::value, 3>;
    using visitor_index_t = neighborhood_t::offset_t;

    template<typename visitor_t>
    void visit_neighbours(const IndexType& center, const visitor_t& visitior)
    {
        static constexpr auto neighborhood = neighborhood_t{};
        std::cout << "Visit: " << center[0] << ", " << center[1] << std::endl;
        neighborhood.visit(visitior);
    }
};

}



int main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    using Storage =
//    cc::Storage<DataType, IndexType,
//                cc::backend::kdtree::KDTree,
//                cc::backend::options::split_value_type<double>,
//                cc::backend::options::on_duplicate_index<cc::backend::options::OnDuplicateIndex::MERGE>
//    >;
//    cc::Storage<DataType, IndexType,
//                cc::backend::simple::UnorderedComponentMap,
//                cc::backend::options::on_duplicate_index<cc::backend::options::OnDuplicateIndex::MERGE>
//    >;
    cc::Storage<DataType, IndexType,
                cc::backend::array::Array,
                cc::backend::options::on_duplicate_index<cc::backend::options::OnDuplicateIndex::MERGE>,
                cc::backend::options::array_size<10, 10>,
                cc::backend::options::array_offset<int, -5, -5>
    >;

    Storage storage;
    storage.set<cc::backend::options::tags::array_size>(10ul, 10ul);
    storage.set<cc::backend::options::tags::array_offset>(-5, -5);

    {
        std::array<std::pair<int, int>, 4> offsets =
                {{
                         { -2, -2 }, { -2, -1 }, { -1, -2 }, { 2, 2 }
                 }};

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> distribution(0, 1);
        for (auto i = 0; i < 100; ++i)
            for (auto offset : offsets)
            {
                DataType d;
                d.x = distribution(gen) + std::get<0>(offset);
                d.y = distribution(gen) + std::get<1>(offset);
                d.seen = false;

                storage.insert({int(d.x), int(d.y)}, std::move(d));
            }
    }

    cc::operations::clustering::Clustering<Storage> clustering(storage);
    ClusterOp op;
    clustering.cluster(op);
}
