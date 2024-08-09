#include <cassert>
#include <optional>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <string_view>
#include <vector>

#include <fuzzer/FuzzedDataProvider.h>

#include <openchecks/check.h>
#include <openchecks/result.h>
#include <openchecks/runner.h>
#include <openchecks/status.h>

#include "common.h"

class Check : public OPENCHECKS_NAMESPACE::BaseCheck<int> {
public:
  Check(std::string title, std::string description,
        OPENCHECKS_NAMESPACE::CheckHint hint,
        OPENCHECKS_NAMESPACE::Status status,
        OPENCHECKS_NAMESPACE::Status fix_status, std::string message,
        std::optional<IntItems> items, bool can_fix, bool can_skip,
        std::optional<std::string> error)
      : _title(title), _description(description), _hint(hint), _status(status),
        _fix_status(fix_status), _message(message), _items(items),
        _can_fix(can_fix), _can_skip(can_skip), _error(error) {}

  virtual const std::string &title() const { return _title; }

  virtual const std::string &description() const { return _description; }

  virtual const OPENCHECKS_NAMESPACE::CheckHint hint() const {
    return this->_hint;
  }

  virtual OPENCHECKS_NAMESPACE::CheckResult<int> check() const {
    return OPENCHECKS_NAMESPACE::CheckResult{_status,  _message,  _items,
                                             _can_fix, _can_skip, _error};
  }

  virtual std::optional<std::string> auto_fix() {
    if (_error) {
      return _error;
    } else {
      _status = _fix_status;
      return std::nullopt;
    }
  }

  std::string _title;
  std::string _description;
  OPENCHECKS_NAMESPACE::CheckHint _hint;
  OPENCHECKS_NAMESPACE::Status _status;
  OPENCHECKS_NAMESPACE::Status _fix_status;
  std::string _message;
  std::optional<IntItems> _items;
  bool _can_fix;
  bool _can_skip;
  std::optional<std::string> _error;
};

OPENCHECKS_NAMESPACE::CheckHint get_hint(FuzzedDataProvider &provider) {
  OPENCHECKS_NAMESPACE::CheckHint hint = OPENCHECKS_NAMESPACE::CheckHint::None;

  if (provider.ConsumeBool()) {
    hint.insert(OPENCHECKS_NAMESPACE::CheckHint::AutoFix);
  }

  return hint;
}

Check create_check(FuzzedDataProvider &provider) {
  std::string title = get_message(provider);
  std::string description = get_message(provider);
  OPENCHECKS_NAMESPACE::CheckHint hint = get_hint(provider);
  OPENCHECKS_NAMESPACE::Status status =
      (OpenChecksStatus)provider.ConsumeIntegralInRange<uint8_t>(
          (uint8_t)OpenChecksStatusPending,
          (uint8_t)OpenChecksStatusSystemError);
  OPENCHECKS_NAMESPACE::Status fix_status =
      (OpenChecksStatus)provider.ConsumeIntegralInRange<uint8_t>(
          (uint8_t)OpenChecksStatusPending,
          (uint8_t)OpenChecksStatusSystemError);
  std::string message = get_message(provider);
  std::optional<IntItems> items =
      provider.ConsumeBool()
          ? std::optional<IntItems>(create_int_items(provider))
          : std::nullopt;
  bool can_fix = provider.ConsumeBool();
  bool can_skip = provider.ConsumeBool();
  std::optional<std::string> error =
      provider.ConsumeBool() ? std::optional<std::string>(get_message(provider))
                             : std::nullopt;

  return Check{title,   description, hint,    status,   fix_status,
               message, items,       can_fix, can_skip, error};
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  FuzzedDataProvider provider(data, size);
  Check check = create_check(provider);

  assert(check.title() == check._title);
  assert(check.description() == check._description);
  assert(check.hint() == check._hint);

  IntResult result = OPENCHECKS_NAMESPACE::run(check);

  OPENCHECKS_NAMESPACE::Status result_status = result.status();
  std::string_view result_message = result.message();
  std::optional<IntItems> result_items = result.items();
  std::optional<std::string_view> result_error = result.error();

  if (result_status == OPENCHECKS_NAMESPACE::Status::SystemError) {
    assert(result.can_fix() == false);
    assert(result.can_skip() == false);
  } else {
    assert(result.can_fix() == check._can_fix);
    assert(result.can_skip() == check._can_skip);
  }

  assert(result_message == check._message);
  assert(result_items == check._items);
  assert(result_error == check._error);

  if (result_status.has_failed() && result.can_fix()) {
    OPENCHECKS_NAMESPACE::CheckResult<int> fix_result =
        OPENCHECKS_NAMESPACE::auto_fix(check);

    OPENCHECKS_NAMESPACE::Status fix_result_status = fix_result.status();
    std::string_view fix_result_message = fix_result.message();
    const std::optional<IntItems> fix_result_items = fix_result.items();
    const std::optional<std::string_view> fix_result_error = fix_result.error();

    OPENCHECKS_NAMESPACE::CheckHint fix_hint = check.hint();

    if (!fix_hint.contains(OPENCHECKS_NAMESPACE::CheckHint::AutoFix)) {
      assert(fix_result_status == OPENCHECKS_NAMESPACE::Status::SystemError);
      assert(fix_result_message == "Check does not implement auto fix.");
      assert(fix_result_items == std::nullopt);
      assert(fix_result_error == std::nullopt);
    } else if (fix_result_error) {
      assert(fix_result_status == OPENCHECKS_NAMESPACE::Status::SystemError);
      assert(fix_result_message == "Error in auto fix.");
      assert(fix_result_items == std::nullopt);
      assert(fix_result_error.value() == check._error);
    } else {
      assert(fix_result_status == check._fix_status);
      assert(fix_result_message == check._message);

      if (!check._items) {
        assert(fix_result_items == std::nullopt);
      } else {
        assert(fix_result_items != std::nullopt);
        assert(fix_result_items == check._items);
      }

      assert(fix_result_error == std::nullopt);
    }

    if (fix_result_status == OPENCHECKS_NAMESPACE::Status::SystemError) {
      assert(fix_result.can_fix() == false);
      assert(fix_result.can_skip() == false);
    } else {
      assert(fix_result.can_fix() == check._can_fix);
      assert(fix_result.can_skip() == check._can_skip);
    }
  }

  return 0;
}
