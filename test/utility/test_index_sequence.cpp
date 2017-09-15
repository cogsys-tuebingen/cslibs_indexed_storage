#include <cslibs_indexed_storage/utility/index_sequence.hpp>

#include <gtest/gtest.h>

using namespace cslibs_indexed_storage::utility;

TEST(TestUtilityIndexSequence, Static)
{
    static_assert(index_sequence<>::size() == 0, "Size incorrect");
    static_assert(index_sequence<1>::size() == 1, "Size incorrect");
    static_assert(index_sequence<1, 2>::size() == 2, "Size incorrect");
    static_assert(index_sequence<1, 2, 3>::size() == 3, "Size incorrect");

    static_assert(make_index_sequence<0>::size() == 0, "Size incorrect");
    static_assert(std::is_same<make_index_sequence<0>, index_sequence<>>::value, "Type incorrect");
    static_assert(make_index_sequence<1>::size() == 1, "Size incorrect");
    static_assert(std::is_same<make_index_sequence<1>, index_sequence<0>>::value, "Type incorrect");
    static_assert(make_index_sequence<2>::size() == 2, "Size incorrect");
    static_assert(std::is_same<make_index_sequence<2>, index_sequence<0, 1>>::value, "Type incorrect");
}
