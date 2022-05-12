#pragma once
#ifndef MY_TYPENAME_HPP
#define MY_TYPENAME_HPP

namespace my {

template <typename T>
constexpr const char* type_name() { return typeid(T).name(); }

#define ENABLE_TYPENAME(A) \
    template <>            \
    constexpr const char* type_name<A>() { return #A; }

#define ENABLE_ALL_SPECS_TYPENAME(A) \
    ENABLE_TYPENAME(A);              \
    ENABLE_TYPENAME(const A);        \
    ENABLE_TYPENAME(A&);             \
    ENABLE_TYPENAME(const A&);       \
    ENABLE_TYPENAME(A*);             \
    ENABLE_TYPENAME(const A*);       \
    ENABLE_TYPENAME(const A* const); \
    ENABLE_TYPENAME(A[]);            \
    ENABLE_TYPENAME(const A[]);      \
    ENABLE_TYPENAME(A*&);            \
    ENABLE_TYPENAME(const A*&);      \
    ENABLE_TYPENAME(const A* const&);

ENABLE_ALL_SPECS_TYPENAME(int8_t);
ENABLE_ALL_SPECS_TYPENAME(int16_t);
ENABLE_ALL_SPECS_TYPENAME(int32_t);
ENABLE_ALL_SPECS_TYPENAME(int64_t);
//
ENABLE_ALL_SPECS_TYPENAME(uint8_t);
ENABLE_ALL_SPECS_TYPENAME(uint16_t);
ENABLE_ALL_SPECS_TYPENAME(uint32_t);
ENABLE_ALL_SPECS_TYPENAME(uint64_t);
//
ENABLE_ALL_SPECS_TYPENAME(char);
ENABLE_ALL_SPECS_TYPENAME(long);
ENABLE_ALL_SPECS_TYPENAME(unsigned long);
//
ENABLE_ALL_SPECS_TYPENAME(float);
ENABLE_ALL_SPECS_TYPENAME(double);
ENABLE_ALL_SPECS_TYPENAME(long double);
//
ENABLE_ALL_SPECS_TYPENAME(std::string);
ENABLE_ALL_SPECS_TYPENAME(std::wstring);
#if __cplusplus >= 202002L
ENABLE_ALL_SPECS_TYPENAME(std::u8string);
#endif
ENABLE_ALL_SPECS_TYPENAME(std::u16string);
ENABLE_ALL_SPECS_TYPENAME(std::u32string);

}  // namespace my

#endif  // MY_TYPENAME_HPP