#include <stdio.h>
#include <stdint.h>

#include "database.h"
#include "bill_manager.h"
#include "reminder.h"
#include "bill.h"
#include "date.h"

static bool reminder_callback(
    const Bill* bill,
    void* context)
{
    (void)context;

    char date_buffer[16];

    date_to_string(
        bill->due_date,
        date_buffer,
        sizeof(date_buffer));

    printf("\nREMINDER REQUIRED\n");

    printf("Account  : %s\n", bill->account_name);
    printf("Provider : %s\n", bill->provider);

    printf("Amount   : %lld.%02lld\n",
        (long long)(bill->amount_paise / 100),
        (long long)(bill->amount_paise % 100));

    printf("Due Date : %s\n", date_buffer);

    printf("Email    : %s\n",
        bill->email_enabled ? "Enabled" : "Disabled");

    printf("SMS      : %s\n",
        bill->sms_enabled ? "Enabled" : "Disabled");

    return true;
}


static bool create_bill(
    Bill* bill,
    const char* account,
    const char* provider,
    const char* account_number,
    int64_t amount_paise,
    Date due_date,
    int reminder_days,
    bool email_enabled,
    bool sms_enabled)
{
    bill_init(bill);

    if (!bill_set_account_name(bill, account))
        return false;

    if (!bill_set_provider(bill, provider))
        return false;

    if (!bill_set_account_number(bill, account_number))
        return false;

    if (!bill_set_amount_paise(bill, amount_paise))
        return false;

    if (!bill_set_due_date(bill, due_date))
        return false;

    if (!bill_set_reminder_days(bill, reminder_days))
        return false;

    bill_set_email_enabled(bill, email_enabled);
    bill_set_sms_enabled(bill, sms_enabled);

    return bill_is_valid(bill);
}


int main(void)
{
    Database database;
    BillManager manager;

    printf("Opening database...\n");

    if (!database_open(
        &database,
        "database/bills.db"))
    {
        printf("Failed to open database.\n");
        return 1;
    }

    if (!database_initialize(&database))
    {
        printf("Failed to initialize database.\n");

        database_close(&database);
        return 1;
    }

    /*
     * ------------------------------------------------
     * Create test bills
     * ------------------------------------------------
     */

    Bill netflix;
    Bill electricity;
    Bill internet;

    /*
     * Today:
     * 2026-09-10
     */

    create_bill(
        &netflix,
        "Netflix",
        "Netflix India",
        "NET123456",
        79999,
        date_create(2026, 9, 15),
        5,
        true,
        true);

    create_bill(
        &electricity,
        "Electricity",
        "MSEDCL",
        "ELEC123456",
        245000,
        date_create(2026, 9, 20),
        5,
        true,
        false);

    create_bill(
        &internet,
        "Internet",
        "JioFiber",
        "JIO123456",
        99900,
        date_create(2026, 9, 15),
        5,
        false,
        false);


    /*
     * ------------------------------------------------
     * Initialize BillManager
     * ------------------------------------------------
     */

    if (!bill_manager_init(&manager))
    {
        printf("Failed to initialize BillManager.\n");

        database_close(&database);
        return 1;
    }


    /*
     * ------------------------------------------------
     * Insert bills into SQLite
     * ------------------------------------------------
     */

    printf("\nInserting test bills...\n");

    if (!bill_manager_add_to_database(
        &manager,
        &database,
        &netflix))
    {
        printf("Failed to insert Netflix bill.\n");
    }

    if (!bill_manager_add_to_database(
        &manager,
        &database,
        &electricity))
    {
        printf("Failed to insert Electricity bill.\n");
    }

    if (!bill_manager_add_to_database(
        &manager,
        &database,
        &internet))
    {
        printf("Failed to insert Internet bill.\n");
    }

    printf("Bills inserted: %zu\n",
        bill_manager_count(&manager));


    /*
     * ------------------------------------------------
     * Free current RAM manager
     * ------------------------------------------------
     *
     * This forces us to prove that the next step
     * actually loads the data from SQLite.
     */

    bill_manager_free(&manager);


    /*
     * ------------------------------------------------
     * Create a fresh BillManager
     * ------------------------------------------------
     */

    if (!bill_manager_init(&manager))
    {
        printf("Failed to initialize second BillManager.\n");

        database_close(&database);
        return 1;
    }


    /*
     * ------------------------------------------------
     * Load bills FROM SQLite
     * ------------------------------------------------
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
     * ------------------------------------------------
     * Process reminders
     * ------------------------------------------------
     */

    printf("\nProcessing reminders...\n");

    size_t reminder_count =
        reminder_process_bills(
            bill_manager_data(&manager),
            bill_manager_count(&manager),
            reminder_callback,
            NULL);

    printf("\nTotal reminders required: %zu\n",
        reminder_count);


    /*
     * ------------------------------------------------
     * Cleanup
     * ------------------------------------------------
     */

    bill_manager_free(&manager);

    database_close(&database);

    printf("\nDatabase closed.\n");

    return 0;
}