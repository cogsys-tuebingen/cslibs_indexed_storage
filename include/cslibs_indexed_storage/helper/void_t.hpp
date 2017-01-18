#pragma once

namespace cslibs_indexed_storage
{
namespace helper
{
template<typename...> using void_t = void;

struct fallback {};
struct exists : fallback {};
}
}
