#include <cslibs_indexed_storage/interface/data/data_storage_hints.hpp>
#include <cslibs_indexed_storage/interface/data/data_interface.hpp>
#include <cslibs_indexed_storage_test/testing.hpp>

namespace cis = cslibs_indexed_storage;

namespace
{
struct TestData : Tracker
{
    TestData() = default;
    TestData(const std::string& a, int b) : str(a), value(b) {}
    std::string str{""};
    int value{0};

    inline void merge(const TestData& other)
    {
        str += other.str;
        value += other.value;
    }

    inline void merge(double)
    {
        str = "special";
    }
};
}

TEST(TestInterfaceDataNonOwning, typeCheck)
{
    using hinted_type = cis::interface::non_owning<TestData>;

    ::testing::StaticAssertTypeEq<hinted_type::type, TestData>();

    using interface = cis::interface::data_interface<hinted_type>;

    ::testing::StaticAssertTypeEq<interface::hinted_type, hinted_type>();
    ::testing::StaticAssertTypeEq<interface::base_type, TestData>();
    ::testing::StaticAssertTypeEq<interface::storage_type, TestData*>();
    ::testing::StaticAssertTypeEq<interface::input_type, TestData*>();
    ::testing::StaticAssertTypeEq<interface::output_type, TestData>();
}

TEST(TestInterfaceDataNonOwning, create)
{
    using interface = cis::interface::data_interface<cis::interface::non_owning<TestData>>;

    // nullptr
    {
        auto val = interface::create(nullptr);
        EXPECT_EQ(val, nullptr);
    }
    // value
    {
        TestData data{ "test", 1 };
        auto val = interface::create(&data);
        EXPECT_EQ(val->str, "test");
        EXPECT_EQ(val->value, 1);
    }
}

TEST(TestInterfaceDataNonOwning, expose)
{
    using interface = cis::interface::data_interface<cis::interface::non_owning<TestData>>;

    // non-const
    {
        TestData data{ "test", 1 };
        auto val = interface::create(&data);
        auto& expose = interface::expose(val);
        EXPECT_EQ(expose.str, "test");
        EXPECT_EQ(expose.value, 1);

        expose.value = 2;
        EXPECT_EQ(val->value, 2);

        val->str = "changed";
        EXPECT_EQ(expose.str, "changed");
    }
    // const
    {
        TestData data{ "test", 1 };
        const auto val = interface::create(&data);
        const auto& expose = interface::expose(val);
        EXPECT_EQ(expose.str, "test");
        EXPECT_EQ(expose.value, 1);
    }
}

TEST(TestInterfaceDataNonOwning, mergeReplace)
{
    using interface = cis::interface::data_interface<cis::interface::non_owning<TestData>>;

    {
        TestData data{ "test", 1 };
        auto val = interface::create(&data);

        TestData replace{"changed", 1};
        interface::merge<cis::option::MergeStrategy::REPLACE>(val, &replace);
        EXPECT_EQ(val->str, "changed");
        EXPECT_EQ(val->value, 1);
    }
}

TEST(TestInterfaceDataNonOwning, mergeMerge)
{
    using interface = cis::interface::data_interface<cis::interface::non_owning<TestData>>;

    // by copy constructor
    {
        TestData data{ "test", 1 };
        auto val = interface::create(&data);

        TestData replace{"changed", 1};
        interface::merge<cis::option::MergeStrategy::MERGE>(val, &replace);
        EXPECT_EQ(val->str, "testchanged");
        EXPECT_EQ(val->value, 2);
    }
    // special
    {
        TestData data{ "test", 1 };
        auto val = interface::create(&data);
        interface::merge<cis::option::MergeStrategy::MERGE>(val, 10.0);
        EXPECT_EQ(val->str, "special");
        EXPECT_EQ(val->value, 1);
    }
}
