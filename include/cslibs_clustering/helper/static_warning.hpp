#pragma once

// see http://stackoverflow.com/a/8990275

#ifdef CONFIGURE_WITH_STATIC_WARNINGS

#if defined(__GNUC__)
#   define DEPRECATE(foo, msg) foo __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
#   define DEPRECATE(foo, msg) __declspec(deprecated(msg)) foo
#else
#   error This compiler is not supported
#endif

#define PP_CAT(x,y) PP_CAT1(x,y)
#define PP_CAT1(x,y) x##y

namespace helper
{
namespace detail
{
struct true_type {};
struct false_type {};
template <bool test> struct converter : public true_type {};
template <> struct converter<false> : public false_type {};
}
}

#define STATIC_WARNING(cond, msg) \
    struct PP_CAT(static_warning,__LINE__) { \
        DEPRECATE(void _(::helper::detail::false_type const& ),msg) {}; \
        void _(::helper::detail::true_type const& ) {}; \
        PP_CAT(static_warning, __LINE__)() {_(::helper::detail::converter<(cond)>());} \
    }

#define STATIC_WARNING_TEMPLATE(token, cond, msg) \
    STATIC_WARNING(cond, msg) PP_CAT(PP_CAT(_localvar_, token),__LINE__)

#else
#   define STATIC_WARNING(cond, msg)
#   define STATIC_WARNING_TEMPLATE(token, cond, msg)
#endif
