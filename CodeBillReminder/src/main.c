#include <stdio.h>

#include "database.h"
#include "bill.h"
#include "bill_ui.h"

int main(void)
{
    Database database = { 0 };

    printf("Opening database...\n");

    if (!database_open(&database, "database/bills.db"))
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
     * Retrieve existing bill
     */
    Bill bill;

    printf("\nRetrieving bill ID 3...\n");

    if (!database_get_bill(&database, 3, &bill))
    {
        printf("Failed to retrieve bill.\n");

        database_close(&database);
        return 1;
    }

    printf("Bill retrieved successfully.\n");

    printf("\nBefore UPDATE:\n");

    bill_ui_print(&bill);

    /*
     * Modify bill
     */
    printf("\nModifying bill...\n");

    bill_set_provider(
        &bill,
        "Netflix India"
    );

    bill_set_amount_paise(
        &bill,
        79999
    );

    bill_set_reminder_days(
        &bill,
        5
    );

    bill_set_email_enabled(
        &bill,
        false
    );

    bill_set_sms_enabled(
        &bill,
        true
    );

    printf("\nUpdating bill ID %d...\n",
        bill.id);

    if (database_update_bill(&database, &bill))
    {
        printf("Bill updated successfully.\n");
    }
    else
    {
        printf("Bill update failed.\n");

        database_close(&database);
        return 1;
    }

    /*
     * Read it back from SQLite
     */
    Bill updated_bill;

    printf("\nRetrieving updated bill...\n");

    if (database_get_bill(
        &database,
        bill.id,
        &updated_bill))
    {
        printf("Updated bill retrieved successfully.\n");

        bill_ui_print(&updated_bill);
    }
    else
    {
        printf("Failed to retrieve updated bill.\n");
    }

    database_close(&database);

    printf("\nDatabase closed.\n");

    return 0;
}