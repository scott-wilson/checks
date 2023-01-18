#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <cmocka.h>

#include "cchecks.h"

void destroy_string_ptr(CChecksString *string) {
  if (string->string != NULL) {
    free((void *)string->string);
  }
}

/* ----------------------------------------------------------------------------
  Int Item
*/
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

/* ----------------------------------------------------------------------------
  String Item
*/
typedef struct StringItem {
  CChecksItem header;
  char *type_hint;
  char *value;
} StringItem;

const char *string_item_type_hint_fn(const CChecksItem *item) {
  return ((StringItem *)item)->type_hint;
}

const void *string_item_value_fn(const CChecksItem *item) {
  return (void *)(((StringItem *)item)->value);
}

void string_item_clone_fn(const CChecksItem *item, CChecksItem *new_item) {
  StringItem *old_item = (StringItem *)item;
  StringItem *new_str_item = (StringItem *)new_item;
  new_str_item->header.type_hint_fn = item->type_hint_fn;
  new_str_item->header.value_fn = item->value_fn;
  new_str_item->header.clone_fn = item->clone_fn;
  new_str_item->header.destroy_fn = item->destroy_fn;
  new_str_item->header.debug_fn = item->debug_fn;
  new_str_item->header.display_fn = item->display_fn;
  new_str_item->header.lt_fn = item->lt_fn;
  new_str_item->header.eq_fn = item->eq_fn;

  if (!old_item->type_hint) {
    new_str_item->type_hint = NULL;
  } else {
    size_t new_type_hint_len = strlen(old_item->type_hint);
    char *new_type_hint = (char *)malloc(new_type_hint_len + 1);
    strcpy(new_type_hint, old_item->type_hint);
    new_str_item->type_hint = new_type_hint;
  }
  if (!old_item->value) {
    new_str_item->value = NULL;
  } else {
    size_t new_value_len = strlen(old_item->value);
    char *new_value = (char *)malloc(new_value_len + 1);
    strcpy(new_value, old_item->value);
    new_str_item->value = new_value;
  }
}

void string_item_destroy_fn(CChecksItem *item) {
  if (((StringItem *)item)->type_hint) {
    free(((StringItem *)item)->type_hint);
  }
  if (((StringItem *)item)->value) {
    free(((StringItem *)item)->value);
  }
}

CChecksString string_item_debug_fn(const CChecksItem *item) {
  char *value = ((StringItem *)item)->value;
  size_t length = snprintf(NULL, 0, "Item(%s)", value);
  char *debug_string = malloc(length + 1);
  sprintf(debug_string, "Item(%s)", value);

  CChecksString result;
  result.string = debug_string;
  result.destroy_fn = destroy_string_ptr;

  return result;
}

CChecksString string_item_display_fn(const CChecksItem *item) {
  char *value = ((StringItem *)item)->value;
  size_t display_str_len = strlen(value);
  char *display_str = (char *)malloc(display_str_len + 1);
  strcpy(display_str, value);

  CChecksString result;
  result.string = display_str;
  result.destroy_fn = destroy_string_ptr;

  return result;
}

size_t size_min(size_t a, size_t b) {
  if (a > b) {
    return a;
  } else {
    return b;
  }
}

bool string_item_lt_fn(const CChecksItem *item, const CChecksItem *other_item) {
  const char *a_value = ((StringItem *)item)->value;
  const char *b_value = ((StringItem *)other_item)->value;

  size_t length = size_min(strlen(a_value), strlen(b_value));

  for (size_t i = 0; i < length; i++) {
    if (a_value[i] < b_value[i]) {
      return true;
    }
  }

  return false;
}

bool string_item_eq_fn(const CChecksItem *item, const CChecksItem *other_item) {
  const char *a_value = ((StringItem *)item)->value;
  const char *b_value = ((StringItem *)other_item)->value;

  if (strlen(a_value) != strlen(b_value)) {
    return false;
  }

  for (size_t i = 0; i < strlen(a_value); i++) {
    if (a_value[i] != b_value[i]) {
      return false;
    }
  }

  return true;
}

