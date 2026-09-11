#ifndef EMAIL_CONFIG_H
#define EMAIL_CONFIG_H

#include <stdbool.h>

#include "notification.h"

bool email_config_load(
    const char *filename,
    EmailConfig *config
);

#endif