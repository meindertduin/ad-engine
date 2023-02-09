#pragma once

#include <tuple>

template<typename ...Args>
struct VariadicArgs {
    using ArgTuple = std::tuple<Args...>;
    using ArgsCount = std::integral_constant<std::size_t, sizeof...(Args)>;

    using FirstArg = std::tuple_element_t<0, ArgTuple>;
    using SecondArg = std::tuple_element_t<1, ArgTuple>;
    using ThirdArg = std::tuple_element_t<2, ArgTuple>;
    using FourthArg = std::tuple_element_t<3, ArgTuple>;
    using FifthArg = std::tuple_element_t<4, ArgTuple>;
};
