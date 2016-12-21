#pragma once

#include <cslibs_indexed_storage/backend/options.hpp>
#include <cslibs_indexed_storage/interface/data/data_storage_hints.hpp>

#include <cslibs_indexed_storage/interface/data/detail/data_interface_dense.hpp>
#include <cslibs_indexed_storage/interface/data/detail/data_interface_sparse.hpp>
#include <cslibs_indexed_storage/interface/data/detail/data_interface_nonowning.hpp>

namespace cslibs_indexed_storage
{
namespace interface
{

template<typename data_t>
struct data_interface : dense_data_interface<data_t>
{};

template<typename data_t>
struct data_interface<dense<data_t>> : dense_data_interface<data_t>
{};

//template<typename data_t>
//struct data_interface<sparse<data_t>> : sparse_data_interface<data_t>
//{};
//! NYI
template<typename data_t>
struct data_interface<sparse<data_t>>;

template<typename data_t>
struct data_interface<non_owning<data_t>> : nonowning_data_interface<data_t>
{};

}
}
