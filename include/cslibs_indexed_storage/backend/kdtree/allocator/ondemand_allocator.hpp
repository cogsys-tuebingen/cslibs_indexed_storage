#pragma once

namespace cslibs_indexed_storage
{
namespace backend
{
namespace kdtree
{

namespace detail
{

template<class Node>
class OnDemandAllocator
{
public:
    inline Node* allocate()
    {
        return new Node();
    }

    inline void deallocate(Node*& node)
    {
        delete node;
        node = nullptr;
    }

    inline void clear() {}

    inline void reset() {}
};

}
}
}
}