#include "email_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONFIG_LINE_SIZE 256

static void trim_newline(char *string)
{
    if (string == NULL)
    {
        return;
    }

    string[strcspn(string, "\r\n")] = '\0';
}


static bool parse_value(
    const char* line,
    const char* key,
    char* output,
    size_t output_size)
{
    size_t key_length = strlen(key);

    if (strncmp(line, key, key_length) != 0)
    {
        return false;
    }

    if (line[key_length] != '=')
    {
        return false;
    }

    const char* value =
        line + key_length + 1;

    size_t value_length =
        strlen(value);

    if (value_length >= output_size)
    {
        return false;
    }

    memcpy(
        output,
        value,
        value_length + 1);

    return true;
}

bool email_config_load(
    const char *filename,
    EmailConfig *config)
{
    if (filename == NULL ||
        config == NULL)
    {
        return false;
    }

    FILE *file = fopen(
        filename,
        "r");

    if (file == NULL)
    {
        return false;
    }

    static char smtp_host[128];
    static char username[128];
    static char password[256];
    static char sender[128];
    static char recipient[128];

    int smtp_port = 0;

    char line[CONFIG_LINE_SIZE];

    while (fgets(
        line,
        sizeof(line),
        file) != NULL)
    {
        trim_newline(line);

        if (parse_value(
                line,
                "SMTP_HOST",
                smtp_host,
                sizeof(smtp_host)))
        {
            continue;
        }

        if (parse_value(
                line,
                "SMTP_USERNAME",
                username,
                sizeof(username)))
        {
            continue;
        }

        if (parse_value(
                line,
                "SMTP_PASSWORD",
                password,
                sizeof(password)))
        {
            continue;
        }

        if (parse_value(
                line,
                "EMAIL_FROM",
                sender,
                sizeof(sender)))
        {
            continue;
        }

        if (parse_value(
                line,
                "EMAIL_TO",
                recipient,
                sizeof(recipient)))
        {
            continue;
        }

        if (strncmp(
                line,
                "SMTP_PORT=",
                10) == 0)
        {
            smtp_port = atoi(
                line + 10);
        }
    }

    fclose(file);

    if (smtp_host[0] == '\0' ||
        username[0] == '\0' ||
        password[0] == '\0' ||
        sender[0] == '\0' ||
        recipient[0] == '\0' ||
        smtp_port == 0)
    {
        return false;
    }

    config->smtp_host = smtp_host;
    config->smtp_port = smtp_port;
    config->username = username;
    config->password = password;
    config->sender = sender;
    config->recipient = recipient;

    return true;
}