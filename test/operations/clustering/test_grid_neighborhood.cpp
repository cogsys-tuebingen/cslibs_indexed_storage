#include <cslibs_indexed_storage/operations/clustering/grid_neighborhood.hpp>
#include <cslibs_indexed_storage_test/testing.hpp>

namespace cc = cslibs_indexed_storage::operations::clustering;

TEST(ClusteringGridNeighborhoodTest, Dummy)
{
    {
        using t = cc::GridNeighborhoodStatic<2, 3, true>;
        t value{};
        value.visit([](t::offset_t o) { std::cout << int(o[0]) << ", " << int(o[1]) << std::endl; });
        STATIC_ASSERT_EQ_VALUE(t::count, 8);
    }
    {
        using t = cc::GridNeighborhoodDynamic;
        t value{};
        value.set_dimensions(3);
        value.set_size(3);
        value.set_skip_self(true);
        value.visit([](t::offset_t o) { std::cout << int(o[0]) << ", " << int(o[1]) << ", " << int(o[2]) << std::endl; });
    }
}