StringItem create_string_item(const char *value, const char *type_hint) {
  char *new_type_hint;

  if (type_hint) {
    size_t new_type_hint_len = strlen(type_hint);
    new_type_hint = (char *)malloc(new_type_hint_len + 1);
    strcpy(new_type_hint, type_hint);
  } else {
    new_type_hint = NULL;
  }

  char *new_value;

  if (value) {
    size_t new_value_len = strlen(value);
    new_value = (char *)malloc(new_value_len + 1);
    strcpy(new_value, value);
  } else {
    new_value = NULL;
  }

  StringItem item;
  item.header.type_hint_fn = string_item_type_hint_fn;
  item.header.value_fn = string_item_value_fn;
  item.header.clone_fn = string_item_clone_fn;
  item.header.destroy_fn = string_item_destroy_fn;
  item.header.debug_fn = string_item_debug_fn;
  item.header.display_fn = string_item_display_fn;
  item.header.lt_fn = string_item_lt_fn;
  item.header.eq_fn = string_item_eq_fn;
  item.type_hint = new_type_hint;
  item.value = new_value;

  return item;
}

void destroy_string_item(StringItem *item) {
  cchecks_item_destroy((CChecksItem *)item);
}

/* ----------------------------------------------------------------------------
  Checks
*/
static void test_item_type_hint_success(void **state) {
  const char *result;
  IntItem item;

  // Test with a hint with text.
  item = create_int_item(1, "test");
  result = cchecks_item_type_hint((CChecksItem *)&item);

  assert_string_equal(result, "test");
  destroy_int_item(&item);

  // Test with a null hint.
  item = create_int_item(1, NULL);

  result = cchecks_item_type_hint((CChecksItem *)&item);

  assert_null(result);
  destroy_int_item(&item);
}

static void test_item_value_success(void **state) {
  IntItem int_item;
  const void *result;

  int_item = create_int_item(1, "test");
  result = cchecks_item_value((CChecksItem *)&int_item);
  int *int_result = (int *)result;

  assert_int_equal(*int_result, 1);
  destroy_int_item(&int_item);

  StringItem string_item;

  string_item = create_string_item("test", NULL);
  result = cchecks_item_value((CChecksItem *)&string_item);
  char *string_result = (char *)result;

  assert_string_equal(string_result, "test");
  destroy_string_item(&string_item);
}

static void test_item_clone_success(void **state) {
  IntItem int_item;
  IntItem new_int_item;

  int_item = create_int_item(1, "test");
  cchecks_item_clone((CChecksItem *)&int_item, (CChecksItem *)&new_int_item);

  assert_int_equal(int_item.value, new_int_item.value);
  assert_string_equal(int_item.type_hint, new_int_item.type_hint);
  assert_ptr_not_equal(&int_item.value, new_int_item.value);
  assert_ptr_not_equal(&int_item.type_hint, new_int_item.type_hint);

  destroy_int_item(&int_item);
  destroy_int_item(&new_int_item);

  StringItem string_item;
  StringItem new_string_item;

  string_item = create_string_item("test", "test");
  cchecks_item_clone((CChecksItem *)&string_item,
                     (CChecksItem *)&new_string_item);

  assert_string_equal(string_item.value, new_string_item.value);
  assert_string_equal(string_item.type_hint, new_string_item.type_hint);
  assert_ptr_not_equal(&string_item.value, new_string_item.value);
  assert_ptr_not_equal(&string_item.type_hint, new_string_item.type_hint);

  destroy_string_item(&string_item);
  destroy_string_item(&new_string_item);
}

static void test_item_debug_success(void **state) {
  CChecksString debug_string;

  IntItem int_item;

  int_item = create_int_item(1, "test");
  debug_string = cchecks_item_debug((CChecksItem *)&int_item);

  assert_string_equal(debug_string.string, "Item(1)");
  cchecks_string_destroy(&debug_string);
  destroy_int_item(&int_item);

  StringItem string_item;

  string_item = create_string_item("test", "test");
  debug_string = cchecks_item_debug((CChecksItem *)&string_item);

  assert_string_equal(debug_string.string, "Item(test)");
  cchecks_string_destroy(&debug_string);
  destroy_string_item(&string_item);
}

