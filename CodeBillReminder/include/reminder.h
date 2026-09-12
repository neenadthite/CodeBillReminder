#ifndef REMINDER_H
#define REMINDER_H

#include <stdbool.h>
#include <stddef.h>
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

typedef bool (*ReminderCallback)(
    const Bill* bill,
    void* context
    );

size_t reminder_process_bills(
    Bill* bills,
    size_t count,
    ReminderCallback callback,
    void* context
);

#endif