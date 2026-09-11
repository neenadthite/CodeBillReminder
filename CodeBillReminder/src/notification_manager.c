#include "notification_manager.h"

bool notification_manager_init(
    NotificationManager* manager,
    const EmailConfig* email_config)
{
    if (manager == NULL ||
        email_config == NULL)
    {
        return false;
    }

    manager->email = *email_config;

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