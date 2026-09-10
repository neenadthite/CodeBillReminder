#include "notification.h"

#include <stdio.h>

#include "date.h"

bool notification_send_email(
    const Bill *bill,
    const EmailConfig *config)
{
    if (bill == NULL || config == NULL)
    {
        return false;
    }

    if (config->recipient == NULL ||
        config->recipient[0] == '\0')
    {
        return false;
    }

    char date_buffer[16];

    date_to_string(
        bill->due_date,
        date_buffer,
        sizeof(date_buffer));

    printf("\n");
    printf("========================================\n");
    printf("          EMAIL NOTIFICATION\n");
    printf("========================================\n");

    printf("To       : %s\n", config->recipient);

    printf("Subject  : Bill Reminder - %s\n",
           bill->provider);

    printf("\n");

    printf("Hello,\n\n");

    printf("This is a reminder for your upcoming bill.\n\n");

    printf("Account  : %s\n",
           bill->account_name);

    printf("Provider : %s\n",
           bill->provider);

    printf("Amount   : ₹%lld.%02lld\n",
           (long long)(bill->amount_paise / 100),
           (long long)(bill->amount_paise % 100));

    printf("Due Date : %s\n",
           date_buffer);

    printf("\n");
    printf("Please make the payment before the due date.\n");

    printf("========================================\n");

    return true;
}