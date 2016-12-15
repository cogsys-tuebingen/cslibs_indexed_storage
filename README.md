# CS - Library: Clustering
This library provides fast, low overhead datastructure implementations for indexed objects.

Also included are configurable operations like:
* Clustering

## Usage & Structure
See [examples](example/) and [tests](test/) directories for some examples.

The only public access API is provided by the `Storage` type, which can be configured to allow abritrary data/index and storage implementations.

`Storage<Data, Index, Backend [, Options]>`


### `Data` requirements
* MUST implement `void merge(Data&)` iff `MergeStrategy == MERGE`


### `Index` requirements
Requirements are implicit through the `index_wrapper<Index>` specialization.

There are already some supported STD containers, see [support](include/cslibs_clustering/index/std):
* `std::array`


`index_wrapper<Index>` requiements:
* MUST have typedef `type`, real type passed through the interface
* MUST have static value `dimensions`, dimension of the index, MUST be `> 0`
* `type` MUST have `/*abritrary*/ operator[](std::size_t)`
* MUST have `struct get<std::size_t I>` (for static index access)
  * MUST have typedef `value_type` repesenting type of `Index` at dimension `I`
  * MUST have function `value_type& access(index&)` (modification access)
  * MUST have function `const value_type& access(const index&)` (get access)
  

### `Backend` requirements
* MUST have template signatur `<typename Data, typename Index>`
* MAY have `typename... Options` additionally
* MUST have function `template<typename... Args> Data& insert(const Index&, Args&&...)`
  * `Args` MAY be `Data` (move create)
  * `Data{std::forward<Args>(args)...}` MAY be valid (inplace create)
  * MUST respect `MergeStrategy` on duplicate index if available
* MUST have function `Data* get(const Index&)` (lookup by index)
  * return value MAY be `nullptr` if lookup failed
* MUST have function `const Data* get(const Index&) const` (lookup by index)
  * return value MAY be `nullptr` if lookup failed 
* MUST have function `template<typename Fn> void traverse(const Fn&)`
  * `Fn` IS of signature `void (const Index&, Data&)`
  * MUST traverse all stored `Data`
  * Traversal order IS be implementation defined
* MUST have function `template<typename Fn> void traverse(const Fn&) const`
  * `Fn` IS of signature `void (const Index&, const Data&)`
  * MUST traverse all stored `Data`
  * Traversal order IS be implementation defined
* MAY have function `void set(OptionTag, ...)` (dynamic option change)


## Operations
Operations are high order function on the dataset. They SHOULD NOT rely on the used backend and the access should mananged through the `Storage` API.

### Clustering
For an example see: [here](example/test.cpp)