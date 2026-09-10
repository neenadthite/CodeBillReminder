#include "bill.h"

#include <string.h>


void bill_init(Bill* bill)
{
    if (bill == NULL)
    {
        return;
    }

    memset(bill, 0, sizeof(Bill));

    bill->reminder_days = 3;

    bill->email_enabled = false;
    bill->sms_enabled = false;

    bill->status = BILL_STATUS_PENDING;
}


bool bill_set_account_name(
    Bill* bill,
    const char* name)
{
    if (bill == NULL || name == NULL)
    {
        return false;
    }

    size_t length = strlen(name);

    if (length == 0 ||
        length >= sizeof(bill->account_name))
    {
        return false;
    }

    memcpy(
        bill->account_name,
        name,
        length + 1
    );

    return true;
}


bool bill_set_provider(
    Bill* bill,
    const char* provider)
{
    if (bill == NULL || provider == NULL)
    {
        return false;
    }

    size_t length = strlen(provider);

    if (length == 0 ||
        length >= sizeof(bill->provider))
    {
        return false;
    }

    memcpy(
        bill->provider,
        provider,
        length + 1
    );

    return true;
}


bool bill_set_account_number(
    Bill* bill,
    const char* account_number)
{
    if (bill == NULL || account_number == NULL)
    {
        return false;
    }

    size_t length = strlen(account_number);

    if (length == 0 ||
        length >= sizeof(bill->account_number))
    {
        return false;
    }

    memcpy(
        bill->account_number,
        account_number,
        length + 1
    );

    return true;
}


bool bill_set_amount_paise(
    Bill* bill,
    int64_t amount_paise)
{
    if (bill == NULL)
    {
        return false;
    }

    if (amount_paise < 0.0)
    {
        return false;
    }

    bill->amount_paise = amount_paise;

    return true;
}


bool bill_set_due_date(
    Bill* bill,
    Date due_date)
{
    if (bill == NULL)
    {
        return false;
    }

    if (!date_is_valid(due_date))
    {
        return false;
    }

    bill->due_date = due_date;

    return true;
}


bool bill_set_reminder_days(
    Bill* bill,
    int days)
{
    if (bill == NULL)
    {
        return false;
    }

    if (days < 0)
    {
        return false;
    }

    bill->reminder_days = days;

    return true;
}


void bill_set_email_enabled(
    Bill* bill,
    bool enabled)
{
    if (bill == NULL)
    {
        return;
    }

    bill->email_enabled = enabled;
}


void bill_set_sms_enabled(
    Bill* bill,
    bool enabled)
{
    if (bill == NULL)
    {
        return;
    }

    bill->sms_enabled = enabled;
}


void bill_set_status(
    Bill* bill,
    BillStatus status)
{
    if (bill == NULL)
    {
        return;
    }

    bill->status = status;
}


bool bill_is_valid(
    const Bill* bill)
{
    if (bill == NULL)
    {
        return false;
    }

    if (strlen(bill->account_name) == 0)
    {
        return false;
    }

    if (strlen(bill->provider) == 0)
    {
        return false;
    }

    if (strlen(bill->account_number) == 0)
    {
        return false;
    }

    if (bill->amount_paise < 0.0)
    {
        return false;
    }

    if (!date_is_valid(bill->due_date))
    {
        return false;
    }

    if (bill->reminder_days < 0)
    {
        return false;
    }

    return true;
}


Date bill_get_reminder_date(
    const Bill* bill)
{
    if (bill == NULL)
    {
        return date_create(1970, 1, 1);
    }

    return date_add_days(
        bill->due_date,
        -bill->reminder_days
    );
}


int bill_days_until_due(
    const Bill* bill)
{
    if (bill == NULL)
    {
        return 0;
    }

    Date today = date_today();

    return date_days_until(
        today,
        bill->due_date
    );
}