#pragma once

#include <cstddef>
#include <cstring>
#include <iterator>
#include <optional>
#include <vector>

extern "C" {
#include <openchecks.h>
}

#include "openchecks/core.h"
#include "openchecks/item.h"

namespace OPENCHECKS_NAMESPACE {
template <class T> class OPENCHECKS_API Items : private OpenChecksItems {
public:
  Items();

  Items(const std::vector<Item<T>> &items);

  virtual ~Items() = default;

  const std::optional<OPENCHECKS_NAMESPACE::Item<T>>
  operator[](size_t index) const;

  size_t length() const noexcept;

  virtual inline size_t item_size() const noexcept;

  virtual inline bool operator==(const Items<T> &other) const;

  virtual inline bool operator!=(const Items<T> &other) const;

private:
  std::vector<OPENCHECKS_NAMESPACE::Item<T>> _items;

  void init();

  static const OpenChecksItem *_get_impl(const OpenChecksItems *items,
                                         size_t index);
  static OpenChecksItems *_clone_impl(const OpenChecksItems *items);
  static size_t _length_impl(const OpenChecksItems *items);
  static size_t _item_size_impl(const OpenChecksItems *items);
  static bool _eq_impl(const OpenChecksItems *items,
                       const OpenChecksItems *other_items);
  static void _destroy_impl(OpenChecksItems *items);
};

template <class T> Items<T>::Items() { this->init(); }

template <class T> Items<T>::Items(const std::vector<Item<T>> &items) {
  this->init();
  this->_items = items;
}

template <class T>
const std::optional<OPENCHECKS_NAMESPACE::Item<T>>
Items<T>::operator[](size_t index) const {
  if (index < this->_items.size()) {
    return this->_items[index];
  } else {
    return std::nullopt;
  }
}

template <class T> size_t Items<T>::length() const noexcept {
  return this->_items.size();
}

template <class T> size_t Items<T>::item_size() const noexcept {
  return sizeof(OPENCHECKS_NAMESPACE::Item<T>);
}

template <class T>
inline bool Items<T>::operator==(const Items<T> &other) const {
  return this->_items == other._items;
}

template <class T>
inline bool Items<T>::operator!=(const Items<T> &other) const {
  return !(*this == other);
}

template <class T> void Items<T>::init() {
  this->get_fn = _get_impl;
  this->clone_fn = _clone_impl;
  this->length_fn = _length_impl;
  this->item_size_fn = _item_size_impl;
  this->eq_fn = _eq_impl;
  this->destroy_fn = _destroy_impl;
}

template <class T>
const OpenChecksItem *Items<T>::_get_impl(const OpenChecksItems *items,
                                          size_t index) {
  const Items<T> *cppitems = (const Items<T> *)items;

  return (const OpenChecksItem *)&cppitems[index];
}

template <class T>
OpenChecksItems *Items<T>::_clone_impl(const OpenChecksItems *items) {
  const Items<T> *cppitems = (const Items<T> *)items;

  Items<T> *new_items = new Items<T>;
  new_items->_items = cppitems->_items;

  return (OpenChecksItems *)new_items;
}

template <class T> size_t Items<T>::_length_impl(const OpenChecksItems *items) {
  const Items<T> *cppitems = (const Items<T> *)items;

  return cppitems->length();
}

template <class T>
size_t Items<T>::_item_size_impl(const OpenChecksItems *items) {
  const Items<T> *cppitems = (const Items<T> *)items;

  return cppitems->item_size();
}

template <class T>
bool Items<T>::_eq_impl(const OpenChecksItems *items,
                        const OpenChecksItems *other_items) {
  if (items == nullptr && other_items == nullptr) {
    return true;
  } else if (items == nullptr && other_items != nullptr) {
    return false;
  } else if (items != nullptr && other_items == nullptr) {
    return false;
  }

  const Items<T> *cppitems = (const Items<T> *)items;
  const Items<T> *cppother_items = (const Items<T> *)other_items;

  return *cppitems == *cppother_items;
}

template <class T> void Items<T>::_destroy_impl(OpenChecksItems *items) {
  Items<T> *cppitems = (Items<T> *)items;

  delete cppitems;
}

} // namespace OPENCHECKS_NAMESPACE
