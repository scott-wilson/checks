#ifndef openchecks_tests_citem
#define openchecks_tests_citem

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "openchecks.h"

void destroy_string_ptr(OpenChecksString *);

/* ----------------------------------------------------------------------------
  Int Item
*/
typedef struct IntItem {
  OpenChecksItem header;
  char *type_hint;
  int value;
} IntItem;

OpenChecksItem *int_item_clone_in_place(const IntItem *item,
                                        IntItem *new_item) {
  new_item->header.type_hint_fn = item->header.type_hint_fn;
  new_item->header.value_fn = item->header.value_fn;
  new_item->header.clone_fn = item->header.clone_fn;
  new_item->header.destroy_fn = item->header.destroy_fn;
  new_item->header.debug_fn = item->header.debug_fn;
  new_item->header.display_fn = item->header.display_fn;
  new_item->header.lt_fn = item->header.lt_fn;
  new_item->header.eq_fn = item->header.eq_fn;

  if (!item->type_hint) {
    new_item->type_hint = NULL;
  } else {
    size_t new_type_hint_len = strlen(item->type_hint);
    char *new_type_hint = (char *)malloc(new_type_hint_len + 1);
    strcpy(new_type_hint, item->type_hint);
    new_item->type_hint = new_type_hint;
  }

  new_item->value = item->value;

  return (OpenChecksItem *)new_item;
}

const char *int_item_type_hint_fn(const OpenChecksItem *item) {
  return ((IntItem *)item)->type_hint;
}

const void *int_item_value_fn(const OpenChecksItem *item) {
  return (void *)(&((IntItem *)item)->value);
}

OpenChecksItem *int_item_clone_fn(const OpenChecksItem *item) {
  IntItem *old_item = (IntItem *)item;
  IntItem *new_int_item = malloc(sizeof(IntItem));
  int_item_clone_in_place(old_item, new_int_item);

  return (OpenChecksItem *)new_int_item;
}

void int_item_destroy_fn(OpenChecksItem *item) {
  if (((IntItem *)item)->type_hint) {
    free(((IntItem *)item)->type_hint);
  }
}

OpenChecksString int_item_debug_fn(const OpenChecksItem *item) {
  return item->display_fn(item);
}

OpenChecksString int_item_display_fn(const OpenChecksItem *item) {
  int value = ((IntItem *)item)->value;
  size_t length = snprintf(NULL, 0, "%d", value);
  char *display_string = (char *)malloc(length + 1);
  sprintf(display_string, "%d", value);

  OpenChecksString result;
  result.string = display_string;
  result.destroy_fn = destroy_string_ptr;

  return result;
}

bool int_item_lt_fn(const OpenChecksItem *item,
                    const OpenChecksItem *other_item) {
  return ((IntItem *)item)->value < ((IntItem *)other_item)->value;
}

bool int_item_eq_fn(const OpenChecksItem *item,
                    const OpenChecksItem *other_item) {
  return ((IntItem *)item)->value == ((IntItem *)other_item)->value;
}

IntItem create_int_item(int value, const char *type_hint) {
  IntItem item;

  char *new_type_hint;

  if (type_hint) {
    size_t new_type_hint_len = strlen(type_hint);
    new_type_hint = (char *)malloc(new_type_hint_len + 1);
    strcpy(new_type_hint, type_hint);
  } else {
    new_type_hint = NULL;
  }

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

/* ----------------------------------------------------------------------------
  String Item
*/
typedef struct StringItem {
  OpenChecksItem header;
  char *type_hint;
  char *value;
} StringItem;

const char *string_item_type_hint_fn(const OpenChecksItem *item) {
  return ((StringItem *)item)->type_hint;
}

const void *string_item_value_fn(const OpenChecksItem *item) {
  return (void *)(((StringItem *)item)->value);
}

OpenChecksItem *string_item_clone_fn(const OpenChecksItem *item) {
  StringItem *old_item = (StringItem *)item;
  StringItem *new_str_item = malloc(sizeof(StringItem));
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

  return (OpenChecksItem *)new_str_item;
}

void string_item_destroy_fn(OpenChecksItem *item) {
  if (((StringItem *)item)->type_hint) {
    free(((StringItem *)item)->type_hint);
  }
  if (((StringItem *)item)->value) {
    free(((StringItem *)item)->value);
  }
}

OpenChecksString string_item_debug_fn(const OpenChecksItem *item) {
  return item->display_fn(item);
}

OpenChecksString string_item_display_fn(const OpenChecksItem *item) {
  char *value = ((StringItem *)item)->value;
  size_t display_str_len = strlen(value);
  char *display_str = (char *)malloc(display_str_len + 1);
  strcpy(display_str, value);

  OpenChecksString result;
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

bool string_item_lt_fn(const OpenChecksItem *item,
                       const OpenChecksItem *other_item) {
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

bool string_item_eq_fn(const OpenChecksItem *item,
                       const OpenChecksItem *other_item) {
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
  openchecks_item_destroy((OpenChecksItem *)item);
}

/* ----------------------------------------------------------------------------
  Utils
*/
void destroy_string_ptr(OpenChecksString *string) {
  if (string->string != NULL) {
    free((void *)string->string);
  }
}

#endif // openchecks_tests_citem
