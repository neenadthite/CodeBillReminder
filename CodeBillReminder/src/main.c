#include <stdio.h>

#include "bill_manager.h"
#include "bill_ui.h"

int main(void)
{
    BillManager manager;

    if (!bill_manager_init(&manager))
    {
        printf("BillManager initialization failed.\n");
        return 1;
    }

    printf("BillManager initialized successfully.\n");

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
        79999
    );

    bill_set_due_date(
        &bill,
        date_create(2026, 9, 20)
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

    bill_set_status(
        &bill,
        BILL_STATUS_PENDING
    );

    if (!bill_manager_add(
        &manager,
        &bill))
    {
        printf("Failed to add bill.\n");

        bill_manager_free(&manager);
        return 1;
    }

    printf("Bill added successfully.\n");

    printf("BillManager count: %zu\n",
        bill_manager_count(&manager));

    bill_manager_print_all(&manager);

    bill_manager_free(&manager);

    printf("BillManager freed successfully.\n");

    return 0;
}