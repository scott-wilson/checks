#include <tuple>
#include <vector>

#include <gtest/gtest.h>

#include <openchecks/core.h>
#include <openchecks/item.h>
#include <openchecks/items.h>
#include <openchecks/result.h>
#include <openchecks/runner.h>
#include <openchecks/status.h>

#include "check_impl.h"
#include "item_impl.h"
#include "items_impl.h"
#include "result_impl.h"

class CheckParameterizedTestFixture
    : public ::testing::TestWithParam<
          std::tuple<std::string,                     // title
                     std::string,                     // description
                     OPENCHECKS_NAMESPACE::CheckHint, // hint
                     OPENCHECKS_NAMESPACE::Status,    // status
                     OPENCHECKS_NAMESPACE::Status,    // fix_status
                     std::string,                     // message
                     std::optional<IntItems>,         // items
                     bool,                            // can_fix
                     bool,                            // can_skip
                     std::optional<std::string>       // error
                     >> {};

TEST_P(CheckParameterizedTestFixture, ResultSuccess) {
  std::string title = std::get<0>(GetParam());
  std::string description = std::get<1>(GetParam());
  OPENCHECKS_NAMESPACE::CheckHint hint = std::get<2>(GetParam());
  OPENCHECKS_NAMESPACE::Status status = std::get<3>(GetParam());
  OPENCHECKS_NAMESPACE::Status fix_status = std::get<4>(GetParam());
  std::string message = std::get<5>(GetParam());
  std::optional<IntItems> items = std::get<6>(GetParam());
  bool can_fix = std::get<7>(GetParam());
  bool can_skip = std::get<8>(GetParam());
  std::optional<std::string> error = std::get<9>(GetParam());

  Check check = Check{title,   description, hint,    status,   fix_status,
                      message, items,       can_fix, can_skip, error};

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
}

INSTANTIATE_TEST_SUITE_P(
    CheckResult, CheckParameterizedTestFixture,
    ::testing::Combine(
        ::testing::Values(std::string("title"), std::string("")), // title
        ::testing::Values(std::string("description"),
                          std::string("")), // description
        ::testing::Values(OPENCHECKS_NAMESPACE::CheckHint::None,
                          OPENCHECKS_NAMESPACE::CheckHint::AutoFix), // hint
        ::testing::Values(OPENCHECKS_NAMESPACE::Status::Pending,
                          OPENCHECKS_NAMESPACE::Status::Skipped,
                          OPENCHECKS_NAMESPACE::Status::Passed,
                          OPENCHECKS_NAMESPACE::Status::Warning,
                          OPENCHECKS_NAMESPACE::Status::Failed,
                          OPENCHECKS_NAMESPACE::Status::SystemError), // status
        ::testing::Values(
            OPENCHECKS_NAMESPACE::Status::Pending,
            OPENCHECKS_NAMESPACE::Status::Skipped,
            OPENCHECKS_NAMESPACE::Status::Passed,
            OPENCHECKS_NAMESPACE::Status::Warning,
            OPENCHECKS_NAMESPACE::Status::Failed,
            OPENCHECKS_NAMESPACE::Status::SystemError), // fix_status
        ::testing::Values(std::string("message"), std::string("")), // message
        ::testing::Values(IntItems{std::vector<IntItem>{
                              IntItem(0, ""), IntItem(1, ""), IntItem(2, "")}},
                          IntItems{std::vector<IntItem>{IntItem(0, "")}},
                          IntItems{std::vector<IntItem>{}},
                          std::nullopt), // items
        ::testing::Bool(),               // can_fix
        ::testing::Bool(),               // can_skip
        ::testing::Values(std::string("error"), std::string(),
                          std::nullopt) // error
        ));
