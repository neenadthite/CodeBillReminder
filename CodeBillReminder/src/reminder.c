#include "reminder.h"

#include "date.h"


bool reminder_is_due(
    const Bill *bill)
{
    if (bill == NULL)
    {
        return false;
    }

    Date today = date_today();

    Date reminder_date =
        bill_get_reminder_date(bill);

    return date_equal(
        today,
        reminder_date
    );
}


bool reminder_is_overdue(
    const Bill *bill)
{
    if (bill == NULL)
    {
        return false;
    }

    return bill_days_until_due(bill) < 0;
}


bool reminder_has_notification_channel(
    const Bill *bill)
{
    if (bill == NULL)
    {
        return false;
    }

    return bill->email_enabled ||
           bill->sms_enabled;
}

size_t reminder_process_bills(
    const Bill* bills,
    size_t count,
    ReminderCallback callback,
    void* context)
{
    if (bills == NULL ||
        count == 0 ||
        callback == NULL)
    {
        return 0;
    }

    size_t reminder_count = 0;

    for (size_t i = 0; i < count; i++)
    {
        const Bill* bill = &bills[i];

        /*
         * Ignore bills that don't need
         * a reminder today.
         */
        if (!reminder_is_due(bill))
        {
            continue;
        }

        /*
         * Ignore bills that have no
         * notification channel enabled.
         */
        if (!reminder_has_notification_channel(bill))
        {
            continue;
        }

        if (callback(bill, context))
        {
            reminder_count++;
        }
    }

    return reminder_count;
}