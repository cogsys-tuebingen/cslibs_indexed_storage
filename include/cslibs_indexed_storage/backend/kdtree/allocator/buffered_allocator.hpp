#pragma once

#include <vector>

namespace cslibs_indexed_storage
{
namespace backend
{
namespace kdtree
{
namespace detail
{
template<class Node>
struct BufferedAllocator
{
    using chunk_t = std::vector<Node>;
    using chunk_list_t = std::vector<chunk_t>;
    static constexpr std::size_t DEFAULT_CHUNK_SIZE = 4096;

public:
    BufferedAllocator(std::size_t chunk_size = DEFAULT_CHUNK_SIZE) :
            chunk_size_(chunk_size),
            active_chunk_index_(0),
            next_free_(0)
    {
        chunks_.emplace_back(chunk_size_);
    }

    inline Node* allocate()
    {
        Node* node = &(chunks_[active_chunk_index_][next_free_]);
        if (++next_free_ >= chunk_size_)
            extend();
        return node;
    }

    inline void deallocate(Node*& node)
    {
        // no-op, if an indivdual node is deallocated we do not care here
        node = nullptr;
    }

    inline void clear()
    {
        chunks_.clear();
        extend();
    }

    inline void reset()
    {
        active_chunk_index_ = 0;
        next_free_ = 0;
    }

private:
    void extend()
    {
        ++active_chunk_index_;
        if (active_chunk_index_ >= chunks_.size())
            chunks_.emplace_back(chunk_size_);
        next_free_ = 0;
    }

private:
    std::size_t chunk_size_;
    chunk_list_t chunks_;
    std::size_t active_chunk_index_;
    std::size_t next_free_;
};

}
}
}
}
