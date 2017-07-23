#ifndef IS_COMPLEX_T_H
#define IS_COMPLEX_T_H
/* Use tag dispaching:
https://stackoverflow.com/questions/41438493/how-to-identifying-whether-a-template-argument-is-stdcomplex
*/
#include <complex>
template<typename T>
struct is_complex_t : public std::false_type {};
template<typename T>
struct is_complex_t<std::complex<T>> : public std::true_type {};
#endif
