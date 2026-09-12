#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "database.h"
#include "bill_manager.h"
#include "bill_ui.h"
#include "reminder.h"
#include "notification.h"
#include "notification_manager.h"
#include "email_config.h"

typedef struct
{
    NotificationManager* notification_manager;
    Database* database;

} ReminderContext;

static bool reminder_notification_callback(
    const Bill* bill,
    void* context)
{
    ReminderContext* reminder_context =
        (ReminderContext*)context;

    if (reminder_context == NULL ||
        reminder_context->notification_manager == NULL ||
        reminder_context->database == NULL)
    {
        return false;
    }

    if (!notification_manager_send(
        reminder_context->notification_manager,
        bill))
    {
        return false;
    }

    if (!database_mark_reminder_sent(
        reminder_context->database,
        bill->id))
    {
        printf(
            "Warning: Email sent but reminder state "
            "could not be saved for Bill ID %d.\n",
            bill->id);

        return false;
    }

    return true;
}



static void print_menu(void)
{
    printf("\n");
    printf("================================\n");
    printf("         BILL REMINDER\n");
    printf("================================\n");
    printf("1. Add Bill\n");
    printf("2. View Bills\n");
    printf("3. Delete Bill\n");
    printf("4. Check Reminders\n");
    printf("5. Exit\n");
    printf("================================\n");
    printf("Select option: ");
}

int main(int argc, char* argv[]) {

    bool check_reminders_mode = false;

    if (argc > 1)
    {
        if (strcmp(argv[1], "--check-reminders") == 0)
        {
            check_reminders_mode = true;
        }
        else
        {
            printf("Unknown argument: %s\n", argv[1]);
            return 1;
        }
    }
    Database database;
    BillManager manager;

    /*
     * ----------------------------------------
     * Open database
     * ----------------------------------------
     */

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
     * ----------------------------------------
     * Initialize BillManager
     * ----------------------------------------
     */

    if (!bill_manager_init(&manager))
    {
        printf("Failed to initialize BillManager.\n");

        database_close(&database);
        return 1;
    }

    /*
     * ----------------------------------------
     * Load existing bills
     * ----------------------------------------
     */

    if (!bill_manager_load_from_database(
        &manager,
        &database))
    {
        printf("Failed to load bills.\n");

        bill_manager_free(&manager);
        database_close(&database);

        return 1;
    }

    printf("Bills loaded: %zu\n",
        bill_manager_count(&manager));


    /*
     * ----------------------------------------
     * Load email configuration
     * ----------------------------------------
     */

    EmailConfig email_config;

    if (!email_config_load(
        "config/email.conf",
        &email_config))
    {
        printf("Warning: Email configuration could not be loaded.\n");
        printf("Email notifications will not work.\n");
    }


    NotificationManager notification_manager;

    bool notification_manager_ready =
        notification_manager_init(
            &notification_manager,
            &email_config);

    if (check_reminders_mode)
    {
        if (!notification_manager_ready)
        {
            printf("Notification Manager is not ready.\n");

            bill_manager_free(&manager);
            database_close(&database);

            return 1;
        }

        printf("Checking reminders...\n");

        ReminderContext reminder_context;

        reminder_context.notification_manager =
            &notification_manager;

        reminder_context.database =
            &database;

        size_t count =
            reminder_process_bills(
                bill_manager_data(&manager),
                bill_manager_count(&manager),
                reminder_notification_callback,
                &reminder_context);

        printf("Notifications sent: %zu\n", count);

        bill_manager_free(&manager);
        database_close(&database);

        return 0;
    }


    /*
     * ----------------------------------------
     * Main application loop
     * ----------------------------------------
     */

    bool running = true;

    while (running)
    {
        print_menu();

        int option;

        if (scanf(
            "%d",
            &option) != 1)
        {
            printf("Invalid input.\n");

            while (getchar() != '\n')
            {
                /* Clear input buffer */
            }

            continue;
        }

        /*
         * Clear newline after scanf_s.
         */

        while (getchar() != '\n')
        {
            /* Clear input buffer */
        }


        switch (option)
        {
        case 1:
        {
            /*
             * Add Bill
             */

            Bill bill;

            if (!bill_ui_create(&bill))
            {
                printf("\nFailed to create bill.\n");
                break;
            }

            if (!bill_manager_add_to_database(
                &manager,
                &database,
                &bill))
            {
                printf("\nFailed to save bill.\n");
                break;
            }

            printf("\nBill added successfully.\n");
            printf("Bill ID: %d\n",
                bill.id);

            break;
        }


        case 2:
        {
            /*
             * View Bills
             */

            bill_manager_print_all(
                &manager);

            break;
        }


        case 3:
        {
            /*
             * Delete Bill
             */

            int id;

            printf("\nEnter Bill ID to delete: ");

            if (scanf("%d", &id) != 1)
            {
                printf("Invalid Bill ID.\n");

                while (getchar() != '\n')
                {
                    /* Clear input buffer */
                }

                break;
            }

            while (getchar() != '\n')
            {
                /* Clear newline */
            }

            if (bill_manager_remove_from_database(
                &manager,
                &database,
                id))
            {
                printf("\nBill deleted successfully.\n");
            }
            else
            {
                printf("\nFailed to delete Bill ID %d.\n", id);
            }

            break;
        }


        case 4:
        {
            /*
             * Check Reminders
             */

            if (!notification_manager_ready)
            {
                printf(
                    "\nNotification Manager is not ready.\n");

                break;
            }

            printf("\nChecking reminders...\n");

            size_t count =
                reminder_process_bills(
                    bill_manager_data(&manager),
                    bill_manager_count(&manager),
                    reminder_notification_callback,
                    &notification_manager);

            printf(
                "\nNotifications sent: %zu\n",
                count);

            break;
        }


        case 5:
        {
            running = false;
            break;
        }


        default:
        {
            printf(
                "\nInvalid option. Please select 1-5.\n");

            break;
        }


        }
}


    /*
     * ----------------------------------------
     * Cleanup
     * ----------------------------------------
     */

    bill_manager_free(&manager);

    database_close(&database);

    printf("\nApplication closed.\n");

    return 0;
}