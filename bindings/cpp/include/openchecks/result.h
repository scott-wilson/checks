#pragma once

#include <algorithm>
#include <cstring>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

extern "C" {
#include <openchecks.h>
}

#include "openchecks/core.h"
#include "openchecks/item.h"
#include "openchecks/items.h"
#include "openchecks/status.h"

namespace OPENCHECKS_NAMESPACE {
template <class T> class BaseCheck;

template <class T>
class OPENCHECKS_API CheckResult : private OpenChecksCheckResult {
public:
  friend class BaseCheck<T>;
  CheckResult(OPENCHECKS_NAMESPACE::Status status, const std::string &message,
              const std::optional<Items<T>> &items, bool can_fix, bool can_skip,
              std::optional<std::string> error = std::nullopt) {
    OpenChecksStatus cstatus = status.c_status();
    const char *cmessage = message.c_str();
    OpenChecksItems *citems = nullptr;

    if (items) {
      citems = openchecks_items_clone((OpenChecksItems *)&(items.value()));
    }

    const char *cerror;

    if (error.has_value()) {
      cerror = error.value().c_str();
    } else {
      cerror = nullptr;
    }

    OpenChecksCheckResult result = openchecks_check_result_new(
        cstatus, cmessage, citems, can_fix, can_skip, cerror);

    OpenChecksCheckResult::status = result.status;
    OpenChecksCheckResult::message = result.message;
    OpenChecksCheckResult::items = result.items;
    OpenChecksCheckResult::can_fix = result.can_fix;
    OpenChecksCheckResult::can_skip = result.can_skip;
    OpenChecksCheckResult::error = result.error;
    OpenChecksCheckResult::check_duration = result.check_duration;
    OpenChecksCheckResult::fix_duration = result.fix_duration;

    result.message = nullptr;
    result.items = nullptr;
    result.error = nullptr;
  }

  CheckResult(OpenChecksCheckResult &result) {
    OpenChecksCheckResult c_result = openchecks_check_result_new(
        result.status, result.message, result.items, result.can_fix,
        result.can_skip, result.error);

    OpenChecksCheckResult::status = c_result.status;
    OpenChecksCheckResult::message = c_result.message;
    OpenChecksCheckResult::items = c_result.items;
    OpenChecksCheckResult::can_fix = c_result.can_fix;
    OpenChecksCheckResult::can_skip = c_result.can_skip;
    OpenChecksCheckResult::error = c_result.error;
    OpenChecksCheckResult::check_duration = result.check_duration;
    OpenChecksCheckResult::fix_duration = result.fix_duration;

    c_result.message = nullptr;
    c_result.items = nullptr;
    c_result.error = nullptr;
  }

  CheckResult(CheckResult &result) {
    OpenChecksCheckResult c_result = openchecks_check_result_new(
        result.status, result.message, result.items, result.can_fix,
        result.can_skip, result.error);

    OpenChecksCheckResult::status = c_result.status;
    OpenChecksCheckResult::message = c_result.message;
    OpenChecksCheckResult::items = c_result.items;
    OpenChecksCheckResult::can_fix = c_result.can_fix;
    OpenChecksCheckResult::can_skip = c_result.can_skip;
    OpenChecksCheckResult::error = c_result.error;
    OpenChecksCheckResult::check_duration = result.check_duration;
    OpenChecksCheckResult::fix_duration = result.fix_duration;

    c_result.message = nullptr;
    c_result.items = nullptr;
    c_result.error = nullptr;
  }

  static CheckResult passed(const std::string &message,
                            const std::optional<Items<T>> &items, bool can_fix,
                            bool can_skip) {
    return CheckResult{OPENCHECKS_NAMESPACE::Status::Passed,
                       message,
                       items,
                       can_fix,
                       can_skip,
                       std::nullopt};
  }

  static CheckResult skipped(const std::string &message,
                             const std::optional<Items<T>> &items, bool can_fix,
                             bool can_skip) {
    return CheckResult{OPENCHECKS_NAMESPACE::Status::Skipped,
                       message,
                       items,
                       can_fix,
                       can_skip,
                       std::nullopt};
  }

  static CheckResult warning(const std::string &message,
                             const std::optional<Items<T>> &items, bool can_fix,
                             bool can_skip) {
    return CheckResult{OPENCHECKS_NAMESPACE::Status::Warning,
                       message,
                       items,
                       can_fix,
                       can_skip,
                       std::nullopt};
  }

  static CheckResult failed(const std::string &message,
                            const std::optional<Items<T>> &items, bool can_fix,
                            bool can_skip) {
    return CheckResult{OPENCHECKS_NAMESPACE::Status::Failed,
                       message,
                       items,
                       can_fix,
                       can_skip,
                       std::nullopt};
  }

  virtual ~CheckResult() {
    openchecks_check_result_destroy((OpenChecksCheckResult *)this);
  }

  const OPENCHECKS_NAMESPACE::Status status() const {
    return OPENCHECKS_NAMESPACE::Status(
        openchecks_check_result_status((OpenChecksCheckResult *)this));
  }

  std::string_view message() const {
    return std::string_view(openchecks_check_result_message(this).string);
  }

  const std::optional<OPENCHECKS_NAMESPACE::Items<T>> items() const {
    const OPENCHECKS_NAMESPACE::Items<T> *items =
        (const OPENCHECKS_NAMESPACE::Items<T> *)openchecks_check_result_items(
            this);

    if (items == nullptr) {
      return std::nullopt;
    } else {
      return std::optional<OPENCHECKS_NAMESPACE::Items<T>>(*items);
    }
  }

  bool can_fix() const { return openchecks_check_result_can_fix(this); }

  bool can_skip() const { return openchecks_check_result_can_skip(this); }

  std::optional<std::string_view> error() const {
    const char *cerr = openchecks_check_result_error(this);

    if (cerr == nullptr) {
      return std::nullopt;
    } else {
      return std::optional<std::string_view>(cerr);
    }
  }

  double check_duration() const {
    return openchecks_check_result_check_duration(this);
  }

  double fix_duration() const {
    return openchecks_check_result_fix_duration(this);
  }

private:
  CheckResult() {}
};

} // namespace OPENCHECKS_NAMESPACE
