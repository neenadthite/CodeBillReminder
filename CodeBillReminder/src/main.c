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

    if (!database_initialize(&database))
    {
        printf("Database initialization failed.\n");

        database_close(&database);
        return 1;
    }

    printf("Database initialized successfully.\n");

    if (!bill_manager_init(&manager))
    {
        printf("BillManager initialization failed.\n");

        database_close(&database);
        return 1;
    }

    printf("BillManager initialized successfully.\n");

    /*
     * Load database into RAM
     */
    printf("\nLoading bills from database...\n");

    if (!bill_manager_load_from_database(
        &manager,
        &database))
    {
        printf("Failed to load bills.\n");

        bill_manager_free(&manager);
        database_close(&database);

        return 1;
    }

    printf("Bills loaded successfully.\n");
    printf("BillManager count: %zu\n",
        bill_manager_count(&manager));

    /*
     * Get bill ID 3
     */
    Bill* bill =
        bill_manager_get(
            &manager,
            3
        );

    if (bill == NULL)
    {
        printf("Bill ID 3 not found.\n");

        bill_manager_free(&manager);
        database_close(&database);

        return 1;
    }

    printf("\nBefore UPDATE:\n");
    bill_ui_print(bill);

    /*
     * Modify RAM copy
     */
    bill_set_provider(
        bill,
        "Netflix Premium"
    );

    bill_set_amount_paise(
        bill,
        99999
    );

    bill_set_reminder_days(
        bill,
        7
    );

    printf("\nUpdating Bill ID %d...\n",
        bill->id);

    if (!bill_manager_update_in_database(
        &manager,
        &database,
        bill))
    {
        printf("Bill update failed.\n");

        bill_manager_free(&manager);
        database_close(&database);

        return 1;
    }

    printf("Bill updated successfully.\n");

    /*
     * Display RAM copy
     */
    printf("\nAfter UPDATE:\n");

    bill_ui_print(bill);

    /*
     * Delete
     */
    printf("\nDeleting Bill ID %d...\n",
        bill->id);

    if (!bill_manager_remove_from_database(
        &manager,
        &database,
        bill->id))
    {
        printf("Bill deletion failed.\n");

        bill_manager_free(&manager);
        database_close(&database);

        return 1;
    }

    printf("Bill deleted successfully.\n");

    printf("BillManager count: %zu\n",
        bill_manager_count(&manager));

    /*
     * Cleanup
     */
    bill_manager_free(&manager);

    database_close(&database);

    printf("\nDatabase and BillManager closed.\n");

    return 0;
}