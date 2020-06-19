/*
 * Copyright 2018-2020 Yury Gribov
 *
 * The MIT License (MIT)
 *
 * Use of this source code is governed by MIT license that can be
 * found in the LICENSE.txt file.
 */
u8R"(
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// Sanity check for ARM to avoid puzzling runtime crashes
#ifdef __arm__
# if defined __thumb__ && ! defined __THUMB_INTERWORK__
#   error "ARM trampolines need -mthumb-interwork to work in Thumb mode"
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define CHECK(cond, fmt, ...) do { \
    if(!(cond)) { \
      fprintf(stderr, "implib-gen: $load_name: " fmt "\n", ##__VA_ARGS__); \
      assert(0 && "Assertion in generated code"); \
      exit(1); \
    } \
  } while(0)

#define __STRINGIFY(x) #x
#define STRINGIFY(x) __STRINGIFY(x)

#ifndef _${lib_suffix}_CALL_USER_CALLBACK
#define _${lib_suffix}_CALL_USER_CALLBACK $has_dlopen_callback
#endif
#ifndef _${lib_suffix}_USER_CALLBACK
#define _${lib_suffix}_USER_CALLBACK $dlopen_callback
#endif
#ifndef _${lib_suffix}_NO_DLOPEN
#define _${lib_suffix}_NO_DLOPEN $no_dlopen
#endif
#ifndef _${lib_suffix}_LAZY_LOAD
#define _${lib_suffix}_LAZY_LOAD $lazy_load
#endif

static __attribute__((__visibility__("internal"))) void *_${lib_suffix}_lib_handle;
#if !_${lib_suffix}_NO_DLOPEN
#if _${lib_suffix}_CALL_USER_CALLBACK
extern void *_${lib_suffix}_USER_CALLBACK(const char *lib_name);
#endif

static __attribute__((__visibility__("internal"))) int _${lib_suffix}_is_lib_loading;

static __attribute__((__visibility__("internal"))) void *_${lib_suffix}_load_library() {
  if(_${lib_suffix}_lib_handle)
    return _${lib_suffix}_lib_handle;

  _${lib_suffix}_is_lib_loading = 1;

  // TODO: dlopen and users callback must be protected w/ critical section (to avoid dlopening lib twice)
#if _${lib_suffix}_CALL_USER_CALLBACK
  _${lib_suffix}_lib_handle = _${lib_suffix}_USER_CALLBACK("$load_name");
  CHECK(_${lib_suffix}_lib_handle, "callback '" STRINGIFY(_${lib_suffix}_USER_CALLBACK) "' failed to load library");
#else
  _${lib_suffix}_lib_handle = dlopen("$load_name", RTLD_LAZY | RTLD_GLOBAL);
  CHECK(_${lib_suffix}_lib_handle, "failed to load library: %s", dlerror());
#endif

  _${lib_suffix}_is_lib_loading = 0;

  return _${lib_suffix}_lib_handle;
}
#endif

#if !_${lib_suffix}_NO_DLOPEN && !_${lib_suffix}_LAZY_LOAD
static __attribute__((__visibility__("internal"), constructor)) void _${lib_suffix}_load() {
  _${lib_suffix}_load_library();
}
#endif

#if !_${lib_suffix}_NO_DLOPEN
static __attribute__((__visibility__("internal"), destructor)) void _${lib_suffix}_unload() {
  if(_${lib_suffix}_lib_handle)
    dlclose(_${lib_suffix}_lib_handle);
}
#endif

// TODO: convert to single 0-separated string
static __attribute__((__visibility__("internal"))) const char *const _${lib_suffix}_sym_names[] = {
  $sym_names
  0
};

extern __attribute__((__visibility__("internal"))) void *_${lib_suffix}_tramp_table[];

// Can be sped up by manually parsing library symtab...
__attribute__((__visibility__("internal"))) void _${lib_suffix}_tramp_resolve(int i)
{
  assert((unsigned)i + 1 < sizeof(_${lib_suffix}_sym_names) / sizeof(_${lib_suffix}_sym_names[0]));

#if !_${lib_suffix}_NO_DLOPEN
  CHECK(!_${lib_suffix}_is_lib_loading, "library function '%s' called during library load", _${lib_suffix}_sym_names[i]);
#endif

  void *h = 0;
#if !_${lib_suffix}_NO_DLOPEN && _${lib_suffix}_LAZY_LOAD
  h = _${lib_suffix}_load_library();
#else
  h = _${lib_suffix}_lib_handle;
  CHECK(h, "failed to resolve symbol '%s', library failed to load", _${lib_suffix}_sym_names[i]);
#endif

  // Dlsym is thread-safe so don't need to protect it.
  _${lib_suffix}_tramp_table[i] = dlsym(h, _${lib_suffix}_sym_names[i]);
  CHECK(_${lib_suffix}_tramp_table[i], "failed to resolve symbol '%s'", _${lib_suffix}_sym_names[i]);
}

// Helper for user to resolve all symbols
__attribute__((__visibility__("internal"))) void _${lib_suffix}_tramp_resolve_all(void)
{
  size_t i;
  for(i = 0; i + 1 < sizeof(_${lib_suffix}_sym_names) / sizeof(_${lib_suffix}_sym_names[0]) - 1; ++i) {
    _${lib_suffix}_tramp_resolve(i);
  }
}

$vtable_names

#ifdef __cplusplus
}  // extern "C"
#endif
)"_sv