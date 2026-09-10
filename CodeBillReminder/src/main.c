#include <stdio.h>

#include "database.h"


int main(void)
{
    Database database = { 0 };

    printf("Opening database...\n");

    if (!database_open(
        &database,
        "database/bills.db"))
    {
        printf(
            "Failed to open database.\n"
        );

        return 1;
    }

    printf(
        "Database opened successfully.\n"
    );


    printf(
        "Initializing database...\n"
    );

    if (!database_initialize(
        &database))
    {
        printf(
            "Failed to initialize database.\n"
        );

        database_close(&database);

        return 1;
    }


    printf(
        "Database initialized successfully.\n"
    );


    database_close(&database);

    printf(
        "Database closed.\n"
    );

    return 0;
}