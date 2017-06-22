#ifndef CIS_USING_ROS_TEST

#include <cslibs_indexed_storage/utility/concept_check.hpp>

#include <cslibs_indexed_storage/utility/void_t.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace cslibs_indexed_storage::utility;

TEST(TestConceptCheck, Static)
{
    static_assert(std::is_base_of<feature_missing_tag, feature_exists_tag>::value, "Type hierarchy invalid");
    static_assert(std::is_same<check_feature_exists, feature_exists_tag>::value, "Type invalid");
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
    auto do_test(feature_exists_tag, T&&) -> void_t<typename T::feature>
    {
        ConditionTrue();
    }

    template<typename T>
    void do_test(feature_missing_tag, T&&)
    {
        ConditionFalse();
    }

    template<typename T>
    auto test(T&& t) -> decltype(do_test(check_feature_exists{}, std::forward<T>(t)))
    {
        return do_test(check_feature_exists{}, std::forward<T>(t));
    }
};

}

TEST(TestConceptCheck, OverloadResolution)
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
