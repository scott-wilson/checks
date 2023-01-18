#pragma once

extern "C"
{
#include <cchecks.h>
}

#include <string>
#include <bitset>

#include "cppchecks/core.h"
#include "cppchecks/check.h"
#include "cppchecks/result.h"

namespace CPPCHECKS_NAMESPACE
{
    template <class T>
    CPPCHECKS_NAMESPACE::CheckResult<T> run(const CPPCHECKS_NAMESPACE::BaseCheck<T> &check)
    {
        CPPCHECKS_NAMESPACE::CheckResult(cchecks_run(&check._check));
    }

    template <class T>
    CPPCHECKS_NAMESPACE::CheckResult<T> auto_fix(CPPCHECKS_NAMESPACE::BaseCheck<T> &check)
    {
        CPPCHECKS_NAMESPACE::CheckResult(cchecks_auto_fix(&check._check));
    }
} // namespace CPPCHECKS_NAMESPACE
