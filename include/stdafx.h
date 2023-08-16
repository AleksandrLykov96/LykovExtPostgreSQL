// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#ifndef __STDAFX_H__
#define __STDAFX_H__

#define NDEBUG

#include <pg_config.h>

#undef HAVE_LONG_INT_64
#define HAVE_LONG_LONG_INT_64 1

#include <postgres.h>

#endif //__STDAFX_HPP__
