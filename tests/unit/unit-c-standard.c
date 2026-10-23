/* SPDX-License-Identifier: Apache 2.0 */
/* Compiled as C with LightUSD's C standard; see unit-c-standard.cc. */
#include <stddef.h>

long lightusd_unit_c_standard_version(void) {
#if defined(__STDC_VERSION__)
  return (long)__STDC_VERSION__;
#else
  return 0L;
#endif
}

size_t lightusd_unit_c_max_align(void) {
  /* max_align_t is C11; nanoimage relies on it. */
  return sizeof(max_align_t);
}
