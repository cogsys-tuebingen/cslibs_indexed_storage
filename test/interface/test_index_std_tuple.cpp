#include <cslibs_indexed_storage/interface/index/index_interface.hpp>
#include <cslibs_indexed_storage/interface/index/std/tuple.hpp>
#include <cslibs_indexed_storage_test/testing.hpp>

namespace cc = cslibs_indexed_storage;


template<typename T>
struct SupportStlTupleTest : public ::testing::Test {};

using TestTypes = ::testing::Types<int, std::size_t, float>;
TYPED_TEST_CASE(SupportStlTupleTest, TestTypes);

TYPED_TEST(SupportStlTupleTest, Creation)
{
    using array = std::tuple<TypeParam, TypeParam, TypeParam>;
    using type = cc::interface::index_interface<array>;

    STATIC_ASSERT_EQ_TYPE(array, typename type::type);
    STATIC_ASSERT_EQ_VALUE(type::dimensions, 3);
}

TYPED_TEST(SupportStlTupleTest, GetIntegral)
{
    using array = std::tuple<TypeParam, TypeParam, TypeParam>;
    using type = cc::interface::index_interface<array>;

    array value{ TypeParam(1), TypeParam(2), TypeParam(3) };
    ASSERT_EQ(type::template dimension<0>::access(value), TypeParam(1));
    ASSERT_EQ(type::template dimension<1>::access(value), TypeParam(2));
    ASSERT_EQ(type::template dimension<2>::access(value), TypeParam(3));
}

TYPED_TEST(SupportStlTupleTest, SetIntegal)
{
    using array = std::tuple<TypeParam, TypeParam, TypeParam>;
    using type = cc::interface::index_interface<array>;

    array value{};

    type::template dimension<0>::access(value) = 1;
    type::template dimension<1>::access(value) = 2;
    type::template dimension<2>::access(value) = 3;

    ASSERT_EQ(type::template dimension<0>::access(value), 1);
    ASSERT_EQ(type::template dimension<1>::access(value), 2);
    ASSERT_EQ(type::template dimension<2>::access(value), 3);
}

TEST(SupportStlTupleTest, Add)
{
    using array = std::tuple<int, int, int>;
    using type = cc::interface::index_interface<array>;

    array value_a{};
    type::template dimension<0>::access(value_a) = 1;
    type::template dimension<1>::access(value_a) = 2;
    type::template dimension<2>::access(value_a) = 3;

    {
        array value_b{};
        type::template dimension<0>::access(value_b) = 4;
        type::template dimension<1>::access(value_b) = 5;
        type::template dimension<2>::access(value_b) = 6;

        auto result = type::add(value_a, value_b);

        STATIC_ASSERT_EQ_TYPE(decltype(result), type::type);
        ASSERT_EQ(type::template dimension<0>::access(result), 5);
        ASSERT_EQ(type::template dimension<1>::access(result), 7);
        ASSERT_EQ(type::template dimension<2>::access(result), 9);
    }

    {
        std::array<double, 3> value_b{4.5, 5.2, 6.6};

        auto result = type::add(value_a, value_b);

        STATIC_ASSERT_EQ_TYPE(decltype(result), type::type);
        ASSERT_EQ(type::template dimension<0>::access(result), 5);
        ASSERT_EQ(type::template dimension<1>::access(result), 7);
        ASSERT_EQ(type::template dimension<2>::access(result), 9);
    }
}
