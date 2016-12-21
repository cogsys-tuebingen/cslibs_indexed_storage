#pragma once

// see http://stackoverflow.com/a/32223343

#include <cstdint>

namespace cslibs_indexed_storage
{
namespace helper
{

template <std::size_t... Ints>
struct index_sequence
{
    using type = index_sequence;
    using value_type = std::size_t;
    static constexpr std::size_t size() noexcept { return sizeof...(Ints); }
};

// --------------------------------------------------------------

template <class Sequence1, class Sequence2>
struct _merge_and_renumber;

template <std::size_t... I1, std::size_t... I2>
struct _merge_and_renumber<index_sequence<I1...>, index_sequence<I2...>>
        : index_sequence<I1..., (sizeof...(I1)+I2)...>
{ };

// --------------------------------------------------------------

template <std::size_t N>
struct make_index_sequence
        : _merge_and_renumber<typename make_index_sequence<N/2>::type,
                typename make_index_sequence<N - N/2>::type>
{ };

template<> struct make_index_sequence<0> : index_sequence<> { };
template<> struct make_index_sequence<1> : index_sequence<0> { };

}
}