#include <cppchecks/core.h>
#include <cppchecks/item.h>
#include <cppchecks/items.h>
#include <cppchecks/result.h>
#include <cppchecks/status.h>
#include <gtest/gtest.h>
#include <tuple>
#include <vector>

#include "item_impl.h"
#include "items_impl.h"

using IntResult = CPPCHECKS_NAMESPACE::CheckResult<int>;

void validate_result(IntResult &result, CPPCHECKS_NAMESPACE::Status &status,
                     std::string &message, std::vector<IntItem> &items,
                     bool can_fix, bool can_skip,
                     std::optional<std::string> &error) {
  EXPECT_EQ(result.status(), status);
  EXPECT_EQ(result.message(), message);

  const std::optional<IntItems> result_items = result.items();
  EXPECT_NE(result_items, std::nullopt);

  size_t index = 0;

  for (index = 0; index < result_items.value().length(); index++) {
    const std::optional<IntItem> item = result_items.value()[index];

    EXPECT_NE(item, std::nullopt);
    EXPECT_EQ((int)item.value().value(), index);
  }

  EXPECT_EQ(index, items.size());

  if (status == CPPCHECKS_NAMESPACE::Status::SystemError) {
    EXPECT_FALSE(result.can_fix());
    EXPECT_FALSE(result.can_skip());
  } else {
    EXPECT_EQ(result.can_fix(), can_fix);
    EXPECT_EQ(result.can_skip(), can_skip);
  }

  EXPECT_EQ(result.error(), error);
}

class ResultParameterizedTestFixture
    : public ::testing::TestWithParam<std::tuple<
          CPPCHECKS_NAMESPACE::Status, std::string, std::vector<IntItem>, bool,
          bool, std::optional<std::string>>> {};

TEST_P(ResultParameterizedTestFixture, ResultSuccess) {
  CPPCHECKS_NAMESPACE::Status status = std::get<0>(GetParam());
  std::string message = std::get<1>(GetParam());
  std::vector<IntItem> items = std::get<2>(GetParam());
  bool can_fix = std::get<3>(GetParam());
  bool can_skip = std::get<4>(GetParam());
  std::optional<std::string> error = std::get<5>(GetParam());

  IntResult result{status, message, items, can_fix, can_skip, error};

  validate_result(result, status, message, items, can_fix, can_skip, error);
}

INSTANTIATE_TEST_SUITE_P(
    CheckResult, ResultParameterizedTestFixture,
    ::testing::Combine(
        ::testing::Values(CPPCHECKS_NAMESPACE::Status::Pending,
                          CPPCHECKS_NAMESPACE::Status::Skipped,
                          CPPCHECKS_NAMESPACE::Status::Passed,
                          CPPCHECKS_NAMESPACE::Status::Warning,
                          CPPCHECKS_NAMESPACE::Status::Failed,
                          CPPCHECKS_NAMESPACE::Status::SystemError), // status
        ::testing::Values(std::string("message")),                   // message
        ::testing::Values(std::vector<IntItem>{IntItem(0, ""), IntItem(1, ""),
                                               IntItem(2, "")}), // items
        ::testing::Bool(),                                       // can_fix
        ::testing::Bool(),                                       // can_skip
        ::testing::Values(std::string("error"), std::string(),
                          std::nullopt) // error
        ));

class PassedResultParameterizedTestFixture
    : public ::testing::TestWithParam<
          std::tuple<std::string, std::vector<IntItem>, bool, bool>> {};

TEST_P(PassedResultParameterizedTestFixture, ResultPassedSuccess) {
  CPPCHECKS_NAMESPACE::Status status = CPPCHECKS_NAMESPACE::Status::Passed;
  std::string message = std::get<0>(GetParam());
  std::vector<IntItem> items = std::get<1>(GetParam());
  bool can_fix = std::get<2>(GetParam());
  bool can_skip = std::get<3>(GetParam());
  std::optional<std::string> error = std::nullopt;

  IntResult result = IntResult::passed(message, items, can_fix, can_skip);

  validate_result(result, status, message, items, can_fix, can_skip, error);
}

