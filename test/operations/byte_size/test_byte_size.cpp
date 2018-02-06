#include <cslibs_indexed_storage/storage.hpp>
#include <cslibs_indexed_storage/backend/array/array.hpp>
#include <cslibs_indexed_storage/backend/kdtree/kdtree.hpp>
#include <cslibs_indexed_storage_test/testing.hpp>

namespace cc = cslibs_indexed_storage;

namespace
{
struct DataNoHeap
{
    int value;
};

struct DataHeap
{
    DataHeap() = default;
    DataHeap(int v)
    {
        value.reserve(1);
        value.push_back(v);
    }

    std::size_t byte_size() const
    {
        return sizeof(*this)
               + sizeof(typename decltype(value)::value_type) * value.capacity();
    }

    std::vector<int> value;
};

void merge(DataNoHeap& a, DataNoHeap&& b) {}
void merge(DataHeap& a, DataHeap&& b)
{
    a.value.reserve(a.value.size() + b.value.size());
    a.value.insert(a.value.end(), b.value.begin(), b.value.end());
}

using Index = std::array<int, 2>;
}

class ByteSizeTest : public ::testing::Test
{};

TEST_F(ByteSizeTest, OwningArrayNoHeap)
{
    using Storage = cc::Storage<
            cc::interface::dense<DataNoHeap>,
            Index,
            cc::backend::array::Array,
            cc::option::array_size<8, 8>>;

    Storage storage;
    EXPECT_EQ(storage.byte_size(), 344);

    storage.insert({1, 1}, {0});
    EXPECT_EQ(storage.byte_size(), 344);
}

TEST_F(ByteSizeTest, OwningArrayHeap)
{
    using Storage = cc::Storage<
            cc::interface::dense<DataHeap>,
            Index,
            cc::backend::array::Array,
            cc::option::array_size<8, 8>>;

    Storage storage;
    EXPECT_EQ(storage.byte_size(), 1624);

    storage.insert({1, 1}, {1});
    EXPECT_EQ(storage.byte_size(), 1624 + 4);

    storage.insert({1, 1}, {1});
    EXPECT_EQ(storage.byte_size(), 1624 + 2 * 4);

    // this does not free the data...
    storage.clear();
    EXPECT_EQ(storage.byte_size(), 1624 + 2 * 4);
}

TEST_F(ByteSizeTest, OwningKDTreeNoHeap)
{
    using Storage = cc::Storage<
            cc::interface::dense<DataNoHeap>,
            Index,
            cc::backend::kdtree::KDTree>;

    Storage storage;
    EXPECT_EQ(storage.byte_size(), 24);

    storage.insert({1, 1}, {0});
    EXPECT_EQ(storage.byte_size(), 24 + 1 * 56);

    // merge --> unchanged
    storage.insert({1, 1}, {0});
    EXPECT_EQ(storage.byte_size(), 24 + 1 * 56);

    // new nodes
    storage.insert({1, 2}, {0});
    EXPECT_EQ(storage.byte_size(), 24 + 3 * 56);

    // new nodes
    storage.insert({1, 3}, {0});
    EXPECT_EQ(storage.byte_size(), 24 + 5 * 56);

    storage.clear();
    EXPECT_EQ(storage.byte_size(), 24);
}

TEST_F(ByteSizeTest, OwningKDTreeHeap)
{
    using Storage = cc::Storage<
            cc::interface::dense<DataHeap>,
            Index,
            cc::backend::kdtree::KDTree>;

    Storage storage;
    EXPECT_EQ(storage.byte_size(), 24);

    storage.insert({1, 1}, {0});
    EXPECT_EQ(storage.byte_size(), 24 + 1 * 72 + 1 * 4);

    // merge --> unchanged
    storage.insert({1, 1}, {0});
    EXPECT_EQ(storage.byte_size(), 24 + 1 * 72 + 2 * 4);

    // new nodes
    storage.insert({1, 2}, {0});
    EXPECT_EQ(storage.byte_size(), 24 + 3 * 72 + 3 * 4);

    // new nodes
    storage.insert({1, 3}, {0});
    EXPECT_EQ(storage.byte_size(), 24 + 5 * 72 + 4 * 4);

    storage.clear();
    EXPECT_EQ(storage.byte_size(), 24);
}
