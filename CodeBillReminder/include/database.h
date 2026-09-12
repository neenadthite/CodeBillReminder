#ifndef DATABASE_H
#define DATABASE_H

#include <stdbool.h>

#include "bill.h"


typedef struct
{
    void* handle;
} Database;


/*
 * Open database.
 */
bool database_open(
    Database* database,
    const char* filename
);


/*
 * Close database.
 */
void database_close(
    Database* database
);


/*
 * Create database tables.
 */
bool database_initialize(
    Database* database
);


/*
 * Insert a bill.
 */
bool database_insert_bill(
    Database* database,
    Bill* bill
);


/*
 * Delete a bill.
 */
bool database_delete_bill(
    Database* database,
    int id
);


/*
 * Update a bill.
 */
bool database_update_bill(
    Database* database,
    const Bill* bill
);


/*
 * Get a bill by ID.
 */
bool database_get_bill(
    Database* database,
    int id,
    Bill* bill
);

typedef bool (*DatabaseBillCallback)(
    const Bill* bill,
    void* context
    );

bool database_get_all_bills(
    Database* database,
    DatabaseBillCallback callback,
    void* context
);

bool database_mark_reminder_sent(
    Database* database,
    int bill_id
);

#endif