#pragma once

#include <bitset>
#include <cstring>
#include <optional>
#include <string>

extern "C" {
#include <openchecks.h>
}

#include "openchecks/core.h"
#include "openchecks/item.h"
#include "openchecks/items.h"
#include "openchecks/result.h"
#include "openchecks/status.h"

namespace OPENCHECKS_NAMESPACE {
class OPENCHECKS_API CheckHint {
public:
  enum Value : OpenChecksCheckHint {
    None = OPENCHECKS_CHECK_HINT_NONE,
    AutoFix = OPENCHECKS_CHECK_HINT_AUTO_FIX,
  };

  constexpr CheckHint(Value hint) : _value(hint) {}
  constexpr CheckHint(OpenChecksCheckHint &hint)
      : _value((CheckHint::Value)hint) {}

  constexpr operator Value() const { return _value; }
  explicit operator bool() const = delete;

  void insert(CheckHint other) { _value = (Value)(_value | other._value); }

  bool contains(CheckHint other) const { return _value & other._value; }

  OpenChecksCheckHint c_hint() const { return _value; }

private:
  Value _value;
};

template <class T> class BaseCheck : private OpenChecksBaseCheck {
public:
  BaseCheck() {
    title_fn = title_impl;
    description_fn = description_impl;
    hint_fn = hint_impl;
    check_fn = check_impl;
    auto_fix_fn = auto_fix_impl;
  }

  virtual const std::string &title() const = 0;
  virtual const std::string &description() const = 0;
  virtual const CheckHint hint() const = 0;
  virtual OPENCHECKS_NAMESPACE::CheckResult<T> check() const = 0;
  virtual std::optional<std::string> auto_fix() {
    return std::string("Auto fix is not implemented.");
  }

private:
  static const char *title_impl(const OpenChecksBaseCheck *check) {
    return ((OPENCHECKS_NAMESPACE::BaseCheck<T> *)check)->title().c_str();
  }

  static const char *description_impl(const OpenChecksBaseCheck *check) {
    return ((OPENCHECKS_NAMESPACE::BaseCheck<T> *)check)->description().c_str();
  }

  static OpenChecksCheckHint hint_impl(const OpenChecksBaseCheck *check) {
    OPENCHECKS_NAMESPACE::BaseCheck<T> *cpp_check =
        (OPENCHECKS_NAMESPACE::BaseCheck<T> *)check;
    OPENCHECKS_NAMESPACE::CheckHint hint = cpp_check->hint();
    OpenChecksCheckHint c_hint = hint.c_hint();

    return c_hint;
  }

  static OpenChecksCheckResult check_impl(const OpenChecksBaseCheck *check) {
    OPENCHECKS_NAMESPACE::CheckResult<T> result =
        ((OPENCHECKS_NAMESPACE::BaseCheck<T> *)check)->check();

    OpenChecksCheckResult c_result = (OpenChecksCheckResult)result;

    ((OpenChecksCheckResult *)&result)->message = nullptr;
    ((OpenChecksCheckResult *)&result)->items = nullptr;
    ((OpenChecksCheckResult *)&result)->error = nullptr;

    return c_result;
  }

  static OpenChecksAutoFixResult auto_fix_impl(OpenChecksBaseCheck *check) {
    std::optional<std::string> auto_fix_result =
        ((OPENCHECKS_NAMESPACE::BaseCheck<T> *)check)->auto_fix();

    if (!auto_fix_result) {
      return openchecks_check_auto_fix_ok();
    } else {
      return openchecks_check_auto_fix_error(auto_fix_result.value().c_str());
    }
  }
};

} // namespace OPENCHECKS_NAMESPACE
