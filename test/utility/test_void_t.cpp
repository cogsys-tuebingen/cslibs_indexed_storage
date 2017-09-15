#ifndef CIS_USING_ROS_TEST

#include <cslibs_indexed_storage/utility/void_t.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace cslibs_indexed_storage::utility;

TEST(TestUtilityVoidT, Static)
{
    struct dummy {};

    static_assert(std::is_same<void_t<>, void>::value, "Type incorrect");
    static_assert(std::is_same<void_t<void>, void>::value, "Type incorrect");
    static_assert(std::is_same<void_t<dummy>, void>::value, "Type incorrect");
}

namespace
{
struct has_feature { using feature = void; };
struct no_feature { };

struct FeatureMock
{
    MOCK_METHOD0(ConditionTrue, void());
    MOCK_METHOD0(ConditionFalse, void());

    template<typename T>
    auto test(T&&) -> void_t<typename T::feature>
    {
        ConditionTrue();
    }

    void test(...)
    {
        ConditionFalse();
    }
};

}

TEST(TestUtilityVoidT, OverloadResolution)
{
    {
        FeatureMock mock;
        EXPECT_CALL(mock, ConditionTrue()).Times(1);
        EXPECT_CALL(mock, ConditionFalse()).Times(0);

        mock.test(has_feature{});
    }
    {
        FeatureMock mock;
        EXPECT_CALL(mock, ConditionTrue()).Times(0);
        EXPECT_CALL(mock, ConditionFalse()).Times(1);

        mock.test(no_feature{});
    }
}

#endif
