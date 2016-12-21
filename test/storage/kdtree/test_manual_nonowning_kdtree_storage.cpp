#include <cslibs_indexed_storage/storage.hpp>
#include <cslibs_indexed_storage/backend/kdtree/kdtree.hpp>
#include <cslibs_indexed_storage_test/testing.hpp>

namespace cc = cslibs_indexed_storage;
// -----------------------------------------------------------

template<cc::option::MergeStrategy strategy>
class Manual_NonOwning_KDTree_Storage : public ::testing::Test, public test::Verifier
{
public:
    using Storage = cc::Storage<
            cc::interface::non_owning<test::Data>, test::Index,
            cc::backend::kdtree::KDTree,
            cc::option::merge_strategy<strategy>
    >;

protected:
    Storage storage;
    std::vector<test::Data> data;
};

using Manual_NonOwning_KDTree_Merge_Storage_Test   = Manual_NonOwning_KDTree_Storage<cc::option::MergeStrategy::MERGE>;
using Manual_NonOwning_KDTree_Replace_Storage_Test = Manual_NonOwning_KDTree_Storage<cc::option::MergeStrategy::REPLACE>;

// -----------------------------------------------------------
TEST_F(Manual_NonOwning_KDTree_Merge_Storage_Test, InsertAndMerge)
{
    auto verifier = test::insert_nonowning_data(storage, data);

    ASSERT_EQ(verifier->merges.size(), 2);        // 2 nodes have merges
    ASSERT_EQ(verifier->merges[1].size(), 1);     // node[1] has 1 merge
    ASSERT_CONTAINS(verifier->merges[1], 2);      //      with node[2]
    ASSERT_EQ(verifier->merges[5].size(), 2);     // node[2] has 2 merges
    ASSERT_CONTAINS(verifier->merges[5], 6);      //      with node[6]
    ASSERT_CONTAINS(verifier->merges[5], 7);      //      with node[7]
}
TEST_F(Manual_NonOwning_KDTree_Replace_Storage_Test, InsertAndReplace)
{
    auto verifier = test::insert_nonowning_data(storage, data);

    ASSERT_EQ(verifier->merges.size(), 0);        // 0 merges
}
