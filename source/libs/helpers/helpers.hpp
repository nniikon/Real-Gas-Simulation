#ifndef HELPERS_HPP_
#define HELPERS_HPP_

#include <type_traits>

#define USE_VAR(var) (void)var;

#if !defined(STRINGIFY)
#define STRINGIFY(x) #x
#endif // STRINGIFY

namespace gas {
namespace hlp {

template <
    typename EnumT, 
    typename ValueT = std::underlying_type_t<EnumT>,
    typename = std::enable_if_t<std::is_enum_v<EnumT>>
>
constexpr inline EnumT ToEnum(ValueT value) noexcept {
    return static_cast<EnumT>(value);
}

template <
    typename EnumT, 
    typename ValueT = std::underlying_type_t<EnumT>,
    typename = std::enable_if_t<std::is_enum_v<EnumT>>
>
constexpr inline ValueT FromEnum(EnumT enum_value) noexcept {
    return static_cast<ValueT>(enum_value);
}

} // namespace hlp
} // namespace gas

#endif // HELPERS_HPP_
