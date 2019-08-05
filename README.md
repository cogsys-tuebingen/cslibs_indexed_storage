# CS - Library: Indexed Storage
This library provides fast, low overhead datastructure implementations for indexed objects.

Also included are configurable operations like:
* Clustering

## Usage & Structure
See [examples](example/) and [tests](test/) directories for some examples.

The only public access API is provided by the `Storage` type, which can be configured to allow arbitrary data/index and storage implementations.

`Storage<Data, Index, Backend [, Options]>`


### `Data` requirements
* There must be `data_interface` support, [see here](doc/data_interface.md).
* There should be an `merge` function:
  * Either free function: `::cslibs_clustering::merge(Data& self, const Data& other)`
  * or member function: `void Data::merge(const Data& other)`
  * Additionally `rvalue` or `variadic` implementation may be provided


### `Index` requirements
* There must be `index_interface` support, [see here](doc/index_interface.md).


## Operations
Operations are high order function on the dataset. They SHOULD NOT rely on the used backend and the access should mananged through the `Storage` API.

### Clustering
For an example see: [here](example/clustering.cpp)
