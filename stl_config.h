#ifndef __STL_CONFIG_H
#define __STL_CONFIG_H

#define __STL_USE_NAMESPACE
#define __STL_CLASS_PARTIAL_SPECIALIZATION
#define __STL_USE_EXCEPTIONS
#define __STL_MEMBER_TEMPLATES
#define __STL_FUNCTION_TMPL_PARTIAL_ORDER
#define __STL_NULL_TMPL_ARGS 

#ifdef __STL_USE_NAMESPACE
#define __STD STD
#define __STL_BEGIN_NAMESPACE namespace STD {
#define __STL_END_NAMESPACE }

#define __STL_BEGIN_RELOPS_NAMESPACE namespace STD {
#define __STL_END_RELOPS_NAMESPACE }

#endif

#ifdef __STL_USE_EXCEPTIONS
#define __STL_TRY try
#define __STL_CATCH_ALL catch(...)
#define __STL_RETHROW throw
#define __STL_NOTHROW throw()
#define __STL_UNWIND(action) catch(...) { action; throw; }
#endif

#define __STL_TEMPLATE_NULL template<>

#ifdef _WIN64
typedef long long ptrdiff_t;
#else
typedef int ptrdiff_t;
#endif /* _WIN64 */

#endif