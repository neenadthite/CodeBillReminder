#include <stdio.h>

#include "bill.h"
#include "reminder.h"


int main(void)
{
    Bill bill;

    bill_init(&bill);

    bill_set_account_name(
        &bill,
        "Netflix"
    );

    bill_set_provider(
        &bill,
        "Netflix India"
    );

    bill_set_account_number(
        &bill,
        "NET123456"
    );

    bill_set_amount_paise(
        &bill,
        99999
    );

    /*
     * Today:
     * 2026-09-10
     *
     * Due:
     * 2026-09-15
     *
     * Reminder:
     * 5 days before due date
     *
     * Reminder date:
     * 2026-09-10
     */
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
        true
    );

    bill_set_status(
        &bill,
        BILL_STATUS_PENDING
    );


    printf("Reminder date test\n");
    printf("------------------\n");

    Date reminder_date =
        bill_get_reminder_date(&bill);

    char date_string[11];

    date_to_string(
        reminder_date,
        date_string,
        sizeof(date_string)
    );

    printf("Reminder date : %s\n",
        date_string);

    printf(
        "Reminder due  : %s\n",
        reminder_is_due(&bill)
        ? "YES"
        : "NO"
    );

    printf(
        "Overdue       : %s\n",
        reminder_is_overdue(&bill)
        ? "YES"
        : "NO"
    );

    printf(
        "Notification   : %s\n",
        reminder_has_notification_channel(&bill)
        ? "YES"
        : "NO"
    );

    return 0;
}