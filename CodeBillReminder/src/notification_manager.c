#include "notification_manager.h"

bool notification_manager_init(
    NotificationManager* manager,
    const char* email_recipient)
{
    if (manager == NULL ||
        email_recipient == NULL ||
        email_recipient[0] == '\0')
    {
        return false;
    }

    manager->email.recipient = email_recipient;

    return true;
}


bool notification_manager_send(
    NotificationManager* manager,
    const Bill* bill)
{
    if (manager == NULL ||
        bill == NULL)
    {
        return false;
    }

    bool notification_sent = false;

    /*
     * Email notification
     */
    if (bill->email_enabled)
    {
        if (notification_send_email(
            bill,
            &manager->email))
        {
            notification_sent = true;
        }
    }

    /*
     * SMS will be added here in Step 8.
     */

    return notification_sent;
}