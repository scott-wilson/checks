#include <gtest/gtest.h>
#include <cppchecks/status.h>
#include <vector>
#include <tuple>

TEST(Status, IsStatusPendingSuccess)
{
    auto cases = std::vector{
        std::make_tuple(cppchecks::Status::Pending, true),
        std::make_tuple(cppchecks::Status::Skipped, false),
        std::make_tuple(cppchecks::Status::Passed, false),
        std::make_tuple(cppchecks::Status::Warning, false),
        std::make_tuple(cppchecks::Status::Failed, false),
        std::make_tuple(cppchecks::Status::SystemError, false),
    };

    cppchecks::Status status;
    bool expected;

    for (auto &&test_case : cases)
    {
        status = std::get<0>(test_case);
        expected = std::get<1>(test_case);
        ASSERT_EQ(status.is_pending(), expected);
    }
}

TEST(Status, HasStatusPassedSuccess)
{
    auto cases = std::vector{
        std::make_tuple(cppchecks::Status::Pending, false),
        std::make_tuple(cppchecks::Status::Skipped, true),
        std::make_tuple(cppchecks::Status::Passed, true),
        std::make_tuple(cppchecks::Status::Warning, true),
        std::make_tuple(cppchecks::Status::Failed, false),
        std::make_tuple(cppchecks::Status::SystemError, false),
    };

    cppchecks::Status status;
    bool expected;

    for (auto &&test_case : cases)
    {
        status = std::get<0>(test_case);
        expected = std::get<1>(test_case);
        ASSERT_EQ(status.has_passed(), expected);
    }
}

TEST(Status, HasStatusFailedSuccess)
{
    auto cases = std::vector{
        std::make_tuple(cppchecks::Status::Pending, false),
        std::make_tuple(cppchecks::Status::Skipped, false),
        std::make_tuple(cppchecks::Status::Passed, false),
        std::make_tuple(cppchecks::Status::Warning, false),
        std::make_tuple(cppchecks::Status::Failed, true),
        std::make_tuple(cppchecks::Status::SystemError, true),
    };

    cppchecks::Status status;
    bool expected;

    for (auto &&test_case : cases)
    {
        status = std::get<0>(test_case);
        expected = std::get<1>(test_case);
        ASSERT_EQ(status.has_failed(), expected);
    }
}
