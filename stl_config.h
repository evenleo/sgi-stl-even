#ifndef __STL_CONFIG_H
#define __STL_CONFIG_H

#define __STL_USE_NAMESPACE
#define __STL_CLASS_PARTIAL_SPECIALIZATION

#ifdef __STL_USE_NAMESPACE
#define __STL_BEGIN_NAMESPACE namespace STD {
#define __STL_END_NAMESPACE }
#endif

#ifdef _WIN64
typedef long long ptrdiff_t;
#else
typedef int ptrdiff_t;
#endif /* _WIN64 */

#endif