#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <stdbool.h>

#include "bill.h"

typedef struct
{
    const char* smtp_host;
    int smtp_port;

    const char* username;
    const char* password;

    const char* sender;
    const char* recipient;

} EmailConfig;

bool notification_send_email(
    const Bill* bill,
    const EmailConfig* config
);

#endif