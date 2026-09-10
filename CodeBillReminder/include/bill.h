#ifndef BILL_H
#define BILL_H

#include <stdbool.h>
#include <stdint.h>
#include "date.h"


/*
 * Bill payment status
 */
typedef enum
{
    BILL_STATUS_PENDING,
    BILL_STATUS_PAID,
    BILL_STATUS_OVERDUE

} BillStatus;


/*
 * Bill information
 */
typedef struct
{
    int id;

    char account_name[64];
    char provider[64];
    char account_number[64];

    int64_t amount_paise;

    Date due_date;

    int reminder_days;

    bool email_enabled;
    bool sms_enabled;

    BillStatus status;

} Bill;


/*
 * Initialize a Bill
 */
void bill_init(Bill* bill);


/*
 * Setters
 */
bool bill_set_account_name(
    Bill* bill,
    const char* name
);

bool bill_set_provider(
    Bill* bill,
    const char* provider
);

bool bill_set_account_number(
    Bill* bill,
    const char* account_number
);

bool bill_set_amount_paise(
    Bill* bill,
    int64_t amount_paise
);

bool bill_set_due_date(
    Bill* bill,
    Date due_date
);

bool bill_set_reminder_days(
    Bill* bill,
    int days
);

void bill_set_email_enabled(
    Bill* bill,
    bool enabled
);

void bill_set_sms_enabled(
    Bill* bill,
    bool enabled
);

void bill_set_status(
    Bill* bill,
    BillStatus status
);


/*
 * Validation
 */
bool bill_is_valid(
    const Bill* bill
);


/*
 * Calculate reminder date
 */
Date bill_get_reminder_date(
    const Bill* bill
);


/*
 * Calculate days remaining until due date
 */
int bill_days_until_due(
    const Bill* bill
);

#endif