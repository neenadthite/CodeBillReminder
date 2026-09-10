#include <stdio.h>

#include "bill.h"
#include "bill_manager.h"
#include "reminder.h"


static bool reminder_callback(
    const Bill* bill,
    void* context)
{
    (void)context;

    printf("\nREMINDER REQUIRED\n");

    printf("Account  : %s\n",
        bill->account_name);

    printf("Provider : %s\n",
        bill->provider);

    printf("Amount   : %lld.%02lld\n",
        (long long)(bill->amount_paise / 100),
        (long long)(bill->amount_paise % 100));

    printf("Email    : %s\n",
        bill->email_enabled
        ? "Enabled"
        : "Disabled");

    printf("SMS      : %s\n",
        bill->sms_enabled
        ? "Enabled"
        : "Disabled");

    return true;
}


static void create_bill(
    Bill* bill,
    const char* account,
    const char* provider,
    int amount_paise,
    Date due_date,
    int reminder_days,
    bool email,
    bool sms)
{
    bill_init(bill);

    bill_set_account_name(
        bill,
        account
    );

    bill_set_provider(
        bill,
        provider
    );

    bill_set_account_number(
        bill,
        "TEST123"
    );

    bill_set_amount_paise(
        bill,
        amount_paise
    );

    bill_set_due_date(
        bill,
        due_date
    );

    bill_set_reminder_days(
        bill,
        reminder_days
    );

    bill_set_email_enabled(
        bill,
        email
    );

    bill_set_sms_enabled(
        bill,
        sms
    );

    bill_set_status(
        bill,
        BILL_STATUS_PENDING
    );
}


int main(void)
{
    BillManager manager;

    if (!bill_manager_init(&manager))
    {
        printf("BillManager initialization failed.\n");
        return 1;
    }

    /*
     * Bill 1
     *
     * Today = 2026-09-10
     * Due = 2026-09-15
     * Reminder = 5 days
     *
     * Reminder date = today
     */
    Bill bill1;

    create_bill(
        &bill1,
        "Netflix",
        "Netflix India",
        79999,
        date_create(2026, 9, 15),
        5,
        true,
        true
    );

    /*
     * Bill 2
     *
     * Reminder is in the future.
     */
    Bill bill2;

    create_bill(
        &bill2,
        "Electricity",
        "MSEDCL",
        250000,
        date_create(2026, 9, 20),
        5,
        true,
        false
    );

    /*
     * Bill 3
     *
     * Reminder date is today,
     * but no notification channel.
     */
    Bill bill3;

    create_bill(
        &bill3,
        "Internet",
        "ISP",
        99900,
        date_create(2026, 9, 15),
        5,
        false,
        false
    );

    bill_manager_add(&manager, &bill1);
    bill_manager_add(&manager, &bill2);
    bill_manager_add(&manager, &bill3);

    printf("BillManager count: %zu\n",
        bill_manager_count(&manager));

    printf("\nProcessing reminders...\n");

    size_t reminder_count =
        reminder_process_bills(
            bill_manager_data(&manager),
            bill_manager_count(&manager),
            reminder_callback,
            NULL
        );

    printf("\nTotal reminders required: %zu\n",
        reminder_count);

    bill_manager_free(&manager);

    return 0;
}