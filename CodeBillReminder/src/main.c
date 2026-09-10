#include <stdio.h>

#include "database.h"
#include "bill.h"

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

    Bill bill;

    bill_init(&bill);

    bill_set_account_name(&bill, "Netflix");
    bill_set_provider(&bill, "Netflix");
    bill_set_account_number(&bill, "NET123456");
    bill_set_amount_paise(&bill, 64999);

    bill_set_due_date(
        &bill,
        date_create(2026, 9, 20)
    );

    bill_set_reminder_days(&bill, 3);

    bill_set_email_enabled(&bill, true);
    bill_set_sms_enabled(&bill, false);

    bill_set_status(
        &bill,
        BILL_STATUS_PENDING
    );

    printf("\nInserting bill...\n");

    if (database_insert_bill(&database, &bill))
    {
        printf("Bill inserted successfully.\n");
        printf("Generated ID: %d\n", bill.id);
    }
    else
    {
        printf("Bill insertion failed.\n");
    }

    database_close(&database);

    printf("Database closed.\n");

    return 0;
}