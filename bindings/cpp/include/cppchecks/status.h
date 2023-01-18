#pragma once

extern "C"
{
#include <cchecks.h>
}

#include "cppchecks/core.h"

namespace CPPCHECKS_NAMESPACE
{
    class Status
    {
    public:
        enum Value
        {
            Pending = CChecksStatusPending,
            Skipped = CChecksStatusSkipped,
            Passed = CChecksStatusPassed,
            Warning = CChecksStatusWarning,
            Failed = CChecksStatusFailed,
            SystemError = CChecksStatusSystemError,
        };

        Status() = default;
        constexpr Status(Value status) : _value(status) {}
        constexpr Status(CChecksStatus status) : _value((Status::Value)status) {}

        constexpr operator Value() const { return _value; }
        explicit operator bool() const = delete;

        bool is_pending() const
        {
            return cchecks_status_is_pending((CChecksStatus *)&_value);
        }

        bool has_passed() const
        {
            return cchecks_status_has_passed((CChecksStatus *)&_value);
        }

        bool has_failed() const
        {
            return cchecks_status_has_failed((CChecksStatus *)&_value);
        }

        CChecksStatus c_status() const
        {
            switch (_value)
            {
            case Status::Value::Pending:
                return CChecksStatusPending;
            case Status::Value::Skipped:
                return CChecksStatusSkipped;
            case Status::Value::Passed:
                return CChecksStatusPassed;
            case Status::Value::Warning:
                return CChecksStatusWarning;
            case Status::Value::Failed:
                return CChecksStatusFailed;
            case Status::Value::SystemError:
                return CChecksStatusSystemError;
            default:
                return CChecksStatusSystemError;
            }
        }

    private:
        Value _value;
    };
} // namespace CPPCHECKS_NAMESPACE
