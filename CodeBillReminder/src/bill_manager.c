#include "bill_manager.h"

#include "bill_ui.h"

#include <stdio.h>
#include <stdlib.h>


#define INITIAL_CAPACITY 4


bool bill_manager_init(
    BillManager* manager)
{
    if (manager == NULL)
    {
        return false;
    }

    manager->bills = malloc(
        INITIAL_CAPACITY * sizeof(Bill)
    );

    if (manager->bills == NULL)
    {
        return false;
    }

    manager->count = 0;
    manager->capacity = INITIAL_CAPACITY;

    return true;
}


void bill_manager_free(
    BillManager* manager)
{
    if (manager == NULL)
    {
        return;
    }

    free(manager->bills);

    manager->bills = NULL;
    manager->count = 0;
    manager->capacity = 0;
}


static bool bill_manager_grow(
    BillManager* manager)
{
    size_t new_capacity =
        manager->capacity * 2;

    Bill* new_bills = realloc(
        manager->bills,
        new_capacity * sizeof(Bill)
    );

    if (new_bills == NULL)
    {
        return false;
    }

    manager->bills = new_bills;
    manager->capacity = new_capacity;

    return true;
}


bool bill_manager_add(
    BillManager* manager,
    const Bill* bill)
{
    if (manager == NULL ||
        bill == NULL)
    {
        return false;
    }

    if (!bill_is_valid(bill))
    {
        return false;
    }

    if (manager->count >= manager->capacity)
    {
        if (!bill_manager_grow(manager))
        {
            return false;
        }
    }

    Bill new_bill = *bill;

    manager->bills[
        manager->count
    ] = new_bill;

    manager->count++;

    return true;
}


Bill* bill_manager_get(
    BillManager* manager,
    int id)
{
    if (manager == NULL)
    {
        return NULL;
    }

    for (size_t i = 0;
        i < manager->count;
        i++)
    {
        if (manager->bills[i].id == id)
        {
            return &manager->bills[i];
        }
    }

    return NULL;
}


bool bill_manager_remove(
    BillManager* manager,
    int id)
{
    if (manager == NULL)
    {
        return false;
    }

    for (size_t i = 0;
        i < manager->count;
        i++)
    {
        if (manager->bills[i].id == id)
        {
            for (size_t j = i;
                j < manager->count - 1;
                j++)
            {
                manager->bills[j] =
                    manager->bills[j + 1];
            }

            manager->count--;

            return true;
        }
    }

    return false;
}


size_t bill_manager_count(
    const BillManager* manager)
{
    if (manager == NULL)
    {
        return 0;
    }

    return manager->count;
}


void bill_manager_print_all(
    const BillManager* manager)
{
    if (manager == NULL)
    {
        return;
    }

    if (manager->count == 0)
    {
        printf("\nNo bills available.\n");
        return;
    }

    printf(
        "\n========== All Bills ==========\n"
    );

    for (size_t i = 0;
        i < manager->count;
        i++)
    {
        bill_ui_print(
            &manager->bills[i]
        );
    }
}

bool bill_manager_add_to_database(
    BillManager* manager,
    Database* database,
    Bill* bill)
{
    if (manager == NULL ||
        database == NULL ||
        bill == NULL)
    {
        return false;
    }

    if (!bill_is_valid(bill))
    {
        return false;
    }

    /*
     * Store the bill in SQLite first.
     *
     * SQLite generates the persistent ID.
     */
    if (!database_insert_bill(database, bill))
    {
        return false;
    }

    /*
     * database_insert_bill() has now
     * populated bill->id.
     *
     * Add the same bill to RAM.
     */
    if (!bill_manager_add(manager, bill))
    {
        /*
         * Important:
         *
         * The database insert succeeded,
         * but RAM insertion failed.
         *
         * Roll back the database row.
         */
        database_delete_bill(
            database,
            bill->id
        );

        return false;
    }

    return true;
}