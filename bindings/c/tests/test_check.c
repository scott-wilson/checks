#include <float.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <cmocka.h>

#include "cchecks.h"

/* ----------------------------------------------------------------------------
  Int Item
*/
void destroy_string_ptr(CChecksString *string) {
  if (string->string != NULL) {
    free((void *)string->string);
  }
}

typedef struct IntItem {
  CChecksItem header;
  char *type_hint;
  int value;
} IntItem;

const char *int_item_type_hint_fn(const CChecksItem *item) {
  return ((IntItem *)item)->type_hint;
}

const void *int_item_value_fn(const CChecksItem *item) {
  return (void *)(&((IntItem *)item)->value);
}

void int_item_clone_fn(const CChecksItem *item, CChecksItem *new_item) {
  IntItem *old_item = (IntItem *)item;
  IntItem *new_int_item = (IntItem *)new_item;
  new_int_item->header.type_hint_fn = item->type_hint_fn;
  new_int_item->header.value_fn = item->value_fn;
  new_int_item->header.clone_fn = item->clone_fn;
  new_int_item->header.destroy_fn = item->destroy_fn;
  new_int_item->header.debug_fn = item->debug_fn;
  new_int_item->header.display_fn = item->display_fn;
  new_int_item->header.lt_fn = item->lt_fn;
  new_int_item->header.eq_fn = item->eq_fn;

  if (!old_item->type_hint) {
    new_int_item->type_hint = NULL;
  } else {
    size_t new_type_hint_len = strlen(old_item->type_hint);
    char *new_type_hint = (char *)malloc(new_type_hint_len + 1);
    strcpy(new_type_hint, old_item->type_hint);
    new_int_item->type_hint = new_type_hint;
  }

  new_int_item->value = old_item->value;
}

void int_item_destroy_fn(CChecksItem *item) {
  if (((IntItem *)item)->type_hint) {
    free(((IntItem *)item)->type_hint);
  }
}

CChecksString int_item_debug_fn(const CChecksItem *item) {
  int value = ((IntItem *)item)->value;
  size_t length = snprintf(NULL, 0, "Item(%d)", value);
  char *debug_string = malloc(length + 1);
  sprintf(debug_string, "Item(%d)", value);

  CChecksString result;
  result.string = debug_string;
  result.destroy_fn = destroy_string_ptr;

  return result;
}

CChecksString int_item_display_fn(const CChecksItem *item) {
  int value = ((IntItem *)item)->value;
  size_t length = snprintf(NULL, 0, "%d", value);
  char *display_string = malloc(length + 1);
  sprintf(display_string, "%d", value);

  CChecksString result;
  result.string = display_string;
  result.destroy_fn = destroy_string_ptr;

  return result;
}

bool int_item_lt_fn(const CChecksItem *item, const CChecksItem *other_item) {
  return ((IntItem *)item)->value < ((IntItem *)other_item)->value;
}

bool int_item_eq_fn(const CChecksItem *item, const CChecksItem *other_item) {
  return ((IntItem *)item)->value == ((IntItem *)other_item)->value;
}

IntItem create_int_item(int value, const char *type_hint) {
  char *new_type_hint;

  if (type_hint) {
    size_t new_type_hint_len = strlen(type_hint);
    new_type_hint = (char *)malloc(new_type_hint_len + 1);
    strcpy(new_type_hint, type_hint);
  } else {
    new_type_hint = NULL;
  }

  IntItem item;
  item.header.type_hint_fn = int_item_type_hint_fn;
  item.header.value_fn = int_item_value_fn;
  item.header.clone_fn = int_item_clone_fn;
  item.header.destroy_fn = int_item_destroy_fn;
  item.header.debug_fn = int_item_debug_fn;
  item.header.display_fn = int_item_display_fn;
  item.header.lt_fn = int_item_lt_fn;
  item.header.eq_fn = int_item_eq_fn;
  item.type_hint = new_type_hint;
  item.value = value;

  return item;
}

void destroy_int_item(IntItem *item) {
  cchecks_item_destroy((CChecksItem *)item);
}

void int_items_destroy_fn(CChecksItem *items) { free(items); }

/* ----------------------------------------------------------------------------
  Test check
*/
typedef struct TestCheck {
  CChecksBaseCheck header;
} TestCheck;

const char *test_check_title_fn(const CChecksBaseCheck *check) {
  return "title";
}

const char *test_check_description_fn(const CChecksBaseCheck *check) {
  return "description";
}

CChecksCheckHint test_check_hint_fn(const CChecksBaseCheck *check) {
  return CCHECKS_CHECK_HINT_NONE | CCHECKS_CHECK_HINT_AUTO_FIX;
}

void noop_items_destroy_fn(CChecksItem *ptr) {}

CChecksCheckResult test_check_run_fn(const CChecksBaseCheck *check) {
  return cchecks_check_result_passed("test", NULL, 0, 0, false, false,
                                     noop_items_destroy_fn);
}

CChecksAutoFixResult test_check_auto_fix_fn(const CChecksBaseCheck *check) {
  return cchecks_check_auto_fix_ok();
}

TestCheck create_test_check() {
  TestCheck check;
  check.header.title_fn = test_check_title_fn;
  check.header.description_fn = test_check_description_fn;
  check.header.hint_fn = test_check_hint_fn;
  check.header.check_fn = test_check_run_fn;
  check.header.auto_fix_fn = test_check_auto_fix_fn;

  return check;
}

/* ----------------------------------------------------------------------------
  Checks
*/
static void test_cchecks_check(void **state) {
  TestCheck check = create_test_check();

  assert_string_equal(cchecks_check_title((CChecksBaseCheck *)&check).string,
                      "title");
  assert_string_equal(
      cchecks_check_description((CChecksBaseCheck *)&check).string,
      "description");
  assert_int_equal(cchecks_check_hint((CChecksBaseCheck *)&check),
                   CCHECKS_CHECK_HINT_NONE | CCHECKS_CHECK_HINT_AUTO_FIX);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_cchecks_check),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
