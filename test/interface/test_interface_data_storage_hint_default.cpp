#include <cslibs_indexed_storage/interface/data/data_storage_hints.hpp>
#include <cslibs_indexed_storage/interface/data/data_interface.hpp>
#include <cslibs_indexed_storage_test/testing.hpp>

namespace cis = cslibs_indexed_storage;

namespace
{
struct dummy {};
}

// All other checks are covered by the TestInterfaceDataDense tests
TEST(TestInterfaceDataDefault, typeCheck)
{
    using hinted_type = dummy;
    using interface = cis::interface::data_interface<hinted_type>;

    static_assert(std::is_base_of<cis::interface::dense_data_interface<dummy>, interface>::value, "type error");
}
