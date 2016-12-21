#include <cslibs_indexed_storage/interface/data/data_merge.hpp>
#include <cslibs_indexed_storage_test/testing.hpp>

namespace cc = cslibs_indexed_storage;

namespace
{
struct Info
{
    int copy_merge = 0;
    int move_merge = 0;
    int variadic_merge = 0;

};

struct MemberMerge : Info
{
    int member;

    MemberMerge() = delete;
    explicit MemberMerge(int member) : member(member) {}
    MemberMerge(const MemberMerge&) = delete;
    MemberMerge(MemberMerge&&) = delete;
    MemberMerge& operator=(const MemberMerge&) = delete;
    MemberMerge& operator=(MemberMerge&&) = delete;

    inline void merge(const MemberMerge&)
    {
        copy_merge++;
    }
    inline void merge(MemberMerge&&)
    {
        move_merge++;
    }
    inline void merge(int)
    {
        variadic_merge++;
    }
};

struct FreeFunctionMerge : Info
{
    int member;

    FreeFunctionMerge() = delete;
    FreeFunctionMerge(int member) : member(member) {}
    FreeFunctionMerge(const FreeFunctionMerge&) = delete;
    FreeFunctionMerge(FreeFunctionMerge&&) = delete;
    FreeFunctionMerge& operator=(const FreeFunctionMerge&) = delete;
    FreeFunctionMerge& operator=(FreeFunctionMerge&&) = delete;
};
}

namespace cslibs_indexed_storage
{
void merge(FreeFunctionMerge& self, const FreeFunctionMerge&)
{
    self.copy_merge++;
}
void merge(FreeFunctionMerge& self, FreeFunctionMerge&&)
{
    self.move_merge++;
}
void merge(FreeFunctionMerge& self, int)
{
    self.variadic_merge++;
}
}

TEST(InterfaceDataMerge, MemberObjectMerge)
{
    using Data = MemberMerge;
    // copy case (non-const)
    {
        Data base{1};
        Data other{2};
        cc::merge(base, other);
        EXPECT_EQ(base.copy_merge, 1);
        EXPECT_EQ(base.move_merge, 0);
        EXPECT_EQ(base.variadic_merge, 0);
    }
    // copy case (const)
    {
        Data base{1};
        const Data other{2};
        cc::merge(base, other);
        EXPECT_EQ(base.copy_merge, 1);
        EXPECT_EQ(base.move_merge, 0);
        EXPECT_EQ(base.variadic_merge, 0);
    }
    // move case (temporary)
    {
        Data base{1};
        cc::merge(base, Data{2});
        EXPECT_EQ(base.copy_merge, 0);
        EXPECT_EQ(base.move_merge, 1);
        EXPECT_EQ(base.variadic_merge, 0);
    }
    // move case (explicit)
    {
        Data base{1};
        Data other{2};
        cc::merge(base, std::move(other));
        EXPECT_EQ(base.copy_merge, 0);
        EXPECT_EQ(base.move_merge, 1);
        EXPECT_EQ(base.variadic_merge, 0);
    }
}

TEST(InterfaceDataMerge, MemberVariadicMerge)
{
    using Data = MemberMerge;
    // variadic case
    {
        Data base{1};
        cc::merge(base, 2);
        EXPECT_EQ(base.copy_merge, 0);
        EXPECT_EQ(base.move_merge, 0);
        EXPECT_EQ(base.variadic_merge, 1);
    }
}

TEST(InterfaceDataMerge, FreeFunctionObjectMerge)
{
    using Data = FreeFunctionMerge;
    // copy case (non-const)
    {
        Data base{1};
        Data other{2};
        cc::merge(base, other);
        EXPECT_EQ(base.copy_merge, 1);
        EXPECT_EQ(base.move_merge, 0);
        EXPECT_EQ(base.variadic_merge, 0);
    }
    // copy case (const)
    {
        Data base{1};
        const Data other{2};
        cc::merge(base, other);
        EXPECT_EQ(base.copy_merge, 1);
        EXPECT_EQ(base.move_merge, 0);
        EXPECT_EQ(base.variadic_merge, 0);
    }
    // move case (temporary)
    {
        Data base{1};
        cc::merge(base, Data{2});
        EXPECT_EQ(base.copy_merge, 0);
        EXPECT_EQ(base.move_merge, 1);
        EXPECT_EQ(base.variadic_merge, 0);
    }
    // move case (explicit)
    {
        Data base{1};
        Data other{2};
        cc::merge(base, std::move(other));
        EXPECT_EQ(base.copy_merge, 0);
        EXPECT_EQ(base.move_merge, 1);
        EXPECT_EQ(base.variadic_merge, 0);
    }
}

TEST(InterfaceDataMerge, FreeFunctionVariadicMerge)
{
    using Data = FreeFunctionMerge;
    // variadic case
    {
        Data base{1};
        cc::merge(base, 2);
        EXPECT_EQ(base.copy_merge, 0);
        EXPECT_EQ(base.move_merge, 0);
        EXPECT_EQ(base.variadic_merge, 1);
    }
}