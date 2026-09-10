#include "bill_ui.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

static bool read_string(
    const char *prompt,
    char *buffer,
    size_t buffer_size)
{
    if (buffer == NULL || buffer_size == 0)
    {
        return false;
    }

    printf("%s", prompt);

    if (fgets(buffer, (int)buffer_size, stdin) == NULL)
    {
        return false;
    }

    buffer[strcspn(buffer, "\n")] = '\0';

    if (strlen(buffer) == 0)
    {
        return false;
    }

    return true;
}

static bool read_int(
    const char *prompt,
    int *value)
{
    char buffer[64];

    if (value == NULL)
    {
        return false;
    }

    printf("%s", prompt);

    if (fgets(buffer, sizeof(buffer), stdin) == NULL)
    {
        return false;
    }

    char extra;

    if (sscanf(buffer, "%d %c", value, &extra) != 1)
    {
        return false;
    }

    return true;
}
static bool read_double(
    const char *prompt,
    double *value)
{
    char buffer[64];

    if (value == NULL)
    {
        return false;
    }

    printf("%s", prompt);

    if (fgets(buffer, sizeof(buffer), stdin) == NULL)
    {
        return false;
    }

    char extra;

    if (sscanf(buffer, "%lf %c", value, &extra) != 1)
    {
        return false;
    }

    return true;
}

static bool read_yes_no(
    const char *prompt,
    bool *value)
{
    char buffer[16];

    if (value == NULL)
    {
        return false;
    }

    printf("%s (y/n): ", prompt);

    if (fgets(buffer, sizeof(buffer), stdin) == NULL)
    {
        return false;
    }

    if (buffer[0] == 'y' ||
        buffer[0] == 'Y')
    {
        *value = true;
        return true;
    }

    if (buffer[0] == 'n' ||
        buffer[0] == 'N')
    {
        *value = false;
        return true;
    }

    return false;
}

bool bill_ui_create(Bill *bill)
{
    if (bill == NULL)
    {
        return false;
    }

    bill_init(bill);

    char account_name[64];
    char provider[64];
    char account_number[64];

    int year;
    int month;
    int day;

    int reminder_days;

    bool email_enabled;
    bool sms_enabled;
    printf("\n");
    printf("========== Add Bill ==========\n\n");

    if (!read_string(
        "Account Name   : ",
        account_name,
        sizeof(account_name)))
    {
        return false;
    }

    if (!read_string(
        "Provider       : ",
        provider,
        sizeof(provider)))
    {
        return false;
    }

    if (!read_string(
        "Account Number : ",
        account_number,
        sizeof(account_number)))
    {
        return false;
    }

    double amount;

    if (!read_double("Amount         : ", &amount))
    {
        return false;
    }

    if (amount < 0.0)
    {
        printf("Amount cannot be negative.\n");
        return false;
    }

    int64_t amount_paise = (int64_t)(amount * 100.0 + 0.5);

    if (!bill_set_amount_paise(bill, amount_paise))
    {
        return false;
    }

    printf("\nDue Date\n");

    if (!read_int(
        "Year           : ",
        &year))
    {
        return false;
    }

    if (!read_int(
        "Month          : ",
        &month))
    {
        return false;
    }

    if (!read_int(
        "Day            : ",
        &day))
    {
        return false;
    }

    Date due_date =
        date_create(year, month, day);

    if (!date_is_valid(due_date))
    {
        printf("Invalid date.\n");
        return false;
    }

    if (!bill_set_due_date(
        bill,
        due_date))
    {
        printf("Failed to set due date.\n");
        return false;
    }

    if (!read_int(
        "\nReminder days before due date : ",
        &reminder_days))
    {
        return false;
    }

    if (!bill_set_reminder_days(
        bill,
        reminder_days))
    {
        printf("Invalid reminder days.\n");
        return false;
    }

    if (!read_yes_no(
        "Enable Email",
        &email_enabled))
    {
        return false;
    }

    if (!read_yes_no(
        "Enable SMS",
        &sms_enabled))
    {
        return false;
    }

    bill_set_email_enabled(
        bill,
        email_enabled);

    bill_set_sms_enabled(
        bill,
        sms_enabled);

    if (!bill_set_account_name(
        bill,
        account_name))
    {
        return false;
    }

    if (!bill_set_provider(
        bill,
        provider))
    {
        return false;
    }

    if (!bill_set_account_number(
        bill,
        account_number))
    {
        return false;
    }

    if (!bill_is_valid(bill))
    {
        printf("\nBill validation failed.\n");
        return false;
    }

    return true;
}

void bill_ui_print(const Bill* bill)
{
    if (bill == NULL)
    {
        return;
    }

    char due_date_string[11];
    char reminder_date_string[11];

    date_to_string(
        bill->due_date,
        due_date_string,
        sizeof(due_date_string)
    );

    Date reminder_date = bill_get_reminder_date(bill);

    date_to_string(
        reminder_date,
        reminder_date_string,
        sizeof(reminder_date_string)
    );

    printf("\n========== Bill ==========\n");

    printf("Account        : %s\n", bill->account_name);
    printf("Provider       : %s\n", bill->provider);
    printf("Account Number : %s\n", bill->account_number);

    printf("Amount         : %lld.%02lld\n",
        (long long)(bill->amount_paise / 100),
        (long long)(bill->amount_paise % 100));

    printf("Due Date       : %s\n", due_date_string);
    printf("Reminder Date  : %s\n", reminder_date_string);
    printf("Reminder Days  : %d\n", bill->reminder_days);

    printf("Email          : %s\n",
        bill->email_enabled ? "Enabled" : "Disabled");

    printf("SMS            : %s\n",
        bill->sms_enabled ? "Enabled" : "Disabled");

    printf("==========================\n");
}