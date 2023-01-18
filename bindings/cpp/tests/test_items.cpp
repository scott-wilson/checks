#include <vector>
#include <gtest/gtest.h>
#include <cppchecks/core.h>
#include <cppchecks/items.h>
#include <cppchecks/item.h>

#include "item_impl.h"

TEST(CheckItems, IterateSuccess)
{
    std::vector<IntItem> vec{IntItem(0, ""), IntItem(1, ""), IntItem(2, "")};
    CPPCHECKS_NAMESPACE::Items items{vec.data(), vec.size()};
    int index = 0;

    for (auto &&item : items)
    {
        ASSERT_EQ(item.value(), index);
        index++;
    }

    ASSERT_EQ(index, vec.size());
}
