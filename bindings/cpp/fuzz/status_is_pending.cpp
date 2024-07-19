#include <cassert>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include <fuzzer/FuzzedDataProvider.h>

#include <openchecks/status.h>

#include "common.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  FuzzedDataProvider provider(data, size);
  OPENCHECKS_NAMESPACE::Status status =
      (OpenChecksStatus)provider.ConsumeIntegralInRange<uint8_t>(
          (uint8_t)OpenChecksStatusPending,
          (uint8_t)OpenChecksStatusSystemError);

  if (status == OPENCHECKS_NAMESPACE::Status::Pending) {
    assert(status.is_pending() == true);
  } else {
    assert(status.is_pending() == false);
  }
  return 0;
}
