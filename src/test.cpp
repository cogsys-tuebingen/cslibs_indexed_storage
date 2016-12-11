#include <cslibs_clustering/storage.hpp>
#include <cslibs_clustering/backend/nested_component_map.hpp>
#include <cslibs_clustering/backend/kdtree.hpp>
#include <cslibs_clustering/support/stl.hpp>
#include <cslibs_clustering/operations/clustering.hpp>

#include <iostream>
#include <array>

namespace cc = cslibs_clustering;

namespace
{
struct DataType
{
    float x;
    float y;

    void merge(const DataType& data) {
        std::cout << "merge" << std::endl;
    }
};

using IndexType = std::array<int, 2>;

struct ClusterOp
{
    bool start(const DataType& data)
    {
        std::cout << "Start cluster" << std::endl;
        return true;
    }

    bool extend(const DataType& data)
    {
        std::cout << "Extend cluster" << std::endl;
        return true;
    }

    template<typename visitor_t>
    void visit_neighbours(const visitor_t& visitior)
    {
        std::cout << "Visit" << std::endl;
    }
};

}



int main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    using Storage =
    cc::Storage<DataType, IndexType,
                cc::backend::kdtree::KDTree,
                cc::backend::options::split_value_type<double>,
                cc::backend::options::on_duplicate_index<cc::backend::options::OnDuplicateIndex::MERGE>
    >;
//    cc::Storage<DataType, IndexType,
//                cc::backend::simple::NestedComponentMap,
//                cc::backend::kdtree::options::split_value_type<float>
//    >;

    Storage storage;

    {
        std::array<std::pair<int, int>, 4> offsets =
                {{
                         { -2, -2 }, { -2, 2 }, { 2, -2 }, { 2, 2 }
                 }};

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> distribution(0, 1);
        for (auto i = 0; i < 100; ++i)
            for (auto offset : offsets)
            {
                DataType d = {distribution(gen) + std::get<0>(offset),
                              distribution(gen) + std::get<1>(offset)};

                storage.insert({d.x, d.y}, std::move(d));
            }
    }

    cc::operations::Clustering<Storage> clustering(storage);
    ClusterOp op;
    clustering.cluster(op);
}
