#ifndef BILL_MANAGER_H
#define BILL_MANAGER_H

#include <stdbool.h>
#include <stddef.h>
#include "database.h"
#include "bill.h"


typedef struct
{
    Bill* bills;

    size_t count;
    size_t capacity;
} BillManager;


/*
 * Initialize manager.
 */
bool bill_manager_init(
    BillManager* manager
);


/*
 * Free manager resources.
 */
void bill_manager_free(
    BillManager* manager
);


/*
 * Add a bill.
 */
bool bill_manager_add(
    BillManager* manager,
    const Bill* bill
);


/*
 * Remove bill by ID.
 */
bool bill_manager_remove(
    BillManager* manager,
    int id
);


/*
 * Find bill by ID.
 */
Bill* bill_manager_get(
    BillManager* manager,
    int id
);


/*
 * Get number of bills.
 */
size_t bill_manager_count(
    const BillManager* manager
);


/*
 * Print all bills.
 */
void bill_manager_print_all(
    const BillManager* manager
);

bool bill_manager_add_to_database(
    BillManager* manager,
    Database* database,
    Bill* bill
);

bool bill_manager_load_from_database(
    BillManager* manager,
    Database* database
);

bool bill_manager_update_in_database(
    BillManager* manager,
    Database* database,
    const Bill* bill
);

bool bill_manager_remove_from_database(
    BillManager* manager,
    Database* database,
    int id
);

const Bill* bill_manager_data(
    const BillManager* manager
);

#endif