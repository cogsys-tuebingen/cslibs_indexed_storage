# ToDo List
* Allow insertion of non default-construtible data elements
  * Confirmed working only on `simple` containers
  * Allocate byte storage instead of real object (warp in union?) (watch for strict aliasing rule)
  * Use placement new for allocation
  * data_interface::create(storage, self) interface change
  
* Expose actual storage type to traverse functor (normalize `pointer` to `refs`)?
  * User might expect `pointer` since it is defined as such, but interoperability with generic `operations`?