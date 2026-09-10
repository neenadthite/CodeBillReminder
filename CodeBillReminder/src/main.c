#include <stdio.h>

#include "bill.h"


static void print_date(
    const char* label,
    Date date)
{
    char buffer[16];

    date_to_string(
        date,
        buffer,
        sizeof(buffer)
    );

    printf(
        "%s%s\n",
        label,
        buffer
    );
}


int main(void)
{
    Bill bill;

    bill_init(&bill);

    bill_set_account_name(
        &bill,
        "Electricity"
    );

    bill_set_provider(
        &bill,
        "MSEDCL"
    );

    bill_set_account_number(
        &bill,
        "123456789"
    );

    bill_set_amount(
        &bill,
        2450.50
    );

    bill_set_due_date(
        &bill,
        date_create(2026, 9, 15)
    );

    bill_set_reminder_days(
        &bill,
        5
    );

    bill_set_email_enabled(
        &bill,
        true
    );

    bill_set_sms_enabled(
        &bill,
        false
    );


    printf(
        "Account        : %s\n",
        bill.account_name
    );

    printf(
        "Provider       : %s\n",
        bill.provider
    );

    printf(
        "Account Number : %s\n",
        bill.account_number
    );

    printf(
        "Amount         : %.2f\n",
        bill.amount
    );

    print_date(
        "Due Date       : ",
        bill.due_date
    );

    print_date(
        "Reminder Date  : ",
        bill_get_reminder_date(&bill)
    );

    printf(
        "Reminder Days  : %d\n",
        bill.reminder_days
    );

    printf(
        "Email          : %s\n",
        bill.email_enabled ? "Enabled" : "Disabled"
    );

    printf(
        "SMS            : %s\n",
        bill.sms_enabled ? "Enabled" : "Disabled"
    );

    printf(
        "Valid          : %s\n",
        bill_is_valid(&bill) ? "YES" : "NO"
    );

    return 0;
}