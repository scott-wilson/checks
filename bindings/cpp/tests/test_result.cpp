#include <vector>
#include <tuple>
#include <gtest/gtest.h>
#include <cppchecks/core.h>
#include <cppchecks/items.h>
#include <cppchecks/item.h>
#include <cppchecks/status.h>
#include <cppchecks/result.h>

#include "item_impl.h"

using IntResult = CPPCHECKS_NAMESPACE::CheckResult<int>;

void validate_result(IntResult &result,
                     CPPCHECKS_NAMESPACE::Status &status,
                     std::string &message,
                     std::vector<IntItem> &items,
                     bool can_fix,
                     bool can_skip,
                     std::string &error)
{
    EXPECT_EQ(result.status(), status);
    EXPECT_EQ(result.message(), message);

    auto result_items = result.items();

    int index = 0;

    for (auto &&item : result_items)
    {
        EXPECT_EQ(item.value(), index);
        index++;
    }

    EXPECT_EQ(index, items.size());

    if (status == CPPCHECKS_NAMESPACE::Status::SystemError)
    {
        EXPECT_FALSE(result.can_fix());
        EXPECT_FALSE(result.can_skip());
    }
    else
    {
        EXPECT_EQ(result.can_fix(), can_fix);
        EXPECT_EQ(result.can_skip(), can_skip);
    }

    EXPECT_EQ(result.error(), error);
}

class ResultParameterizedTestFixture : public ::testing::TestWithParam<std::tuple<CPPCHECKS_NAMESPACE::Status, std::string, std::vector<IntItem>, bool, bool, std::string>>
{
};

TEST_P(ResultParameterizedTestFixture, ResultSuccess)
{
    CPPCHECKS_NAMESPACE::Status status = std::get<0>(GetParam());
    std::string message = std::get<1>(GetParam());
    std::vector<IntItem> items = std::get<2>(GetParam());
    bool can_fix = std::get<3>(GetParam());
    bool can_skip = std::get<4>(GetParam());
    std::string error = std::get<5>(GetParam());

    IntResult result{status, message, items, can_fix, can_skip, error};

    validate_result(result, status, message, items, can_fix, can_skip, error);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    CheckResult,
    ResultParameterizedTestFixture,
    ::testing::Combine(
        ::testing::Values(CPPCHECKS_NAMESPACE::Status::Pending, CPPCHECKS_NAMESPACE::Status::Skipped, CPPCHECKS_NAMESPACE::Status::Passed, CPPCHECKS_NAMESPACE::Status::Warning, CPPCHECKS_NAMESPACE::Status::Failed, CPPCHECKS_NAMESPACE::Status::SystemError),
        ::testing::Values(std::string("message")),
        ::testing::Values(std::vector<IntItem>{IntItem(0, ""), IntItem(1, ""), IntItem(2, "")}),
        ::testing::Bool(),
        ::testing::Bool(),
        ::testing::Values(std::string("error"))
    )
);
// clang-format on

class StaticResultParameterizedTestFixture : public ::testing::TestWithParam<std::tuple<std::string, std::vector<IntItem>, bool, bool>>
{
};

TEST_P(StaticResultParameterizedTestFixture, ResultPassedSuccess)
{
    CPPCHECKS_NAMESPACE::Status status = CPPCHECKS_NAMESPACE::Status::Passed;
    std::string message = std::get<0>(GetParam());
    std::vector<IntItem> items = std::get<1>(GetParam());
    bool can_fix = std::get<2>(GetParam());
    bool can_skip = std::get<3>(GetParam());
    std::string error = "";

    IntResult result = IntResult::passed(message, items, can_fix, can_skip);

    validate_result(result, status, message, items, can_fix, can_skip, error);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    CheckResult,
    StaticResultParameterizedTestFixture,
    ::testing::Combine(
        ::testing::Values(std::string("message")),
        ::testing::Values(std::vector<IntItem>{IntItem(0, ""), IntItem(1, ""), IntItem(2, "")}),
        ::testing::Bool(),
        ::testing::Bool()
    )
);
// clang-format on
