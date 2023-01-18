#pragma once

extern "C"
{
#include <cchecks.h>
}

#include <cstring>
#include <iterator>
#include <cstddef>

#include "cppchecks/core.h"
#include "cppchecks/item.h"

namespace CPPCHECKS_NAMESPACE
{
    template <class T>
    class Items
    {
    public:
        class iterator
        {
        public:
            iterator(const CPPCHECKS_NAMESPACE::Item<T> *items) : _items(items) {}
            iterator &operator++()
            {
                this->_items++;
                return *this;
            }
            iterator operator++(int)
            {
                iterator retval = *this;
                ++(*this);
                return retval;
            }

            bool operator==(iterator &other) const
            {
                return this->_items == other._items;
            }

            bool operator!=(iterator &other) const
            {
                return !(*this == other);
            }

            const CPPCHECKS_NAMESPACE::Item<T> &operator*()
            {
                return *_items;
            }

            // iterator traits
            using difference_type = std::ptrdiff_t;
            // using difference_type = size_t;
            using value_type = CPPCHECKS_NAMESPACE::Item<T>;
            using pointer = const CPPCHECKS_NAMESPACE::Item<T> *;
            using reference = const CPPCHECKS_NAMESPACE::Item<T> &;
            using iterator_category = std::input_iterator_tag;

        private:
            const CPPCHECKS_NAMESPACE::Item<T> *_items;
        };

        Items(const CPPCHECKS_NAMESPACE::Item<T> *items, size_t count) : _items(items), _count(count) {}

        iterator begin()
        {
            return Items<T>::iterator(_items);
        }

        iterator end()
        {
            return Items<T>::iterator(_items + _count);
        }

    private:
        const CPPCHECKS_NAMESPACE::Item<T> *_items;
        size_t _count;
    };
} // namespace CPPCHECKS_NAMESPACE
