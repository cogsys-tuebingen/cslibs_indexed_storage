# Index Interface
The `index_interface` manages every access to the actual index type.
Thus, it acts as a warpper interface. It may be specialized for custom types to facilitate possible optimizations or special cases when working with the index.

## Default implementation support
The index interface provides an default implementation, which works for certain index types:
Currently supported out of the box are:
* `std::array`

## Copy & Paste Dummy
```c++
struct index_interface : index_interface_common<T>
{
    using type = /* actual storage type */;
    static constexpr std::size_t dimensions = /* dimension of the index */;

    static constexpr auto access(std::size_t i, type& index) -> decltype(index[i])
    {
        // non-const dynamic access to an index
    }
    static constexpr auto access(std::size_t i, const type& index) -> decltype(index[i])
    {
        // const dynamic access to an index
    }
};
```

### The common base class
The `index_interface_common` implements certain additional functions, which can be implemented on the `access` functions.
Normally you can simply inherit from this interface.
```c++
struct index_interface_common
{
    template<std::size_t I>
    struct dimension
    {
        static_assert(I < dimensions, "index out of bounds");

        using value_type = /* type of the I-th* index value */;

        static inline constexpr value_type& access(type& index)
        {
            // static index access
        }
        static inline constexpr const value_type& access(const type& index)
        {
            // static index access
        }
    };

    template<typename other_t>
    static inline constexpr type add(const type& a, const other_t& b)
    {
        // adds an same sized index to this index
        // Note: the other index may be abritaty, so use the index_interface wrapper for it
    }
};
```