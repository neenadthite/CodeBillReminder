#include <stdio.h>
#include <stdint.h>
#include <curl/curl.h>

#include "notification_manager.h"
#include "database.h"
#include "bill_manager.h"
#include "reminder.h"
#include "bill.h"
#include "date.h"
#include "notification.h"
#include "email_config.h"


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
    printf("libcurl version: %s\n", curl_version());
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
    Date today = date_today();

    Date netflix_due_date =
        date_add_days(today, 5);

    Date electricity_due_date =
        date_add_days(today, 10);

    Date internet_due_date =
        date_add_days(today, 5);

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
        netflix_due_date,
        5,
        true,
        true);

    create_bill(
        &electricity,
        "Electricity",
        "MSEDCL",
        "ELEC123456",
        245000,
        electricity_due_date,
        5,
        true,
        false);

    create_bill(
        &internet,
        "Internet",
        "JioFiber",
        "JIO123456",
        99900,
        internet_due_date,
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

    EmailConfig email_config;

    if (!email_config_load(
        "config/email.conf",
        &email_config))
    {
        printf("Failed to load email configuration.\n");

        bill_manager_free(&manager);
        database_close(&database);

        return 1;
    }

    NotificationManager notification_manager;

    if (!notification_manager_init(
        &notification_manager,
        &email_config))
    {
        printf("Failed to initialize Notification Manager.\n");

        bill_manager_free(&manager);
        database_close(&database);

        return 1;
    }

    
    /*
     * ------------------------------------------------
     * Process reminders
     * ------------------------------------------------
     */

    printf("\nProcessing reminders...\n");

    size_t notification_count =
        reminder_process_bills(
            bill_manager_data(&manager),
            bill_manager_count(&manager),
            reminder_notification_callback,
            &notification_manager);

    printf("\nNotifications sent: %zu\n",
        notification_count);

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

static bool reminder_notification_callback(
    const Bill* bill,
    void* context)
{
    NotificationManager* manager =
        (NotificationManager*)context;

    return notification_manager_send(
        manager,
        bill);
}

static bool clear_test_bills(Database* database)
{
    const int test_ids[] = { 1, 2, 3, 4, 5, 6 };

    for (size_t i = 0;
        i < sizeof(test_ids) / sizeof(test_ids[0]);
        i++)
    {
        database_delete_bill(
            database,
            test_ids[i]);
    }

    return true;
}