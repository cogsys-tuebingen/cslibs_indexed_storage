#pragma once

namespace cslibs_indexed_storage
{
namespace interface
{

template<typename data_t> struct dense      { using type = data_t; };
template<typename data_t> struct sparse     { using type = data_t; };
template<typename data_t> struct non_owning { using type = data_t; };

}
}