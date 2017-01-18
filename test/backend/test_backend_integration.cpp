#include <cslibs_indexed_storage/storage.hpp>
#include <cslibs_indexed_storage/backends.hpp>
#include <cslibs_indexed_storage_test/testing.hpp>
#include <type_traits>

namespace cis = cslibs_indexed_storage;

namespace
{
struct dummy_data {};
using dummy_index = std::array<int, 2>;

template<template<typename, typename, typename...>class backend_t>
using storage_t = cis::Storage<dummy_data, dummy_index, backend_t,
        cis::option::merge_strategy<cis::option::MergeStrategy::REPLACE>,
        cis::option::array_size<11, 11>, cis::option::array_offset<int, -5, -5>>;
}

using AllBackendTypes = ::testing::Types<
        storage_t<cis::backend::kdtree::KDTree>,
        storage_t<cis::backend::kdtree::KDTreeBuffered>,
        storage_t<cis::backend::array::Array>,
        storage_t<cis::backend::simple::Map>,
        storage_t<cis::backend::simple::UnorderedMap>,
        storage_t<cis::backend::simple::UnorderedComponentMap>
>;

using CapacityBackendTypes = storage_t<cis::backend::array::Array>;

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

template<typename T>
class TestBackendIntegration : public ::testing::Test
{};

TYPED_TEST_CASE_P(TestBackendIntegration);

TYPED_TEST_P(TestBackendIntegration, size)
{
    TypeParam storage;
    EXPECT_EQ(storage.size(), std::size_t(0));
    storage.insert({0, 0});
    EXPECT_EQ(storage.size(), std::size_t(1));
    storage.insert({0, 0});
    EXPECT_EQ(storage.size(), std::size_t(1));
    storage.insert({1, 1});
    EXPECT_EQ(storage.size(), std::size_t(2));
    storage.insert({0, 1});
    EXPECT_EQ(storage.size(), std::size_t(3));
}

TYPED_TEST_P(TestBackendIntegration, capacity)
{
    TypeParam storage;
    if (std::is_same<CapacityBackendTypes, TypeParam>::value)
        EXPECT_EQ(storage.capacity(), std::size_t(11 * 11));
    else
        EXPECT_EQ(storage.capacity(), std::numeric_limits<std::size_t>::max());
}

REGISTER_TYPED_TEST_CASE_P(TestBackendIntegration, size, capacity);
INSTANTIATE_TYPED_TEST_CASE_P(Common, TestBackendIntegration, AllBackendTypes);
