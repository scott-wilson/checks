#pragma once

extern "C"
{
#include <cchecks.h>
}

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>
#include <cstring>

#include "cppchecks/core.h"
#include "cppchecks/item.h"
#include "cppchecks/items.h"
#include "cppchecks/status.h"

namespace CPPCHECKS_NAMESPACE
{
    namespace _private
    {
        template <class T>
        inline CPPCHECKS_NAMESPACE::Item<T> *clone_vector(const std::vector<CPPCHECKS_NAMESPACE::Item<T>> &items)
        {
            size_t array_size = sizeof(CPPCHECKS_NAMESPACE::Item<T>) * items.size();
            void *citems_malloc = malloc(array_size);
            CPPCHECKS_NAMESPACE::Item<T> *citems = (CPPCHECKS_NAMESPACE::Item<T> *)citems_malloc;
            std::copy(items.cbegin(), items.cend(), citems);

            return citems;
        }
    } // namespace _private

    template <class T>
    class CheckResult
    {
    public:
        CheckResult(CPPCHECKS_NAMESPACE::Status status, const std::string &message, const std::vector<CPPCHECKS_NAMESPACE::Item<T>> &items, bool can_fix, bool can_skip, std::string error)
        {
            CChecksStatus cstatus = status.c_status();
            const char *cmessage = message.c_str();
            CChecksItem *citems = nullptr;
            size_t item_size = sizeof(CPPCHECKS_NAMESPACE::Item<T>);
            size_t item_count = 0;
            const char *cerror = error.c_str();

            citems = (CChecksItem *)CPPCHECKS_NAMESPACE::_private::clone_vector(items);
            item_count = items.size();

            _result = cchecks_check_result_new(cstatus, cmessage, citems, item_size, item_count, can_fix, can_skip, cerror, items_destroy_fn);
        }

        static CheckResult passed(const std::string &message, const std::vector<CPPCHECKS_NAMESPACE::Item<T>> &items, bool can_fix, bool can_skip)
        {
            const char *cmessage = message.c_str();
            CChecksItem *citems = nullptr;
            size_t item_size = sizeof(CPPCHECKS_NAMESPACE::Item<T>);
            size_t item_count = 0;

            citems = (CChecksItem *)CPPCHECKS_NAMESPACE::_private::clone_vector(items);
            item_count = items.size();

            CheckResult<T> result;
            result._result = cchecks_check_result_passed(cmessage, citems, item_size, item_count, can_fix, can_skip, items_destroy_fn);

            return result;
        }

        static CheckResult skipped(const std::string &message, const std::vector<CPPCHECKS_NAMESPACE::Item<T>> &items, bool can_fix, bool can_skip)
        {
            const char *cmessage = message.c_str();
            CChecksItem *citems = nullptr;
            size_t item_size = sizeof(CPPCHECKS_NAMESPACE::Item<T>);
            size_t item_count = 0;

            if (items)
            {
                citems = (CChecksItem *)CPPCHECKS_NAMESPACE::_private::clone_vector(items.value());
                item_count = items.value().size();
            }

            CheckResult<T> result;
            result._result = cchecks_check_result_skipped(cmessage, citems, item_size, item_count, can_fix, can_skip, items_destroy_fn);

            return result;
        }

        static CheckResult warning(const std::string &message, const std::vector<CPPCHECKS_NAMESPACE::Item<T>> &items, bool can_fix, bool can_skip)
        {
            const char *cmessage = message.c_str();
            CChecksItem *citems = nullptr;
            size_t item_size = sizeof(CPPCHECKS_NAMESPACE::Item<T>);
            size_t item_count = 0;

            if (items)
            {
                citems = (CChecksItem *)CPPCHECKS_NAMESPACE::_private::clone_vector(items.value());
                item_count = items.value().size();
            }

            CheckResult<T> result;
            result._result = cchecks_check_result_warning(cmessage, citems, item_size, item_count, can_fix, can_skip, items_destroy_fn);

            return result;
        }

        static CheckResult failed(const std::string &message, const std::vector<CPPCHECKS_NAMESPACE::Item<T>> &items, bool can_fix, bool can_skip)
        {
            const char *cmessage = message.c_str();
            CChecksItem *citems = nullptr;
            size_t item_size = sizeof(CPPCHECKS_NAMESPACE::Item<T>);
            size_t item_count = 0;

            if (items)
            {
                citems = (CChecksItem *)CPPCHECKS_NAMESPACE::_private::clone_vector(items.value());
                item_count = items.value().size();
            }

            CheckResult<T> result;
            result._result = cchecks_check_result_failed(cmessage, citems, item_size, item_count, can_fix, can_skip, items_destroy_fn);

            return result;
        }

        virtual ~CheckResult()
        {
            // TODO: This causes double frees.
            // cchecks_check_result_destroy(&this->_result);
        }

        const CPPCHECKS_NAMESPACE::Status status() const
        {
            return CPPCHECKS_NAMESPACE::Status(cchecks_check_result_status((CChecksCheckResult *)&_result));
        }

        std::string_view message() const
        {
            return std::string_view(cchecks_check_result_message(&_result).string);
        }

        CPPCHECKS_NAMESPACE::Items<T> items() const
        {
            const CChecksItems *citems = cchecks_check_result_items(&_result);

            const CChecksItem *ptr = citems->ptr;
            return CPPCHECKS_NAMESPACE::Items<T>((const CPPCHECKS_NAMESPACE::Item<T> *)ptr, citems->length);
        }

        bool can_fix() const
        {
            return cchecks_check_result_can_fix(&_result);
        }

        bool can_skip() const
        {
            return cchecks_check_result_can_skip(&_result);
        }

        std::string error() const
        {
            CChecksStringView cerr = cchecks_check_result_error(&_result);

            if (!cerr.string)
            {
                return std::string();
            }
            else
            {
                return std::string(cerr.string);
            }
        }

        double check_duration() const
        {
            return cchecks_check_result_check_duration(&_result);
        }

        double fix_duration() const
        {
            return cchecks_check_result_fix_duration(&_result);
        }

    private:
        CheckResult() {}
        CheckResult(CChecksCheckResult result) : _result(result) {}
        CChecksCheckResult _result;
        std::vector<CPPCHECKS_NAMESPACE::Item<T>> _items;

        static void items_destroy_fn(CChecksItem *item)
        {
            free((CPPCHECKS_NAMESPACE::Item<T> *)item);
        }
    };

} // namespace CPPCHECKS_NAMESPACE
