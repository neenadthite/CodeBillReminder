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
     * Retrieve bill ID 3
     */
    Bill bill;

    printf("\nRetrieving bill ID 3...\n");

    if (!database_get_bill(
        &database,
        3,
        &bill))
    {
        printf("Failed to retrieve bill.\n");

        database_close(&database);
        return 1;
    }

    printf("Bill retrieved successfully.\n");

    bill_ui_print(&bill);

    /*
     * Delete bill
     */
    printf("\nDeleting bill ID %d...\n",
        bill.id);

    if (database_delete_bill(
        &database,
        bill.id))
    {
        printf("Bill deleted successfully.\n");
    }
    else
    {
        printf("Bill deletion failed.\n");

        database_close(&database);
        return 1;
    }

    /*
     * Verify deletion
     */
    Bill deleted_bill;

    printf("\nVerifying deletion...\n");

    if (!database_get_bill(
        &database,
        3,
        &deleted_bill))
    {
        printf("Bill ID 3 no longer exists.\n");
    }
    else
    {
        printf("ERROR: Bill ID 3 still exists!\n");
    }

    database_close(&database);

    printf("\nDatabase closed.\n");

    return 0;
}