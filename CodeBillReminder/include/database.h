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

#endif