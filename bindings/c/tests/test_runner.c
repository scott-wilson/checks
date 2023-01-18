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
  Helpers
*/
void noop_items_destroy_fn(CChecksItem *ptr) {}

/* ----------------------------------------------------------------------------
  Always pass check
*/
typedef struct AlwaysPassCheck {
  CChecksBaseCheck header;
} AlwaysPassCheck;

const char *always_pass_check_title_fn(const CChecksBaseCheck *check) {
  return "Always Pass Check";
}

const char *always_pass_check_description_fn(const CChecksBaseCheck *check) {
  return "description";
}

CChecksCheckHint always_pass_check_hint_fn(const CChecksBaseCheck *check) {
  return CCHECKS_CHECK_HINT_NONE | CCHECKS_CHECK_HINT_AUTO_FIX;
}

CChecksCheckResult always_pass_check_run_fn(const CChecksBaseCheck *check) {
  return cchecks_check_result_passed("test", NULL, 0, 0, false, false,
                                     noop_items_destroy_fn);
}

AlwaysPassCheck create_always_pass_check() {
  AlwaysPassCheck check;
  check.header.title_fn = always_pass_check_title_fn;
  check.header.description_fn = always_pass_check_description_fn;
  check.header.hint_fn = always_pass_check_hint_fn;
  check.header.check_fn = always_pass_check_run_fn;
  check.header.auto_fix_fn = NULL;

  return check;
}

/* ----------------------------------------------------------------------------
  Always fail check
*/
typedef struct AlwaysFailCheck {
  CChecksBaseCheck header;
} AlwaysFailCheck;

const char *always_fail_check_title_fn(const CChecksBaseCheck *check) {
  return "Always Fail Check";
}

const char *always_fail_check_description_fn(const CChecksBaseCheck *check) {
  return "description";
}

CChecksCheckHint always_fail_check_hint_fn(const CChecksBaseCheck *check) {
  return CCHECKS_CHECK_HINT_NONE | CCHECKS_CHECK_HINT_AUTO_FIX;
}

CChecksCheckResult always_fail_check_run_fn(const CChecksBaseCheck *check) {
  return cchecks_check_result_failed("test", NULL, 0, 0, false, false,
                                     noop_items_destroy_fn);
}

AlwaysFailCheck create_always_fail_check() {
  AlwaysFailCheck check;
  check.header.title_fn = always_fail_check_title_fn;
  check.header.description_fn = always_fail_check_description_fn;
  check.header.hint_fn = always_fail_check_hint_fn;
  check.header.check_fn = always_fail_check_run_fn;
  check.header.auto_fix_fn = NULL;

  return check;
}
/* ----------------------------------------------------------------------------
  Pass on fix check
*/
typedef struct PassOnFixCheck {
  CChecksBaseCheck header;
  uint8_t value;
} PassOnFixCheck;

const char *pass_on_fix_title_fn(const CChecksBaseCheck *check) {
  return "Pass On Fix Check";
}

const char *pass_on_fix_description_fn(const CChecksBaseCheck *check) {
  return "description";
}

CChecksCheckHint pass_on_fix_hint_fn(const CChecksBaseCheck *check) {
  return CCHECKS_CHECK_HINT_NONE | CCHECKS_CHECK_HINT_AUTO_FIX;
}

CChecksCheckResult pass_on_fix_run_fn(const CChecksBaseCheck *check) {
  if (((PassOnFixCheck *)check)->value != 0) {
    return cchecks_check_result_failed("failed", NULL, 0, 0, true, false,
                                       noop_items_destroy_fn);
  } else {
    return cchecks_check_result_passed("passed", NULL, 0, 0, false, false,
                                       noop_items_destroy_fn);
  }
}

CChecksAutoFixResult pass_on_fix_auto_fix_fn(const CChecksBaseCheck *check) {
  ((PassOnFixCheck *)check)->value = 0;
  return cchecks_check_auto_fix_ok();
}

PassOnFixCheck create_pass_on_fix() {
  PassOnFixCheck check;
  check.header.title_fn = pass_on_fix_title_fn;
  check.header.description_fn = pass_on_fix_description_fn;
  check.header.hint_fn = pass_on_fix_hint_fn;
  check.header.check_fn = pass_on_fix_run_fn;
  check.header.auto_fix_fn = pass_on_fix_auto_fix_fn;
  check.value = 1;

  return check;
}

/* ----------------------------------------------------------------------------
  Fail on fix check
*/
typedef struct FailOnFixCheck {
  CChecksBaseCheck header;
  uint8_t value;
} FailOnFixCheck;

const char *fail_on_fix_title_fn(const CChecksBaseCheck *check) {
  return "Fail On Fix Check";
}

const char *fail_on_fix_description_fn(const CChecksBaseCheck *check) {
  return "description";
}

