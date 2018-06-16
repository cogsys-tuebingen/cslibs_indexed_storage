#pragma once

#include <cslibs_indexed_storage/utility/index_sequence.hpp>
#include <boost/integer.hpp>
#include <array>
#include <valarray>

namespace cslibs_indexed_storage
{
namespace operations
{
namespace clustering
{

namespace detail
{

template <typename Seq1, std::size_t Offset, typename Seq2> struct concat_seq;

template <std::size_t ... Is1, std::size_t Offset, std::size_t ... Is2>
struct concat_seq<utility::index_sequence<Is1...>, Offset, utility::index_sequence<Is2...>>
{
    using type = utility::index_sequence<Is1..., (Offset + Is2)...>;
};

template<typename T>
constexpr T pow(T base, std::size_t exp)
{
    return exp == 0 ? T(1) : base * pow(base, exp - 1);
}

template<typename T>
constexpr T get_bit(std::size_t base, std::size_t bit, std::size_t value)
{
    return bit == 0 ? T(value % base) : get_bit<T>(base, bit - 1, value / base);
}

template<typename offset_t, std::size_t... breaks>
constexpr offset_t generate_offset(std::size_t base, std::size_t counter, utility::index_sequence<breaks...>)
{
    using value_type = typename offset_t::value_type;
    return {value_type(get_bit<value_type>(base, breaks, counter) - base / 2)...};
}

template<typename offset_t>
constexpr offset_t generate_offset(std::size_t base, std::size_t counter)
{
    return generate_offset<offset_t>(base, counter, utility::make_index_sequence<std::tuple_size<offset_t>::value>{});
}

template<typename list_t, std::size_t... counter>
constexpr list_t generate_all(std::size_t base, utility::index_sequence<counter...>)
{
    return {generate_offset<typename list_t::value_type>(base, counter)...};
}

template<typename list_t, std::size_t skip>
constexpr list_t generate(std::size_t base, bool skip_self)
{
    return skip_self ?
           generate_all<list_t>(base,
                                typename concat_seq<
                                        typename utility::make_index_sequence<skip>::type,
                                        skip + 1,
                                        typename utility::make_index_sequence<std::tuple_size<list_t>::value - skip>::type
                                >::type{}) :
           generate_all<list_t>(base, utility::make_index_sequence<std::tuple_size<list_t>::value>{});
}

//! \todo can be increased when unsing a log2 approach in make_index_sequence
constexpr std::size_t MAX_STATIC_SIZE = pow(9ul, 5);

}

template<std::size_t dimensions_, std::size_t size_, bool skip_self_ = true>
class GridNeighborhoodStatic
{
public:
    static constexpr std::size_t dimensions = dimensions_;
    static constexpr std::size_t size = size_;
    static constexpr bool skip_self = skip_self_;

    static_assert((size % 2) == 1, "size must be odd");

    static constexpr std::size_t count = detail::pow(size, dimensions) - (skip_self ? 1 : 0);
    static constexpr std::size_t skip_index = count / 2;
    static constexpr std::size_t shift = size / 2;

    static_assert(count <= detail::MAX_STATIC_SIZE, "static mask size exceeded, use GridNeighborhoodDynamic instead");

    using offset_value_t = typename boost::int_max_value_t<size>::fast;
    using offset_t = std::array<offset_value_t, dimensions>;
    using offset_list_t = std::array<offset_t, count>;

    template<typename visitor_t>
    static inline void visit(const visitor_t& visitor)
    {
        static constexpr offset_list_t offsets = detail::generate<offset_list_t, skip_index>(size, skip_self);
        for (const auto& offset : offsets)
            visitor(offset);
    }
};

class GridNeighborhoodDynamic
{
public:
    using offset_value_t = int64_t;
    using offset_t = std::valarray<offset_value_t>;

    size_t get_dimensions() const
    {
        return dimensions_;
    }

    void set_dimensions(size_t dimensions)
    {
        dimensions_ = dimensions;
    }

    size_t get_size() const
    {
        return size_;
    }

    void set_size(size_t size)
    {
        size_ = size;
    }

    bool is_skip_self() const
    {
        return skip_self_;
    }

    void set_skip_self(bool skip_self)
    {
        skip_self_ = skip_self;
    }

    template<typename visitor_t>
    inline void visit(const visitor_t& visitor)
    {
        const std::size_t count = std::pow<long double>(size_, dimensions_) - (skip_self_ ? 1 : 0);
        const std::size_t skip_index = count / 2;
        const std::size_t shift = size_ / 2;

        for (std::size_t i = 0; i <= count; ++i)
        {
            if (skip_self_ && i == skip_index)
                continue;

            offset_t offset(dimensions_);
            std::size_t value = i;
            for (std::size_t dim = 0; dim < dimensions_; ++dim)
            {
                offset[dim] = offset_value_t(offset_value_t(value % size_) - shift);
                value /= size_;
            }

            visitor(offset);
        }
    }

private:
    std::size_t dimensions_;
    std::size_t size_;
    bool skip_self_;
};

}
}
}
