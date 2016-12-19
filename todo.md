# ToDo List
* Allow insertion of non default-construtible data elements
  * Confirmed working only on `simple` containers
  * Allocate byte storage instead of real object (warp in union?) (watch for strict aliasing rule)
  * Use placement new for allocation
  * data_interface::create(storage, self) interface change
  
* Expose actual storage type to traverse functor or (normalize `pointer` to `refs`)?
  * User might expect `pointer` since it is defined as such, but interoperability with generic `operations`?
  
* Implement `dense` (by value), `sparse` (by pointer) and `non_owning` (by pointer, not owned)

* Improve `auto_index` to support different indexer per data type
  * Use `auto_index_tag` as marker instead of `auto_index<data_t>` wrapper
  * Pass `auto_index` functor as template param?

* Refactor test cases for different backends
  * Recudes copy&paste code 