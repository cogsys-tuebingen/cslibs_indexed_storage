#include <cslibs_indexed_storage/storage.hpp>
#include <cslibs_indexed_storage/backends.hpp>
#include <cslibs_indexed_storage_test/testing.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/contains.hpp>
#include <type_traits>

namespace cis = cslibs_indexed_storage;

namespace
{
struct dummy_data
{
    dummy_data() = default;
    dummy_data(int a, int b) : a(a), b(b) {};
    int a;
    int b;
};
using dummy_index = std::array<int, 2>;

template<template<typename, typename, typename...>class backend_t>
using storage_t = cis::Storage<dummy_data, dummy_index, backend_t,
        cis::option::merge_strategy<cis::option::MergeStrategy::REPLACE>,
        cis::option::array_size<11, 11>, cis::option::array_offset<int, -5, -5>>;

template<template<typename, typename, typename...>class backend_t>
using auto_index_storage_t = cis::AutoIndexStorage<dummy_data, backend_t,
        cis::option::merge_strategy<cis::option::MergeStrategy::REPLACE>,
        cis::option::array_size<11, 11>, cis::option::array_offset<int, -5, -5>>;
}

namespace cslibs_indexed_storage
{
template<>
struct auto_index<dummy_data>
{
    using index_t = std::array<int, 2>;

    inline index_t index(const dummy_data& data) const
    {
        return { int(data.a), int(data.b) };
    }
};
}

namespace std
{
template<>
struct hash<dummy_index>
{
    typedef dummy_index argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& s) const
    {
        result_type const h1 ( std::hash<int>{}(s[0]) );
        result_type const h2 ( std::hash<int>{}(s[1]) );
        return h1 ^ (h2 << 1);
    }
};
}

using AllTypesStorage = ::testing::Types<
        storage_t<cis::backend::kdtree::KDTree>,
        storage_t<cis::backend::kdtree::KDTreeBuffered>,
        storage_t<cis::backend::array::Array>,
        storage_t<cis::backend::simple::Map>,
        storage_t<cis::backend::simple::UnorderedMap>,
        storage_t<cis::backend::simple::UnorderedComponentMap>
>;

using AllTypesAutoIndexStorage = ::testing::Types<
        auto_index_storage_t<cis::backend::kdtree::KDTree>,
        auto_index_storage_t<cis::backend::kdtree::KDTreeBuffered>,
        auto_index_storage_t<cis::backend::array::Array>,
        auto_index_storage_t<cis::backend::simple::Map>,
        auto_index_storage_t<cis::backend::simple::UnorderedMap>,
        auto_index_storage_t<cis::backend::simple::UnorderedComponentMap>
>;

using FixedCapacityTypes = boost::mpl::vector<
        storage_t<cis::backend::array::Array>,
        auto_index_storage_t<cis::backend::array::Array>
>;

template<typename T>
class TestBackendIntegration : public ::testing::Test
{};

TYPED_TEST_CASE_P(TestBackendIntegration);

TYPED_TEST_P(TestBackendIntegration, size)
{
    TypeParam storage;

    EXPECT_EQ(storage.size(), std::size_t(0));
    storage.insert({1, 1});
    EXPECT_EQ(storage.size(), std::size_t(1));
    storage.insert({0, 1});
    EXPECT_EQ(storage.size(), std::size_t(2));

    // duplicate index should not increase size
    storage.insert({0, 0});
    EXPECT_EQ(storage.size(), std::size_t(3));
    storage.insert({0, 0});
    EXPECT_EQ(storage.size(), std::size_t(3));


    storage.clear();
    EXPECT_EQ(storage.size(), std::size_t(0));
    storage.insert({0, 0});
    EXPECT_EQ(storage.size(), std::size_t(1));
}

TYPED_TEST_P(TestBackendIntegration, capacity)
{
    TypeParam storage;
    if (boost::mpl::contains<FixedCapacityTypes, TypeParam>::value)
        EXPECT_EQ(storage.capacity(), std::size_t(11 * 11));
    else
        EXPECT_EQ(storage.capacity(), std::numeric_limits<std::size_t>::max());

    // capacity should remain unchanged on clear
    storage.clear();
    if (boost::mpl::contains<FixedCapacityTypes, TypeParam>::value)
        EXPECT_EQ(storage.capacity(), std::size_t(11 * 11));
    else
        EXPECT_EQ(storage.capacity(), std::numeric_limits<std::size_t>::max());
}

REGISTER_TYPED_TEST_CASE_P(TestBackendIntegration, size, capacity);
INSTANTIATE_TYPED_TEST_CASE_P(Storage, TestBackendIntegration, AllTypesStorage);
INSTANTIATE_TYPED_TEST_CASE_P(AutoIndexStorage, TestBackendIntegration, AllTypesAutoIndexStorage);
