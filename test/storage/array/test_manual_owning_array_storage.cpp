#include <cslibs_indexed_storage/storage.hpp>
#include <cslibs_indexed_storage/backend/array/array.hpp>
#include <cslibs_indexed_storage_test/testing.hpp>

namespace cc = cslibs_indexed_storage;
// -----------------------------------------------------------

template<cc::option::MergeStrategy strategy>
class Manual_Owning_Array_Storage : public ::testing::Test
{
public:
    using Storage = cc::Storage<
            test::Data, test::Index,
            cc::backend::array::Array,
            cc::option::array_size<10, 10>,
            cc::option::array_offset<test::Index::value_type, -5, -5>,
            cc::option::merge_strategy<strategy>
    >;

protected:
    Storage storage;
};

using Manual_Owning_Array_Merge_Storage_Test   = Manual_Owning_Array_Storage<cc::option::MergeStrategy::MERGE>;
using Manual_Owning_Array_Replace_Storage_Test = Manual_Owning_Array_Storage<cc::option::MergeStrategy::REPLACE>;

// -----------------------------------------------------------
TEST_F(Manual_Owning_Array_Merge_Storage_Test, InsertAndMerge)
{
    auto verifier = test::insert_owning_data(storage);

    ASSERT_EQ(verifier->merges.size(), 2);        // 2 nodes have merges
    ASSERT_EQ(verifier->merges[1].size(), 1);     // node[1] has 1 merge
    ASSERT_CONTAINS(verifier->merges[1], 2);      //      with node[2]
    ASSERT_EQ(verifier->merges[5].size(), 2);     // node[2] has 2 merges
    ASSERT_CONTAINS(verifier->merges[5], 6);      //      with node[6]
    ASSERT_CONTAINS(verifier->merges[5], 7);      //      with node[7]
}
TEST_F(Manual_Owning_Array_Replace_Storage_Test, All)
{
    auto verifier = test::insert_owning_data(storage);

    ASSERT_EQ(verifier->merges.size(), 0);        // 0 nodes have merges
}
