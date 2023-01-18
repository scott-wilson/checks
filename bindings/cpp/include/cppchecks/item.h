#pragma once

extern "C"
{
#include <cchecks.h>
}

#include <string>
#include <string_view>
#include <cstring>
#include <sstream>

#include "cppchecks/core.h"

namespace CPPCHECKS_NAMESPACE
{
    namespace _private
    {
        void destroy_str_fn(struct CChecksString *str)
        {
            delete[] str->string;
        }
    } // namespace _private

    template <class T>
    class Item : private CChecksItem
    {
    public:
        Item(const T &value, const std::string &type_hint) : _value(value), _type_hint(type_hint)
        {
            this->init();
        }

        virtual ~Item() {}

        virtual void clone(const Item<T> &other)
        {
            this->clone(&other);
        }

        virtual void clone(const Item<T> *other)
        {
            this->init();
            this->_value = other->value();
            this->_type_hint = other->type_hint();
        }

        const T &value() const { return this->_value; }
        const std::string &type_hint() const { return this->_type_hint; }

        // Display
        virtual std::string display() const
        {
            return std::to_string(this->value());
        }
        // Debug
        virtual std::string debug() const
        {
            std::ostringstream stream;

            stream << "Item(" << this->display() << ")";

            return stream.str();
        }

        // Ordering
        virtual inline bool operator<(const Item<T> &other) const { return this->value() < other.value(); }
        virtual inline bool operator>(const Item<T> &other) const { return other < *this; }
        virtual inline bool operator<=(const Item<T> &other) const { return !(*this > other); }
        virtual inline bool operator>=(const Item<T> &other) const { return !(*this < other); }

        // Comparison
        virtual inline bool operator==(const Item<T> &other) const { return this->value() == other.value(); }
        virtual inline bool operator!=(const Item<T> &other) const { return !(*this == other); }

    private:
        T _value;
        std::string _type_hint;

        void init()
        {
            this->type_hint_fn = _type_hint_impl;
            this->value_fn = _value_impl;
            this->clone_fn = _clone_impl;
            this->destroy_fn = _destroy_impl;
            this->debug_fn = _debug_impl;
            this->display_fn = _display_impl;
            this->lt_fn = _lt_impl;
            this->eq_fn = _eq_impl;
        }

        static const char *_type_hint_impl(const CChecksItem *item)
        {
            const char *type_hint = ((Item<T> *)item)->type_hint().c_str();

            if (type_hint == nullptr || std::string_view(type_hint).empty())
            {
                return nullptr;
            }

            return type_hint;
        }

        static const void *_value_impl(const CChecksItem *item)
        {
            const T *value = &((Item<T> *)item)->value();

            return (void *)value;
        }

        static void _clone_impl(const CChecksItem *item, CChecksItem *other)
        {
            ((Item<T> *)item)->clone((Item<T> *)other);
        }

        static void _destroy_impl(CChecksItem *item)
        {
            ((Item<T> *)item)->~Item();
        };

        static CChecksString _debug_impl(const CChecksItem *item)
        {
            Item<T> *cppitem = (Item<T> *)item;

            std::string msg = cppitem->debug();

            char *cstr = new char[msg.length() + 1];
            std::strcpy(cstr, msg.c_str());

            CChecksString cchecks_msg;
            cchecks_msg.string = cstr;
            cchecks_msg.destroy_fn = CPPCHECKS_NAMESPACE::_private::destroy_str_fn;

            return cchecks_msg;
        }

        static CChecksString _display_impl(const CChecksItem *item)
        {
            Item<T> *cppitem = (Item<T> *)item;

            std::string msg = cppitem->display();

            char *cstr = new char[msg.length() + 1];
            std::strcpy(cstr, msg.c_str());

            CChecksString cchecks_msg;
            cchecks_msg.string = cstr;
            cchecks_msg.destroy_fn = CPPCHECKS_NAMESPACE::_private::destroy_str_fn;

            return cchecks_msg;
        }

        static bool _lt_impl(const CChecksItem *item, const CChecksItem *other)
        {
            return (*(Item<T> *)item) < (*(Item<T> *)other);
        }

        static bool _eq_impl(const CChecksItem *item, const CChecksItem *other)
        {
            return (*(Item<T> *)item) == (*(Item<T> *)other);
        }
    };
} // namespace CPPCHECKS_NAMESPACE
