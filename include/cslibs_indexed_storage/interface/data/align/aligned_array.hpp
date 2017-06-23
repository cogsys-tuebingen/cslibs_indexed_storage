#pragma once

#include <cslibs_indexed_storage/interface/data/align/aligned_allocator.hpp>
#include <type_traits>

namespace cslibs_indexed_storage
{
namespace interface
{

namespace detail
{
template<typename T>
void construct_array_elements_impl(std::true_type, std::size_t, T*)
{}

template<typename T>
void construct_array_elements_impl(std::false_type, std::size_t count, T* array)
{
    for (std::size_t i = 0; i < count; ++i)
        new (&(array[i])) T();
}

template<typename T>
void construct_array_elements(std::size_t count, T* array)
{
    // std::is_trivially_* is not implement in gcc 4.x, so always enforce construction...
#if __GNUC__ >= 5
    construct_array_elements_impl(std::is_trivially_default_constructible<T>{}, count, array);
#else
    construct_array_elements_impl(std::false_type{}, count, array);
#endif
}

template<typename T>
void destruct_array_elements_impl(std::true_type, std::size_t, T*)
{}

template<typename T>
void destruct_array_elements_impl(std::false_type, std::size_t count, T* array)
{
    for (std::size_t i = 0; i < count; ++i)
        array[i].~T();
}

template<typename T>
void destruct_array_elements(std::size_t count, T* array)
{
    destruct_array_elements_impl(std::is_trivially_destructible<T>{}, count, array);
}
}

template<typename T, std::size_t Alignment = alignof(T)>
class aligned_array
{
private:
    using Allocator = aligned_allocator<T, Alignment>;
public:
    aligned_array(std::size_t size) :
            allocator_(),
            size_(size),
            elements_(allocator_.allocate(size))
    {
        detail::construct_array_elements<T>(size, elements_);
    }

    ~aligned_array()
    {
        detail::destruct_array_elements<T>(size_, elements_);
        allocator_.deallocate(elements_, size_);
    }

    T& operator[](std::size_t index)
    {
        return elements_[index];
    }

    const T& operator[](std::size_t index) const
    {
        return elements_[index];
    }

    void resize(std::size_t new_size)
    {
        detail::destruct_array_elements<T>(size_, elements_);
        allocator_.deallocate(elements_, size_);

        size_ = new_size;
        elements_ = allocator_.allocate(size_);
        detail::construct_array_elements<T>(size_, elements_);
    }

private:
    Allocator allocator_;
    std::size_t size_;
    T* elements_;
};

}
}
