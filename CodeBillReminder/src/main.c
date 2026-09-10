#include <stdio.h>

#include "database.h"
#include "bill.h"
#include "bill_manager.h"
#include "bill_ui.h"

int main(void)
{
    Database database = { 0 };
    BillManager manager;

    printf("Opening database...\n");

    if (!database_open(
        &database,
        "database/bills.db"))
    {
        printf("Database open failed.\n");
        return 1;
    }

    printf("Database opened successfully.\n");

    printf("Initializing database...\n");

    if (!database_initialize(&database))
    {
        printf("Database initialization failed.\n");

        database_close(&database);
        return 1;
    }

    printf("Database initialized successfully.\n");

    /*
     * Initialize BillManager
     */
    if (!bill_manager_init(&manager))
    {
        printf("BillManager initialization failed.\n");

        database_close(&database);
        return 1;
    }

    printf("BillManager initialized successfully.\n");

    /*
     * Create bill
     */
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
        true
    );

    bill_set_status(
        &bill,
        BILL_STATUS_PENDING
    );

    /*
     * Add to database + BillManager
     */
    printf("\nAdding bill to database and BillManager...\n");

    if (!bill_manager_add_to_database(
        &manager,
        &database,
        &bill))
    {
        printf("Failed to add bill.\n");

        bill_manager_free(&manager);
        database_close(&database);

        return 1;
    }

    printf("Bill added successfully.\n");

    printf("Generated database ID: %d\n",
        bill.id);

    printf("BillManager count: %zu\n",
        bill_manager_count(&manager));

    /*
     * Print RAM copy
     */
    printf("\nBill stored in BillManager:\n");

    Bill* stored_bill =
        bill_manager_get(
            &manager,
            bill.id
        );

    if (stored_bill != NULL)
    {
        bill_ui_print(stored_bill);
    }

    /*
     * Cleanup
     */
    bill_manager_free(&manager);

    database_close(&database);

    printf("\nDatabase and BillManager closed.\n");

    return 0;
}