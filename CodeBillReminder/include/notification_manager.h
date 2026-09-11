#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H

#include <stdbool.h>

#include "bill.h"
#include "notification.h"

typedef struct
{
    EmailConfig email;

} NotificationManager;

bool notification_manager_init(
    NotificationManager* manager,
    const EmailConfig* email_config
);

bool notification_manager_send(
    NotificationManager* manager,
    const Bill* bill
);

#endif