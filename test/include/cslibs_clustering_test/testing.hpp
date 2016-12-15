#pragma once
#include <gtest/gtest.h>
#include <type_traits>

#ifdef CONFIGURE_STATIC_ASSERT_COMPILE_TIME

#define STATIC_ASSERT_EQ_TYPE(T1, T2) \
    ::testing::StaticAssertTypeEq<T1, T2>()


#define STATIC_ASSERT_EQ_VALUE(V1, V2) \
    ::testing::StaticAssertTypeEq< \
        std::integral_constant<typename std::common_type<decltype(V1), decltype(V2)>::type, V1>, \
        std::integral_constant<typename std::common_type<decltype(V1), decltype(V2)>::type, V2> \
    >()

#else

#define STATIC_ASSERT_EQ_TYPE(T1, T2) \
    ASSERT_TRUE((std::is_same<T1, T2>::value))

// needed to prevent odr-usage of values in gtest
#define STATIC_ASSERT_EQ_VALUE(V1, V2) \
    ASSERT_EQ( \
        (std::integral_constant<typename std::common_type<decltype(V1), decltype(V2)>::type, V1>::value), \
        (std::integral_constant<typename std::common_type<decltype(V1), decltype(V2)>::type, V2>::value) \
    )

#endif