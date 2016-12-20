# Data Interface
The data interface manages every access to the actual data type.
Thus, it acts as a warpper interface. It may be specialized for custom types to facilitate possible optimizations or special cases when working with the data.


Currently the interface wraps:
* Creation of new entries on insertion
* Merging two entries


## Storage hinting
The library supports transparent access to different underlying storage types.
For performance it is desired to store the data with automatic storage duration, instead of dynamic allocations.
But when working with large/sparse datasets we may run into memory presure.
Also it is often useful to only use this container to optimize access times, but the data is not actually stored in the container.


To facilitate these differentiations, it is possible to hint the used storage method.
The following methods exists (by default):
* `dense`: Owning, everything is allocated on storage creation
* `sparse`: NYI, Owning, nodes are only allocated when needed (heap allocation)
* `non_owniing`: non-owning, only references are stored, data is located elsewhere
By default the `dense` type is used.

To allow maximum transparency the defined `input` and `output` types should be the same for all specializations (e.g. all by `ref` or by `pointer`).

## Copy & Paste Dummy
```c++
template<typename data_t>
struct data_interface
{
    using hinted_type = /* type hinted: dense, sparse, non_owned */;
    using base_type = /* unqualified type */;
    using storage_type = /* type which is actually stored, usually base_type if dense otherwiese base_type* */;
    using input_type = /* type which is inserted in the API, usually base_type */;
    using output_type = /* type which is exposed in the API, usually base_type */;

    template<typename... Args>
    static inline constexpr storage_type create(Args&&... args)
    {
        // initializes and returns storage value
        // Note: Args may be variadic (inplace) or input_type (copy/move)
    }

    template<option::MergeStrategy strategy, typename... Args>
    static inline constexpr void merge(storage_type& self, Args&&... args)
    {
        // merges two nodes depending on the strategy parameter
        // Note: When MERGE is desired it should be implemented by the ::cslibs_clustering::merge function if possible
        // Note: Args may be variadic (inplace) or input_type (copy/move)
    }

    static inline constexpr output_type& expose(storage_type& storage)
    {
        // returns non-const output reference (e.g. pointer to ref conversion)
    }
    static inline constexpr const output_type& expose(const storage_type& storage)
    {
        // returns const output reference (e.g. pointer to ref conversion)
    }
};
```