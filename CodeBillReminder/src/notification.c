#include "notification.h"

#include <stdio.h>
#include <string.h>

#include <curl/curl.h>

#include "date.h"


typedef struct
{
    const char* data;
    size_t size;
    size_t position;

} EmailPayload;


static size_t email_read_callback(
    char* buffer,
    size_t size,
    size_t number_of_elements,
    void* userdata)
{
    EmailPayload* payload =
        (EmailPayload*)userdata;

    size_t buffer_size =
        size * number_of_elements;

    if (payload == NULL ||
        payload->data == NULL)
    {
        return 0;
    }

    size_t remaining =
        payload->size - payload->position;

    size_t copy_size =
        remaining < buffer_size
        ? remaining
        : buffer_size;

    if (copy_size > 0)
    {
        memcpy(
            buffer,
            payload->data + payload->position,
            copy_size);

        payload->position += copy_size;
    }

    return copy_size;
}


bool notification_send_email(
    const Bill* bill,
    const EmailConfig* config)
{
    if (bill == NULL ||
        config == NULL)
    {
        return false;
    }

    if (config->smtp_host == NULL ||
        config->username == NULL ||
        config->password == NULL ||
        config->sender == NULL ||
        config->recipient == NULL)
    {
        return false;
    }


    /*
     * Create date string.
     */

    char date_buffer[16];

    date_to_string(
        bill->due_date,
        date_buffer,
        sizeof(date_buffer));


    /*
     * Create email body.
     *
     * UTF-8 encoded rupee symbol.
     */

    char email_body[2048];

    int body_length = snprintf(
        email_body,
        sizeof(email_body),

        "From: %s\r\n"
        "To: %s\r\n"
        "Subject: Bill Reminder - %s\r\n"
        "MIME-Version: 1.0\r\n"
        "Content-Type: text/plain; charset=UTF-8\r\n"
        "\r\n"

        "Hello,\r\n"
        "\r\n"

        "This is a reminder for your upcoming bill.\r\n"
        "\r\n"

        "Account  : %s\r\n"
        "Provider : %s\r\n"
        "Amount   : \xE2\x82\xB9%lld.%02lld\r\n"
        "Due Date : %s\r\n"
        "\r\n"

        "Please make the payment before the due date.\r\n",

        config->sender,
        config->recipient,
        bill->provider,

        bill->account_name,
        bill->provider,

        (long long)(bill->amount_paise / 100),
        (long long)(bill->amount_paise % 100),

        date_buffer);


    if (body_length < 0 ||
        (size_t)body_length >= sizeof(email_body))
    {
        return false;
    }


    EmailPayload payload =
    {
        .data = email_body,
        .size = (size_t)body_length,
        .position = 0
    };


    /*
     * Initialize libcurl.
     */

    CURL* curl = curl_easy_init();

    if (curl == NULL)
    {
        return false;
    }


    /*
     * Gmail SMTP server.
     */

    char smtp_url[256];

    int url_length = snprintf(
        smtp_url,
        sizeof(smtp_url),
        "smtp://%s:%d",
        config->smtp_host,
        config->smtp_port);

    if (url_length < 0 ||
        (size_t)url_length >= sizeof(smtp_url))
    {
        curl_easy_cleanup(curl);
        return false;
    }


    /*
     * Configure SMTP connection.
     */

    curl_easy_setopt(
        curl,
        CURLOPT_URL,
        smtp_url);

    curl_easy_setopt(
        curl,
        CURLOPT_USERNAME,
        config->username);

    curl_easy_setopt(
        curl,
        CURLOPT_PASSWORD,
        config->password);


    /*
     * Sender.
     */

    curl_easy_setopt(
        curl,
        CURLOPT_MAIL_FROM,
        config->sender);


    /*
     * Recipient list.
     */

    struct curl_slist* recipients = NULL;

    recipients = curl_slist_append(
        recipients,
        config->recipient);

    if (recipients == NULL)
    {
        curl_easy_cleanup(curl);
        return false;
    }

    curl_easy_setopt(
        curl,
        CURLOPT_MAIL_RCPT,
        recipients);


    /*
     * Email data.
     */

    curl_easy_setopt(
        curl,
        CURLOPT_READFUNCTION,
        email_read_callback);

    curl_easy_setopt(
        curl,
        CURLOPT_READDATA,
        &payload);

    curl_easy_setopt(
        curl,
        CURLOPT_UPLOAD,
        1L);


    /*
     * Gmail STARTTLS.
     */

    curl_easy_setopt(
        curl,
        CURLOPT_USE_SSL,
        (long)CURLUSESSL_ALL);


    /*
     * Enable verbose output temporarily.
     *
     * This is useful during development to see
     * the SMTP/TLS negotiation.
     */

    curl_easy_setopt(
        curl,
        CURLOPT_VERBOSE,
        1L);


    /*
     * Perform SMTP transaction.
     */

    CURLcode result =
        curl_easy_perform(curl);


    /*
     * Cleanup.
     */

    curl_slist_free_all(recipients);

    curl_easy_cleanup(curl);


    if (result != CURLE_OK)
    {
        printf(
            "Email sending failed: %s\n",
            curl_easy_strerror(result));

        return false;
    }


    printf(
        "Email sent successfully to %s\n",
        config->recipient);

    return true;
}