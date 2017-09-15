#pragma once

#include <memory>

namespace cslibs_indexed_storage
{
namespace interface
{

template<typename T, std::size_t RequestedAlignment = alignof(T)>
class aligned_allocator
{
public:
#if __GNUC__ >= 5
    static constexpr std::size_t Alignment = RequestedAlignment >= alignof(std::max_align_t) ? RequestedAlignment : alignof(std::max_align_t);
#else
    static constexpr std::size_t Alignment = RequestedAlignment >= alignof(void*) ? RequestedAlignment : alignof(void*);
#endif

    using pointer = T*;
    using const_pointer = const T*;
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
#if __GNUC__ < 5
    using reference = T&;
    using const_reference = const T&;
#endif

    template<typename U>
    struct rebind
    {
        using other = aligned_allocator<U, RequestedAlignment>;
    };

    aligned_allocator() = default;

    template<typename U>
    aligned_allocator(const aligned_allocator<U, RequestedAlignment>&) {}

    pointer allocate(size_type num, const void* /*hint*/ = nullptr)
    {
        const std::size_t bytes = sizeof(T) * num;
        void* memory;
        if (posix_memalign(&memory, Alignment, bytes) != 0)
            memory = nullptr;

        if (memory == nullptr && bytes != 0)
            throw std::bad_alloc();

        return static_cast<pointer>(memory);
    }

    void deallocate(pointer p, size_type /*num*/)
    {
        std::free(p);
    }

#if __GNUC__ < 5
    template<typename U, typename... Args>
    void construct(U* p, Args&&... args)
    {
        new ((void*) p) U(std::forward<Args>(args)...);
    }

    template<typename U>
    void destroy(U* p)
    {
        (*p).~U();
    }
#endif
};

template<typename T1, std::size_t Align1, typename T2, std::size_t Align2>
bool operator==(const aligned_allocator<T1, Align1>&,
                const aligned_allocator<T2, Align2>&) noexcept
{
    return false;
}

template<typename T1, typename T2, std::size_t Align>
bool operator==(const aligned_allocator<T1, Align>&,
                const aligned_allocator<T2, Align>&) noexcept
{
    return true;
}

template<typename T1, std::size_t Align1, typename T2, std::size_t Align2>
bool operator!=(const aligned_allocator<T1, Align1>& allocator1,
                const aligned_allocator<T2, Align2>& allocator2) noexcept
{
    return !(allocator1 == allocator2);
}

}
}
