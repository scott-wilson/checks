extern "C"
{
#include <cchecks.h>
}

#include <gtest/gtest.h>
#include <cppchecks/item.h>
#include <string>
#include <sstream>

#include "item_impl.h"

TEST(Item, IntItemValueSuccess)
{
    IntItem item{1, std::string("test")};
    ASSERT_EQ(item.value(), 1);
}

TEST(Item, IntItemTypeHintSuccess)
{
    IntItem item{1, std::string("test")};
    ASSERT_EQ(item.type_hint(), std::string("test"));

    item = IntItem{1, ""};
    ASSERT_EQ(item.type_hint(), "");
}

TEST(Item, IntItemDisplaySuccess)
{
    IntItem item{1, std::string("test")};

    ASSERT_EQ(item.display(), "1");
}

TEST(Item, IntItemDebugSuccess)
{
    IntItem item{1, std::string("test")};

    ASSERT_EQ(item.debug(), "Item(1)");
}

TEST(Item, IntItemCloneSuccess)
{
    IntItem item{1, std::string("test")};
    IntItem other{2, std::string("other")};
    other.clone(item);

    ASSERT_EQ(item.value(), other.value());
    ASSERT_EQ(item.type_hint(), other.type_hint());
    ASSERT_NE(&item, &other);
}

TEST(Item, IntItemLtSuccess)
{
    IntItem item{1, std::string("test")};
    IntItem other{2, std::string("test")};

    ASSERT_LT(item, other);

    item = IntItem{1, std::string("test")};
    other = IntItem{1, std::string("test")};

    ASSERT_FALSE(item < other);
}

TEST(Item, IntItemLeSuccess)
{
    IntItem item{1, std::string("test")};
    IntItem other{2, std::string("test")};

    ASSERT_LE(item, other);

    item = IntItem{1, std::string("test")};
    other = IntItem{1, std::string("test")};

    ASSERT_LE(item, other);

    item = IntItem{2, std::string("test")};
    other = IntItem{1, std::string("test")};

    ASSERT_FALSE(item <= other);
}

TEST(Item, IntItemGtSuccess)
{
    IntItem item{2, std::string("test")};
    IntItem other{1, std::string("test")};

    ASSERT_GT(item, other);

    item = IntItem{1, std::string("test")};
    other = IntItem{1, std::string("test")};

    ASSERT_FALSE(item > other);
}

TEST(Item, IntItemGeSuccess)
{
    IntItem item{2, std::string("test")};
    IntItem other{1, std::string("test")};

    ASSERT_GE(item, other);

    item = IntItem{1, std::string("test")};
    other = IntItem{1, std::string("test")};

    ASSERT_GE(item, other);

    item = IntItem{1, std::string("test")};
    other = IntItem{2, std::string("test")};

    ASSERT_FALSE(item >= other);
}

TEST(Item, IntItemEqSuccess)
{
    IntItem item{1, std::string("test")};
    IntItem other{1, std::string("test")};

    ASSERT_EQ(item, other);

    item = IntItem{1, std::string("test")};
    other = IntItem{2, std::string("test")};

    ASSERT_FALSE(item == other);
}

TEST(Item, IntItemNeSuccess)
{
    IntItem item{1, std::string("test")};
    IntItem other{2, std::string("test")};

    ASSERT_NE(item, other);

    item = IntItem{1, std::string("test")};
    other = IntItem{1, std::string("test")};

    ASSERT_FALSE(item != other);
}

TEST(Item, CIntItemValueSuccess)
{
    IntItem item{1, std::string("test")};
    ASSERT_EQ(*(int *)cchecks_item_value((CChecksItem *)&item), 1);
}

TEST(Item, CIntItemTypeHintSuccess)
{
    IntItem item{1, std::string("test")};
    CChecksItem *citem = (CChecksItem *)&item;
    const char *ctype_hint = cchecks_item_type_hint(citem);

    ASSERT_STREQ(ctype_hint, "test");

    item = IntItem{1, ""};
    citem = (CChecksItem *)&item;
    ctype_hint = cchecks_item_type_hint(citem);

    ASSERT_EQ(ctype_hint, nullptr);
}

TEST(Item, CIntItemDisplaySuccess)
{
    IntItem item{1, std::string("test")};
    CChecksItem *citem = (CChecksItem *)&item;
    CChecksString display = cchecks_item_display(citem);

    ASSERT_STREQ(display.string, "1");

    cchecks_string_destroy(&display);
}

TEST(Item, CIntItemDebugSuccess)
{
    IntItem item{1, std::string("test")};
    CChecksItem *citem = (CChecksItem *)&item;
    CChecksString debug = cchecks_item_debug(citem);

    ASSERT_STREQ(debug.string, "Item(1)");

    cchecks_string_destroy(&debug);
}

TEST(Item, CIntItemCloneSuccess)
{
    IntItem item{1, std::string("test")};
    IntItem other{2, std::string("other")};
    cchecks_item_clone((CChecksItem *)&item, (CChecksItem *)&other);

    ASSERT_EQ(item.value(), other.value());
    ASSERT_EQ(item.type_hint(), other.type_hint());
    ASSERT_NE(&item, &other);
}

TEST(Item, CIntItemLtSuccess)
{
    IntItem item{1, std::string("test")};
    IntItem other{2, std::string("test")};

    ASSERT_TRUE(cchecks_item_lt((CChecksItem *)&item, (CChecksItem *)&other));

    item = IntItem{1, std::string("test")};
    other = IntItem{1, std::string("test")};

    ASSERT_FALSE(cchecks_item_lt((CChecksItem *)&item, (CChecksItem *)&other));
}

TEST(Item, CIntItemEqSuccess)
{
    IntItem item{1, std::string("test")};
    IntItem other{1, std::string("test")};

    ASSERT_TRUE(cchecks_item_eq((CChecksItem *)&item, (CChecksItem *)&other));

    item = IntItem{1, std::string("test")};
    other = IntItem{2, std::string("test")};

    ASSERT_FALSE(cchecks_item_eq((CChecksItem *)&item, (CChecksItem *)&other));
}
