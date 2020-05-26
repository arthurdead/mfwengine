#ifndef __THIRDPARTY_H
#define __THIRDPARTY_H

#pragma once

#if defined _M_X64 || defined _M_ARM64
using size_t = unsigned long long;
#elif defined _M_IX86 || defined _M_ARM
using size_t = unsigned int;
#else
#error
#endif

extern void * __cdecl operator new(size_t size, void *ptr) noexcept(true);
extern void * __cdecl operator new[](size_t size, void *ptr) noexcept(true);

extern void __cdecl operator delete(void *ptr, void *place) noexcept(true);
extern void __cdecl operator delete[](void *ptr, void *place) noexcept(true);

#endif