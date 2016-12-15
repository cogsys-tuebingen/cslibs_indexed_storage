#include <cslibs_clustering/operations/clustering/grid_neighborhood.hpp>
#include <gtest/gtest.h>

#define STATIC_ASSERT_EQ_TYPE(T1, T2) \
    ::testing::StaticAssertTypeEq<T1, T2>()

/*
#define STATIC_ASSERT_EQ_VALUE(V1, V2) \
    ::testing::StaticAssertTypeEq< \
        std::integral_constant<typename std::common_type<decltype(V1), decltype(V2)>::type, V1>, \
        std::integral_constant<typename std::common_type<decltype(V1), decltype(V2)>::type, V2> \
    >()
*/

// needed to prevent odr-usage of values in gtest
#define STATIC_ASSERT_EQ_VALUE(V1, V2) \
    ASSERT_EQ( \
        (std::integral_constant<typename std::common_type<decltype(V1), decltype(V2)>::type, V1>::value), \
        (std::integral_constant<typename std::common_type<decltype(V1), decltype(V2)>::type, V2>::value) \
    )

namespace cc = cslibs_clustering::operations::clustering;

TEST(ClusteringGridNeighborhoodTest, Dummy)
{
    {
        using t = cc::GridNeighborhoodStatic<2, 3, true>;
        t value{};
        value.visit([](t::offset_t o) { std::cout << int(o[0]) << ", " << int(o[1]) << ", " << int(o[2]) << std::endl; });
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
