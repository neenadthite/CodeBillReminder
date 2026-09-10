#include <stdio.h>

#include "database.h"
#include "bill_manager.h"

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

    if (!bill_manager_init(&manager))
    {
        printf("BillManager initialization failed.\n");

        database_close(&database);
        return 1;
    }

    printf("BillManager initialized successfully.\n");

    printf("\nLoading bills from database...\n");

    if (!bill_manager_load_from_database(
        &manager,
        &database))
    {
        printf("Failed to load bills from database.\n");

        bill_manager_free(&manager);
        database_close(&database);

        return 1;
    }

    printf("Bills loaded successfully.\n");

    printf("BillManager count: %zu\n",
        bill_manager_count(&manager));

    bill_manager_print_all(&manager);

    bill_manager_free(&manager);

    database_close(&database);

    printf("\nDatabase and BillManager closed.\n");

    return 0;
}