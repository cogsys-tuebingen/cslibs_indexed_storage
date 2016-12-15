#include <cslibs_clustering/index/std/array.hpp>
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


namespace cc = cslibs_clustering;


template<typename T>
struct SupportStlArrayTest : public ::testing::Test {};

using TestTypes = ::testing::Types<int, std::size_t, float>;
TYPED_TEST_CASE(SupportStlArrayTest, TestTypes);

TYPED_TEST(SupportStlArrayTest, Creation)
{
    using array = std::array<TypeParam, 3>;
    using type = cc::support::index_wrapper<array>;

    STATIC_ASSERT_EQ_TYPE(array, typename type::type);
    STATIC_ASSERT_EQ_VALUE(type::dimensions, 3);
}

TYPED_TEST(SupportStlArrayTest, GetIntegral)
{
    using array = std::array<TypeParam, 3>;
    using type = cc::support::index_wrapper<array>;

    array value{ TypeParam(1), TypeParam(2), TypeParam(3) };
    ASSERT_EQ(type::template get<0>::access(value), TypeParam(1));
    ASSERT_EQ(type::template get<1>::access(value), TypeParam(2));
    ASSERT_EQ(type::template get<2>::access(value), TypeParam(3));
}

TYPED_TEST(SupportStlArrayTest, SetIntegal)
{
    using array = std::array<int, 3>;
    using type = cc::support::index_wrapper<array>;

    array value{};

    type::template get<0>::access(value) = 1;
    type::template get<1>::access(value) = 2;
    type::template get<2>::access(value) = 3;

    ASSERT_EQ(type::template get<0>::access(value), 1);
    ASSERT_EQ(type::template get<1>::access(value), 2);
    ASSERT_EQ(type::template get<2>::access(value), 3);
}

TEST(SupportStlArray, Add)
{
    using array = std::array<int, 3>;
    using type = cc::support::index_wrapper<array>;

    array value_a{};
    type::template get<0>::access(value_a) = 1;
    type::template get<1>::access(value_a) = 2;
    type::template get<2>::access(value_a) = 3;

    {
        array value_b{};
        type::template get<0>::access(value_b) = 4;
        type::template get<1>::access(value_b) = 5;
        type::template get<2>::access(value_b) = 6;

        auto result = type::add(value_a, value_b);

        STATIC_ASSERT_EQ_TYPE(decltype(result), type::type);
        ASSERT_EQ(type::template get<0>::access(result), 5);
        ASSERT_EQ(type::template get<1>::access(result), 7);
        ASSERT_EQ(type::template get<2>::access(result), 9);
    }

    {
        std::array<double, 3> value_b{4.5, 5.2, 6.6};

        auto result = type::add(value_a, value_b);

        STATIC_ASSERT_EQ_TYPE(decltype(result), type::type);
        ASSERT_EQ(type::template get<0>::access(result), 5);
        ASSERT_EQ(type::template get<1>::access(result), 7);
        ASSERT_EQ(type::template get<2>::access(result), 9);
    }
}
