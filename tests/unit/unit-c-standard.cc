// SPDX-License-Identifier: Apache 2.0
//
// LightUSD's C sources are built as C11 or newer, also when LightUSD is a
// subproject of a parent that requests C99 (CMAKE_C_STANDARD).

#define TEST_NO_MAIN
#include "acutest.h"

#include <cstddef>

#include "unit-c-standard.h"

extern "C" long lightusd_unit_c_standard_version(void);
extern "C" size_t lightusd_unit_c_max_align(void);

void c_standard_c11_test(void) {
  const long version = lightusd_unit_c_standard_version();
  TEST_CHECK(version >= 201112L);
  TEST_MSG("__STDC_VERSION__ = %ld", version);
  TEST_CHECK(lightusd_unit_c_max_align() >= sizeof(double));
}
