#pragma once

#include <cslibs_indexed_storage/interface/data/align/aligned_allocator.hpp>

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
        Node* node = allocator_.allocate(1);
        new (node) Node();
        return node;
    }

    inline void deallocate(Node*& node)
    {
        if(node != nullptr) {
            (*node).~Node();
            allocator_.deallocate(node, 1);
        }
        node = nullptr;
    }

    inline void clear() {}

    inline void reset() {}

private:
    interface::aligned_allocator<Node> allocator_;
};

}
}
}
}
