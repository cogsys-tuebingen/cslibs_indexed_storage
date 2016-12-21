#include <cslibs_indexed_storage/storage.hpp>
#include <cslibs_indexed_storage/backend/simple/unordered_component_map.hpp>
#include <cslibs_indexed_storage/backend/kdtree/kdtree.hpp>
#include <cslibs_indexed_storage/backend/array/array.hpp>
#include <cslibs_indexed_storage/interface/index/index_std.hpp>
#include <cslibs_indexed_storage/operations/clustering.hpp>




#include <iostream>

namespace cc = cslibs_indexed_storage;

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
//                cc::option::split_value_type<double>
//    >;
//    cc::Storage<DataType, IndexType,
//                cc::backend::simple::UnorderedComponentMap
//    >;
    cc::Storage<DataType, IndexType,
                cc::backend::array::Array,
                cc::option::array_size<10, 10>,
                cc::option::array_offset<int, -5, -5>
    >;

    Storage storage;
    storage.set<cc::option::tags::array_size>(10ul, 10ul);
    storage.set<cc::option::tags::array_offset>(-5, -5);

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
