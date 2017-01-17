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

TEST(TestInterfaceDataDense, typeCheck)
{
    using hinted_type = cis::interface::dense<TestData>;

    ::testing::StaticAssertTypeEq<hinted_type::type, TestData>();

    using interface = cis::interface::data_interface<hinted_type>;

    ::testing::StaticAssertTypeEq<interface::hinted_type, hinted_type>();
    ::testing::StaticAssertTypeEq<interface::base_type, TestData>();
    ::testing::StaticAssertTypeEq<interface::storage_type, TestData>();
    ::testing::StaticAssertTypeEq<interface::input_type, TestData>();
    ::testing::StaticAssertTypeEq<interface::output_type, TestData>();
}

TEST(TestInterfaceDataDense, create)
{
    using interface = cis::interface::data_interface<cis::interface::dense<TestData>>;

    // by default construction
    {
        auto val = interface::create();
        EXPECT_EQ(val.str, "");
        EXPECT_EQ(val.value, 0);
    }
    // by variadic construction
    {
        auto val = interface::create(std::string("test"), 1);
        EXPECT_EQ(val.str, "test");
        EXPECT_EQ(val.value, 1);
    }
    // by copy constructor
    {
        TestData data{"test2", 2};
        auto val = interface::create(data);
        EXPECT_EQ(val.str, "test2");
        EXPECT_EQ(val.value, 2);
    }
    // by move constructor
    {
        auto val = interface::create(TestData{"test3", 3});
        EXPECT_EQ(val.str, "test3");
        EXPECT_EQ(val.value, 3);
    }
}

TEST(TestInterfaceDataDense, expose)
{
    using interface = cis::interface::data_interface<cis::interface::dense<TestData>>;

    // non-const
    {
        auto val = interface::create(std::string("test"), 1);
        auto& expose = interface::expose(val);
        EXPECT_EQ(expose.str, "test");
        EXPECT_EQ(expose.value, 1);

        expose.value = 2;
        EXPECT_EQ(val.value, 2);

        val.str = "changed";
        EXPECT_EQ(expose.str, "changed");
    }
    // const
    {
        const auto val = interface::create(std::string("test"), 1);
        const auto& expose = interface::expose(val);
        EXPECT_EQ(expose.str, "test");
        EXPECT_EQ(expose.value, 1);
    }
}

TEST(TestInterfaceDataDense, mergeReplace)
{
    using interface = cis::interface::data_interface<cis::interface::dense<TestData>>;

    // by default construction
    {
        auto val = interface::create(std::string("test"), 1);
        interface::merge<cis::option::MergeStrategy::REPLACE>(val);
        EXPECT_EQ(val.str, "");
        EXPECT_EQ(val.value, 0);
    }
    // by variadic construction
    {
        auto val = interface::create(std::string("test"), 1);
        interface::merge<cis::option::MergeStrategy::REPLACE>(val, "changed", 1);
        EXPECT_EQ(val.str, "changed");
        EXPECT_EQ(val.value, 1);
    }
    // by copy constructor
    {
        auto val = interface::create(std::string("test"), 1);
        TestData data{"changed", 1};
        interface::merge<cis::option::MergeStrategy::REPLACE>(val, data);
        EXPECT_EQ(val.str, "changed");
        EXPECT_EQ(val.value, 1);
    }
    // by move constructor
    {
        auto val = interface::create(std::string("test"), 1);
        interface::merge<cis::option::MergeStrategy::REPLACE>(val, TestData{"changed", 1});
        EXPECT_EQ(val.str, "changed");
        EXPECT_EQ(val.value, 1);
    }
}

TEST(TestInterfaceDataDense, mergeMerge)
{
    using interface = cis::interface::data_interface<cis::interface::dense<TestData>>;

    // by default construction
    {
        auto val = interface::create(std::string("test"), 1);
        interface::merge<cis::option::MergeStrategy::MERGE>(val);
        EXPECT_EQ(val.str, "test");
        EXPECT_EQ(val.value, 1);
    }
    // by variadic construction
    {
        auto val = interface::create(std::string("test"), 1);
        interface::merge<cis::option::MergeStrategy::MERGE>(val, "changed", 1);
        EXPECT_EQ(val.str, "testchanged");
        EXPECT_EQ(val.value, 2);
    }
    // by copy constructor
    {
        auto val = interface::create(std::string("test"), 1);
        TestData data{"changed", 1};
        interface::merge<cis::option::MergeStrategy::MERGE>(val, data);
        EXPECT_EQ(val.str, "testchanged");
        EXPECT_EQ(val.value, 2);
    }
    // by move constructor
    {
        auto val = interface::create(std::string("test"), 1);
        interface::merge<cis::option::MergeStrategy::MERGE>(val, TestData{"changed", 1});
        EXPECT_EQ(val.str, "testchanged");
        EXPECT_EQ(val.value, 2);
    }
    // special
    {
        auto val = interface::create(std::string("test"), 1);
        interface::merge<cis::option::MergeStrategy::MERGE>(val, 10.0);
        EXPECT_EQ(val.str, "special");
        EXPECT_EQ(val.value, 1);
    }
}