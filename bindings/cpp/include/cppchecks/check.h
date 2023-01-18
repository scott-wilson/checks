#pragma once

extern "C"
{
#include <cchecks.h>
}

#include <string>
#include <bitset>
#include <cstring>

#include "cppchecks/core.h"
#include "cppchecks/item.h"
#include "cppchecks/items.h"
#include "cppchecks/status.h"
#include "cppchecks/result.h"

namespace CPPCHECKS_NAMESPACE
{
    enum class CheckHint
    {
        None = CCHECKS_CHECK_HINT_NONE,
        AutoFix = CCHECKS_CHECK_HINT_AUTO_FIX,
    };

    template <class T>
    class BaseCheck
    {
    public:
        BaseCheck()
        {
            _check.title_fn = title_fn;
            _check.description_fn = description_fn;
            _check.hint_fn = hint_fn;
            _check.check_fn = check_fn;
            _check.auto_fix_fn = auto_fix_fn;
        }

        virtual std::string title() = 0;
        virtual std::string description() = 0;
        virtual CheckHint hint() = 0;
        virtual CPPCHECKS_NAMESPACE::CheckResult<T> check() = 0;
        virtual std::string auto_fix() { return std::string("Auto fix is not implemented."); }

    private:
        CChecksBaseCheck _check;
        static const char *title_fn(const CChecksBaseCheck *check)
        {
            return ((CPPCHECKS_NAMESPACE::BaseCheck<T> *)check)->title().c_str();
        }

        static const char *description_fn(const CChecksBaseCheck *check)
        {
            return ((CPPCHECKS_NAMESPACE::BaseCheck<T> *)check)->description().c_str();
        }

        static CChecksCheckHint hint_fn(const CChecksBaseCheck *check)
        {
            return ((CPPCHECKS_NAMESPACE::BaseCheck<T> *)check)->hint();
        }

        static CChecksCheckResult check_fn(const CChecksBaseCheck *check)
        {
            return ((CPPCHECKS_NAMESPACE::BaseCheck<T> *)check)->check()._result;
        }

        static CChecksAutoFixResult auto_fix_fn(const CChecksBaseCheck *check)
        {
            auto auto_fix_result = ((CPPCHECKS_NAMESPACE::BaseCheck<T> *)check)->auto_fix();

            if (!auto_fix_result)
            {
                return cchecks_check_auto_fix_ok();
            }
            else
            {
                return cchecks_check_auto_fix_error(auto_fix_result.value().c_str());
            }
        }
    };

} // namespace CPPCHECKS_NAMESPACE
