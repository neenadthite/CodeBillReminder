#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <stdbool.h>

#include "bill.h"

/*
 * Email notification configuration
 */
typedef struct
{
    const char *recipient;
} EmailConfig;

/*
 * Send an email notification for a bill.
 *
 * Step 7.1:
 * This is a console implementation.
 * Later it will be replaced by an actual
 * email transport.
 */
bool notification_send_email(
    const Bill *bill,
    const EmailConfig *config
);

#endif