# ToDo List
* Allow insertion of non default-construtible data elements
  * Confirmed working only on `simple` containers
  * Allocate byte storage instead of real object (warp in union?) (watch for strict aliasing rule)
  * Use placement new for allocation
  * data_interface::create(storage, self) interface change
  
* Implement <strike>`dense` (by value)</strike>, `sparse` (by pointer) and <strike>`non_owning` (by pointer, not owned) </strike>

* Improve `auto_index` to support different indexer per data type
  * Use `auto_index_tag` as marker instead of `auto_index<data_t>` wrapper
  * Pass `auto_index` functor as template param?
  * Allow auto_index optionally on variadics

* Refactor test cases for different backends
  * Recudes copy&paste code 
  
* Variadic Merge: Add type_trait tests that code certain constructions are invalid
  
# Open Questions
* Varidaic Merge: Should we allow a different signature that constructors?
  * May already implicitly rejected due to template instantiations
  * Explicit warning would be nicer
* Variadic Merge: if we have to decay to temporary object creation due to merge function signature,
  should we use data_interface::create?
