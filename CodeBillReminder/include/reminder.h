#ifndef REMINDER_H
#define REMINDER_H

#include <stdbool.h>

#include "bill.h"

/*
 * Check whether a bill's reminder date
 * is today.
 */
bool reminder_is_due(
    const Bill *bill
);

/*
 * Check whether the bill is overdue.
 */
bool reminder_is_overdue(
    const Bill *bill
);

/*
 * Check whether any notification is enabled.
 */
bool reminder_has_notification_channel(
    const Bill *bill
);

#endif