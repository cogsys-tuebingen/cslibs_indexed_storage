#pragma once

namespace cslibs_indexed_storage
{
namespace interface
{
/**
 * Storage Hint: dense
 *
 * Orders the backend to use a dense storage approach.
 * Data is stored "on stack" (allocated on storage init) and owned by the backend.
 *
 * @tparam data_t actual data type
 */
template<typename data_t> struct dense      { using type = data_t; };

/**
 * Storage Hint: sparse
 *
 * Orders the backend to use a sparse storage approach.
 * Data is stored "on heap" (allocated when used) and owned by the backend.
 *
 * @tparam data_t actual data type
 */
template<typename data_t> struct sparse     { using type = data_t; };

/**
 * Storage Hint: non_owning
 *
 * Orders the backend to use a sparse storage approach.
 * Data is stored "by reference" and is not owned by the backend.
 *
 * @tparam data_t actual data type
 */
template<typename data_t> struct non_owning { using type = data_t; };

}
}