static void test_item_display_success(void **state) {
  CChecksString display_string;

  IntItem int_item;

  int_item = create_int_item(1, "test");
  display_string = cchecks_item_display((CChecksItem *)&int_item);

  assert_string_equal(display_string.string, "1");
  cchecks_string_destroy(&display_string);
  destroy_int_item(&int_item);

  StringItem string_item;

  string_item = create_string_item("test", "test");
  display_string = cchecks_item_display((CChecksItem *)&string_item);

  assert_string_equal(display_string.string, "test");
  cchecks_string_destroy(&display_string);
  destroy_string_item(&string_item);
}

static void test_item_lt_success(void **state) {
  IntItem a_int_item, b_int_item;

  // Int: A < B
  a_int_item = create_int_item(1, NULL);
  b_int_item = create_int_item(2, NULL);

  assert_true(
      cchecks_item_lt((CChecksItem *)&a_int_item, (CChecksItem *)&b_int_item));

  destroy_int_item(&a_int_item);
  destroy_int_item(&b_int_item);

  // Int: A == B
  a_int_item = create_int_item(1, NULL);
  b_int_item = create_int_item(1, NULL);

  assert_false(
      cchecks_item_lt((CChecksItem *)&a_int_item, (CChecksItem *)&b_int_item));
  destroy_int_item(&a_int_item);
  destroy_int_item(&b_int_item);

  // Int: A > B
  a_int_item = create_int_item(1, NULL);
  b_int_item = create_int_item(0, NULL);

  assert_false(
      cchecks_item_lt((CChecksItem *)&a_int_item, (CChecksItem *)&b_int_item));
  destroy_int_item(&a_int_item);
  destroy_int_item(&b_int_item);

  StringItem a_string_item, b_string_item;

  // String: A < B
  a_string_item = create_string_item("a", NULL);
  b_string_item = create_string_item("b", NULL);

  assert_true(cchecks_item_lt((CChecksItem *)&a_string_item,
                              (CChecksItem *)&b_string_item));

  destroy_string_item(&a_string_item);
  destroy_string_item(&b_string_item);

  // String: A == B
  a_string_item = create_string_item("a", NULL);
  b_string_item = create_string_item("a", NULL);

  assert_false(cchecks_item_lt((CChecksItem *)&a_string_item,
                               (CChecksItem *)&b_string_item));
  destroy_string_item(&a_string_item);
  destroy_string_item(&b_string_item);

  // String: A > B
  a_string_item = create_string_item("b", NULL);
  b_string_item = create_string_item("a", NULL);

  assert_false(cchecks_item_lt((CChecksItem *)&a_string_item,
                               (CChecksItem *)&b_string_item));
  destroy_string_item(&a_string_item);
  destroy_string_item(&b_string_item);
}

static void test_item_eq_success(void **state) {
  IntItem a_int_item, b_int_item;

  a_int_item = create_int_item(1, NULL);
  b_int_item = create_int_item(1, NULL);

  assert_true(
      cchecks_item_eq((CChecksItem *)&a_int_item, (CChecksItem *)&b_int_item));

  destroy_int_item(&a_int_item);
  destroy_int_item(&b_int_item);

  a_int_item = create_int_item(1, NULL);
  b_int_item = create_int_item(2, NULL);

  assert_false(
      cchecks_item_eq((CChecksItem *)&a_int_item, (CChecksItem *)&b_int_item));
  destroy_int_item(&a_int_item);
  destroy_int_item(&b_int_item);

  StringItem a_string_item, b_string_item;

  a_string_item = create_string_item("1", NULL);
  b_string_item = create_string_item("1", NULL);

  assert_true(cchecks_item_eq((CChecksItem *)&a_string_item,
                              (CChecksItem *)&b_string_item));

  destroy_string_item(&a_string_item);
  destroy_string_item(&b_string_item);

  a_string_item = create_string_item("1", NULL);
  b_string_item = create_string_item("2", NULL);

  assert_false(cchecks_item_eq((CChecksItem *)&a_string_item,
                               (CChecksItem *)&b_string_item));
  destroy_string_item(&a_string_item);
  destroy_string_item(&b_string_item);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_item_type_hint_success),
      cmocka_unit_test(test_item_value_success),
      cmocka_unit_test(test_item_clone_success),
      cmocka_unit_test(test_item_debug_success),
      cmocka_unit_test(test_item_display_success),
      cmocka_unit_test(test_item_lt_success),
      cmocka_unit_test(test_item_eq_success),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
