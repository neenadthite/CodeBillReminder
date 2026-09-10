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