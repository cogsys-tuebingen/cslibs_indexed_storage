#include <cslibs_indexed_storage/storage.hpp>
#include <cslibs_indexed_storage/backend/kdtree/kdtree.hpp>
#include <cslibs_indexed_storage_test/testing.hpp>

namespace cc = cslibs_indexed_storage;

// -----------------------------------------------------------
template<cc::option::MergeStrategy strategy>
class Manaul_Owning_KDTree_Storage : public ::testing::Test, public test::Verifier
{
public:
    using Storage = cc::Storage<
            test::Data, test::Index,
            cc::backend::kdtree::KDTree,
            cc::option::split_value_type<float>,
            cc::option::merge_strategy<strategy>
    >;

protected:
    Storage storage;
};

using Manual_Owning_KDTree_Merge_Storage_Test   = Manaul_Owning_KDTree_Storage<cc::option::MergeStrategy::MERGE>;
using Manual_Owning_KDTree_Replace_Storage_Test = Manaul_Owning_KDTree_Storage<cc::option::MergeStrategy::REPLACE>;

// -----------------------------------------------------------
TEST_F(Manual_Owning_KDTree_Merge_Storage_Test, InsertAndMerge)
{
    auto verifier = test::insert_owning_data(storage);

    ASSERT_EQ(verifier->merges.size(), 2);        // 2 nodes have merges
    ASSERT_EQ(verifier->merges[1].size(), 1);     // node[1] has 1 merge
    ASSERT_CONTAINS(verifier->merges[1], 2);      //      with node[2]
    ASSERT_EQ(verifier->merges[5].size(), 2);     // node[2] has 2 merges
    ASSERT_CONTAINS(verifier->merges[5], 6);      //      with node[6]
    ASSERT_CONTAINS(verifier->merges[5], 7);      //      with node[7]
}
TEST_F(Manual_Owning_KDTree_Replace_Storage_Test, InsertAndReplace)
{
    auto verifier = test::insert_owning_data(storage);

    ASSERT_EQ(verifier->merges.size(), 0);        // 0 nodes have merges
}
