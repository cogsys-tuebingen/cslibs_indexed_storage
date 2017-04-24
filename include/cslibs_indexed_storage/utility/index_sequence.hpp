#pragma once

#include <cstdint>

namespace cslibs_indexed_storage { namespace utility
{

/**
 * \brief C++14 index_sequence implementation
 * \tparam Ints Indices
 *
 * Custom implementation of the C++14 \code std::index_sequence.
 *
 * Construction in \em log(N).
 *
 * \see http://stackoverflow.com/a/32223343 for source
 */
template <std::size_t... Ints>
struct index_sequence
{
    using type = index_sequence;
    using value_type = std::size_t;
    static constexpr std::size_t size() noexcept { return sizeof...(Ints); }
};

namespace detail
{

template<class Sequence1, class Sequence2>
struct merge_and_renumber;

template<std::size_t... I1, std::size_t... I2>
struct merge_and_renumber<index_sequence<I1...>, index_sequence<I2...>>
        : index_sequence<I1..., (sizeof...(I1) + I2)...>
{};

template <std::size_t N>
struct index_sequence_maker
        : detail::merge_and_renumber<typename index_sequence_maker<N/2>::type,
                typename index_sequence_maker<N - N/2>::type>
{};

template<> struct index_sequence_maker<0> : index_sequence<> { };
template<> struct index_sequence_maker<1> : index_sequence<0> { };

}

/**
 * \brief Construct an index_sequence of given size, 0 indexed.
 */
template<std::size_t N>
using make_index_sequence = typename detail::index_sequence_maker<N>::type;

}}