INSTANTIATE_TEST_SUITE_P(
    CheckResult, PassedResultParameterizedTestFixture,
    ::testing::Combine(::testing::Values(std::string("message")), // message
                       ::testing::Values(std::vector<IntItem>{
                           IntItem(0, ""), IntItem(1, ""),
                           IntItem(2, "")}), // items
                       ::testing::Bool(),    // can_fix
                       ::testing::Bool()     // can_skip
                       ));

class SkippedResultParameterizedTestFixture
    : public ::testing::TestWithParam<
          std::tuple<std::string, std::vector<IntItem>, bool, bool>> {};

TEST_P(SkippedResultParameterizedTestFixture, ResultSkippedSuccess) {
  CPPCHECKS_NAMESPACE::Status status = CPPCHECKS_NAMESPACE::Status::Skipped;
  std::string message = std::get<0>(GetParam());
  std::vector<IntItem> items = std::get<1>(GetParam());
  bool can_fix = std::get<2>(GetParam());
  bool can_skip = std::get<3>(GetParam());
  std::optional<std::string> error = std::nullopt;

  IntResult result = IntResult::skipped(message, items, can_fix, can_skip);

  validate_result(result, status, message, items, can_fix, can_skip, error);
}

INSTANTIATE_TEST_SUITE_P(
    CheckResult, SkippedResultParameterizedTestFixture,
    ::testing::Combine(::testing::Values(std::string("message")), // message
                       ::testing::Values(std::vector<IntItem>{
                           IntItem(0, ""), IntItem(1, ""),
                           IntItem(2, "")}), // items
                       ::testing::Bool(),    // can_fix
                       ::testing::Bool()     // can_skip
                       ));

class WarningResultParameterizedTestFixture
    : public ::testing::TestWithParam<
          std::tuple<std::string, std::vector<IntItem>, bool, bool>> {};

TEST_P(WarningResultParameterizedTestFixture, ResultWarningSuccess) {
  CPPCHECKS_NAMESPACE::Status status = CPPCHECKS_NAMESPACE::Status::Warning;
  std::string message = std::get<0>(GetParam());
  std::vector<IntItem> items = std::get<1>(GetParam());
  bool can_fix = std::get<2>(GetParam());
  bool can_skip = std::get<3>(GetParam());
  std::optional<std::string> error = std::nullopt;

  IntResult result = IntResult::warning(message, items, can_fix, can_skip);

  validate_result(result, status, message, items, can_fix, can_skip, error);
}

INSTANTIATE_TEST_SUITE_P(
    CheckResult, WarningResultParameterizedTestFixture,
    ::testing::Combine(::testing::Values(std::string("message")), // message
                       ::testing::Values(std::vector<IntItem>{
                           IntItem(0, ""), IntItem(1, ""),
                           IntItem(2, "")}), // items
                       ::testing::Bool(),    // can_fix
                       ::testing::Bool()     // can_skip
                       ));

class FailedResultParameterizedTestFixture
    : public ::testing::TestWithParam<
          std::tuple<std::string, std::vector<IntItem>, bool, bool>> {};

TEST_P(FailedResultParameterizedTestFixture, ResultFailedSuccess) {
  CPPCHECKS_NAMESPACE::Status status = CPPCHECKS_NAMESPACE::Status::Failed;
  std::string message = std::get<0>(GetParam());
  std::vector<IntItem> items = std::get<1>(GetParam());
  bool can_fix = std::get<2>(GetParam());
  bool can_skip = std::get<3>(GetParam());
  std::optional<std::string> error = std::nullopt;

  IntResult result = IntResult::failed(message, items, can_fix, can_skip);

  validate_result(result, status, message, items, can_fix, can_skip, error);
}

INSTANTIATE_TEST_SUITE_P(
    CheckResult, FailedResultParameterizedTestFixture,
    ::testing::Combine(::testing::Values(std::string("message")), // message
                       ::testing::Values(std::vector<IntItem>{
                           IntItem(0, ""), IntItem(1, ""),
                           IntItem(2, "")}), // items
                       ::testing::Bool(),    // can_fix
                       ::testing::Bool()     // can_skip
                       ));