CChecksCheckHint fail_on_fix_hint_fn(const CChecksBaseCheck *check) {
  return CCHECKS_CHECK_HINT_NONE | CCHECKS_CHECK_HINT_AUTO_FIX;
}

CChecksCheckResult fail_on_fix_run_fn(const CChecksBaseCheck *check) {
  if (((FailOnFixCheck *)check)->value != 0) {
    return cchecks_check_result_failed("failed", NULL, 0, 0, true, false,
                                       noop_items_destroy_fn);
  } else {
    return cchecks_check_result_passed("passed", NULL, 0, 0, false, false,
                                       noop_items_destroy_fn);
  }
}

CChecksAutoFixResult fail_on_fix_auto_fix_fn(const CChecksBaseCheck *check) {
  ((FailOnFixCheck *)check)->value = 2;
  return cchecks_check_auto_fix_ok();
}

FailOnFixCheck create_fail_on_fix() {
  FailOnFixCheck check;
  check.header.title_fn = fail_on_fix_title_fn;
  check.header.description_fn = fail_on_fix_description_fn;
  check.header.hint_fn = fail_on_fix_hint_fn;
  check.header.check_fn = fail_on_fix_run_fn;
  check.header.auto_fix_fn = fail_on_fix_auto_fix_fn;
  check.value = 1;

  return check;
}

/* ----------------------------------------------------------------------------
  No auto-fix flag check
*/
typedef struct NoAutoFixFlagCheck {
  CChecksBaseCheck header;
} NoAutoFixFlagCheck;

const char *no_auto_fix_flag_check_title_fn(const CChecksBaseCheck *check) {
  return "No Auto Fix Flag Check";
}

const char *
no_auto_fix_flag_check_description_fn(const CChecksBaseCheck *check) {
  return "description";
}

CChecksCheckHint no_auto_fix_flag_check_hint_fn(const CChecksBaseCheck *check) {
  return CCHECKS_CHECK_HINT_NONE;
}

CChecksCheckResult
no_auto_fix_flag_check_run_fn(const CChecksBaseCheck *check) {
  return cchecks_check_result_failed("test", NULL, 0, 0, false, false,
                                     noop_items_destroy_fn);
}

NoAutoFixFlagCheck create_no_auto_fix_flag_check() {
  NoAutoFixFlagCheck check;
  check.header.title_fn = no_auto_fix_flag_check_title_fn;
  check.header.description_fn = no_auto_fix_flag_check_description_fn;
  check.header.hint_fn = no_auto_fix_flag_check_hint_fn;
  check.header.check_fn = no_auto_fix_flag_check_run_fn;
  check.header.auto_fix_fn = NULL;

  return check;
}

/* ----------------------------------------------------------------------------
  Checks
*/
static void test_always_pass_check(void **state) {
  AlwaysPassCheck check = create_always_pass_check();
  CChecksCheckResult result = cchecks_run((CChecksBaseCheck *)&check);
  assert_int_equal(cchecks_status_has_passed(&result.status), true);
  cchecks_check_result_destroy(&result);
}

static void test_always_fail_check(void **state) {
  AlwaysFailCheck check = create_always_fail_check();
  CChecksCheckResult result = cchecks_run((CChecksBaseCheck *)&check);
  assert_int_equal(cchecks_status_has_passed(&result.status), false);
  cchecks_check_result_destroy(&result);
}

static void test_pass_on_fix_check(void **state) {
  PassOnFixCheck check = create_pass_on_fix();
  CChecksCheckResult result;
  result = cchecks_run((CChecksBaseCheck *)&check);
  assert_int_equal(cchecks_status_has_passed(&result.status), false);
  cchecks_check_result_destroy(&result);

  result = cchecks_auto_fix((CChecksBaseCheck *)&check);
  assert_int_equal(cchecks_status_has_passed(&result.status), true);
  cchecks_check_result_destroy(&result);
}

static void test_fail_on_fix_check(void **state) {
  FailOnFixCheck check = create_fail_on_fix();
  CChecksCheckResult result;
  result = cchecks_run((CChecksBaseCheck *)&check);
  assert_int_equal(cchecks_status_has_passed(&result.status), false);
  cchecks_check_result_destroy(&result);

  result = cchecks_auto_fix((CChecksBaseCheck *)&check);
  assert_int_equal(cchecks_status_has_passed(&result.status), false);
  cchecks_check_result_destroy(&result);
}

static void test_no_auto_fix_flag_check(void **state) {
  NoAutoFixFlagCheck check = create_no_auto_fix_flag_check();
  CChecksCheckResult result = cchecks_auto_fix((CChecksBaseCheck *)&check);
  assert_int_equal(result.status, CChecksStatusSystemError);
  cchecks_check_result_destroy(&result);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_always_pass_check),
      cmocka_unit_test(test_always_fail_check),
      cmocka_unit_test(test_pass_on_fix_check),
      cmocka_unit_test(test_fail_on_fix_check),
      cmocka_unit_test(test_no_auto_fix_flag_check),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
