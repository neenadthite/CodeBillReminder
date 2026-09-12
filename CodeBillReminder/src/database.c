#include "database.h"

#include <stdio.h>

#include "sqlite3.h"

bool database_open(
    Database* database,
    const char* filename)
{
    if (database == NULL ||
        filename == NULL)
    {
        return false;
    }

    sqlite3* db = NULL;

    int result =
        sqlite3_open(filename, &db);

    if (result != SQLITE_OK)
    {
        if (db != NULL)
        {
            fprintf(
                stderr,
                "SQLite open error: %s\n",
                sqlite3_errmsg(db)
            );

            sqlite3_close(db);
        }

        return false;
    }

    database->handle = db;

    return true;
}

void database_close(
    Database* database)
{
    if (database == NULL ||
        database->handle == NULL)
    {
        return;
    }

    sqlite3* db =
        (sqlite3*)database->handle;

    sqlite3_close(db);

    database->handle = NULL;
}

static bool database_has_reminder_sent_column(sqlite3* db)
{
    const char* sql =
        "PRAGMA table_info(bills);";

    sqlite3_stmt* statement = NULL;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        return false;
    }

    bool found = false;

    while (sqlite3_step(statement) == SQLITE_ROW)
    {
        const unsigned char* column_name =
            sqlite3_column_text(statement, 1);

        if (column_name != NULL &&
            strcmp(
                (const char*)column_name,
                "reminder_sent") == 0)
        {
            found = true;
            break;
        }
    }

    sqlite3_finalize(statement);

    return found;
}   

bool database_initialize(
    Database* database)
{
    if (database == NULL ||
        database->handle == NULL)
    {
        return false;
    }

    sqlite3* db =
        (sqlite3*)database->handle;

    const char* sql =
        "CREATE TABLE IF NOT EXISTS bills ("
        "id INTEGER PRIMARY KEY,"
        "account_name TEXT NOT NULL,"
        "provider TEXT NOT NULL,"
        "account_number TEXT NOT NULL,"
        "amount_paise INTEGER NOT NULL,"
        "due_date TEXT NOT NULL,"
        "reminder_days INTEGER NOT NULL,"
        "email_enabled INTEGER NOT NULL,"
        "sms_enabled INTEGER NOT NULL,"
        "status INTEGER NOT NULL,"
        "reminder_sent INTEGER NOT NULL DEFAULT 0"
        ");";

    char* error_message = NULL;

    int result = sqlite3_exec(
        db,
        sql,
        NULL,
        NULL,
        &error_message
    );

    if (result != SQLITE_OK)
    {
        fprintf(
            stderr,
            "Database initialization error: %s\n",
            error_message
        );

        sqlite3_free(error_message);

        return false;
    }

    if (!database_has_reminder_sent_column(db))
    {
        const char* migration_sql =
            "ALTER TABLE bills "
            "ADD COLUMN reminder_sent "
            "INTEGER NOT NULL DEFAULT 0;";

        char* migration_error = NULL;

        result = sqlite3_exec(
            db,
            migration_sql,
            NULL,
            NULL,
            &migration_error
        );

        if (result != SQLITE_OK)
        {
            fprintf(
                stderr,
                "Database migration error: %s\n",
                migration_error
            );

            sqlite3_free(migration_error);

            return false;
        }

        printf(
            "Database migrated: reminder_sent column added.\n");
    }

    return true;
}

bool database_insert_bill(Database* database, Bill* bill)
{
    if (database == NULL ||
        database->handle == NULL ||
        bill == NULL)
    {
        return false;
    }

    sqlite3* db = (sqlite3*)database->handle;

    const char* sql =
        "INSERT INTO bills ("
        "account_name, "
        "provider, "
        "account_number, "
        "amount_paise, "
        "due_date, "
        "reminder_days, "
        "email_enabled, "
        "sms_enabled, "
        "status, "
        "reminder_sent"
        ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* statement = NULL;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        printf("Failed to prepare INSERT statement: %s\n",
            sqlite3_errmsg(db));

        return false;
    }

    char due_date_string[11];

    date_to_string(
        bill->due_date,
        due_date_string,
        sizeof(due_date_string)
    );

    sqlite3_bind_text(
        statement,
        1,
        bill->account_name,
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        statement,
        2,
        bill->provider,
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        statement,
        3,
        bill->account_number,
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_int64(
        statement,
        4,
        bill->amount_paise
    );

    sqlite3_bind_text(
        statement,
        5,
        due_date_string,
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_int(
        statement,
        6,
        bill->reminder_days
    );

    sqlite3_bind_int(
        statement,
        7,
        bill->email_enabled ? 1 : 0
    );

    sqlite3_bind_int(
        statement,
        8,
        bill->sms_enabled ? 1 : 0
    );

    sqlite3_bind_int(
        statement,
        9,
        (int)bill->status
    );

    sqlite3_bind_int(
        statement,
        10,
        bill->reminder_sent ? 1 : 0
    );

    result = sqlite3_step(statement);

    if (result != SQLITE_DONE)
    {
        printf("Failed to insert bill: %s\n",
            sqlite3_errmsg(db));

        sqlite3_finalize(statement);

        return false;
    }

    bill->id = (int)sqlite3_last_insert_rowid(db);

    sqlite3_finalize(statement);

    return true;
}

bool database_get_bill(Database* database, int id, Bill* bill)
{
    if (database == NULL ||
        database->handle == NULL ||
        bill == NULL ||
        id <= 0)
    {
        return false;
    }

    sqlite3* db = (sqlite3*)database->handle;

    const char* sql =
        "SELECT "
        "id, "
        "account_name, "
        "provider, "
        "account_number, "
        "amount_paise, "
        "due_date, "
        "reminder_days, "
        "email_enabled, "
        "sms_enabled, "
        "status "
        "FROM bills "
        "WHERE id = ?;";

    sqlite3_stmt* statement = NULL;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        printf("Failed to prepare SELECT statement: %s\n",
            sqlite3_errmsg(db));

        return false;
    }

    result = sqlite3_bind_int(
        statement,
        1,
        id
    );

    if (result != SQLITE_OK)
    {
        printf("Failed to bind bill ID: %s\n",
            sqlite3_errmsg(db));

        sqlite3_finalize(statement);

        return false;
    }

    result = sqlite3_step(statement);

    if (result == SQLITE_ROW)
    {
        bill_init(bill);

        bill->id = sqlite3_column_int(statement, 0);

        const unsigned char* account_name =
            sqlite3_column_text(statement, 1);

        const unsigned char* provider =
            sqlite3_column_text(statement, 2);

        const unsigned char* account_number =
            sqlite3_column_text(statement, 3);

        sqlite3_int64 amount_paise =
            sqlite3_column_int64(statement, 4);

        const unsigned char* due_date =
            sqlite3_column_text(statement, 5);

        int reminder_days =
            sqlite3_column_int(statement, 6);

        int email_enabled =
            sqlite3_column_int(statement, 7);

        int sms_enabled =
            sqlite3_column_int(statement, 8);

        int status =
            sqlite3_column_int(statement, 9);

        if (account_name == NULL ||
            provider == NULL ||
            account_number == NULL ||
            due_date == NULL)
        {
            printf("Invalid NULL data in database.\n");

            sqlite3_finalize(statement);

            return false;
        }

        if (!bill_set_account_name(
            bill,
            (const char*)account_name))
        {
            sqlite3_finalize(statement);
            return false;
        }

        if (!bill_set_provider(
            bill,
            (const char*)provider))
        {
            sqlite3_finalize(statement);
            return false;
        }

        if (!bill_set_account_number(
            bill,
            (const char*)account_number))
        {
            sqlite3_finalize(statement);
            return false;
        }

        if (!bill_set_amount_paise(
            bill,
            (int64_t)amount_paise))
        {
            sqlite3_finalize(statement);
            return false;
        }

        if (!date_from_string(
            (const char*)due_date,
            &bill->due_date))
        {
            printf("Invalid date stored in database.\n");

            sqlite3_finalize(statement);

            return false;
        }

        if (!bill_set_reminder_days(
            bill,
            reminder_days))
        {
            sqlite3_finalize(statement);
            return false;
        }

        bill_set_email_enabled(
            bill,
            email_enabled != 0
        );

        bill_set_sms_enabled(
            bill,
            sms_enabled != 0
        );

        if (status < BILL_STATUS_PENDING ||
            status > BILL_STATUS_OVERDUE)
        {
            printf("Invalid bill status in database.\n");

            sqlite3_finalize(statement);

            return false;
        }

        bill_set_status(
            bill,
            (BillStatus)status
        );

        sqlite3_finalize(statement);

        return true;
    }

    if (result == SQLITE_DONE)
    {
        printf("Bill with ID %d was not found.\n", id);
    }
    else
    {
        printf("Failed to execute SELECT: %s\n",
            sqlite3_errmsg(db));
    }

    sqlite3_finalize(statement);

    return false;
}

bool database_update_bill(Database* database, const Bill* bill)
{
    if (database == NULL ||
        database->handle == NULL ||
        bill == NULL ||
        bill->id <= 0)
    {
        return false;
    }

    sqlite3* db = (sqlite3*)database->handle;

    const char* sql =
        "UPDATE bills SET "
        "account_name = ?, "
        "provider = ?, "
        "account_number = ?, "
        "amount_paise = ?, "
        "due_date = ?, "
        "reminder_days = ?, "
        "email_enabled = ?, "
        "sms_enabled = ?, "
        "status = ? "
        "WHERE id = ?;";

    sqlite3_stmt* statement = NULL;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        printf("Failed to prepare UPDATE statement: %s\n",
            sqlite3_errmsg(db));

        return false;
    }

    char due_date_string[11];

    date_to_string(
        bill->due_date,
        due_date_string,
        sizeof(due_date_string)
    );

    result = sqlite3_bind_text(
        statement,
        1,
        bill->account_name,
        -1,
        SQLITE_TRANSIENT
    );

    if (result != SQLITE_OK)
    {
        goto bind_error;
    }

    result = sqlite3_bind_text(
        statement,
        2,
        bill->provider,
        -1,
        SQLITE_TRANSIENT
    );

    if (result != SQLITE_OK)
    {
        goto bind_error;
    }

    result = sqlite3_bind_text(
        statement,
        3,
        bill->account_number,
        -1,
        SQLITE_TRANSIENT
    );

    if (result != SQLITE_OK)
    {
        goto bind_error;
    }

    result = sqlite3_bind_int64(
        statement,
        4,
        bill->amount_paise
    );

    if (result != SQLITE_OK)
    {
        goto bind_error;
    }

    result = sqlite3_bind_text(
        statement,
        5,
        due_date_string,
        -1,
        SQLITE_TRANSIENT
    );

    if (result != SQLITE_OK)
    {
        goto bind_error;
    }

    result = sqlite3_bind_int(
        statement,
        6,
        bill->reminder_days
    );

    if (result != SQLITE_OK)
    {
        goto bind_error;
    }

    result = sqlite3_bind_int(
        statement,
        7,
        bill->email_enabled ? 1 : 0
    );

    if (result != SQLITE_OK)
    {
        goto bind_error;
    }

    result = sqlite3_bind_int(
        statement,
        8,
        bill->sms_enabled ? 1 : 0
    );

    if (result != SQLITE_OK)
    {
        goto bind_error;
    }

    result = sqlite3_bind_int(
        statement,
        9,
        (int)bill->status
    );

    if (result != SQLITE_OK)
    {
        goto bind_error;
    }

    result = sqlite3_bind_int(
        statement,
        10,
        bill->id
    );

    if (result != SQLITE_OK)
    {
        goto bind_error;
    }

    result = sqlite3_step(statement);

    if (result != SQLITE_DONE)
    {
        printf("Failed to update bill: %s\n",
            sqlite3_errmsg(db));

        sqlite3_finalize(statement);

        return false;
    }

    if (sqlite3_changes(db) == 0)
    {
        printf("No bill was updated. ID %d may not exist.\n",
            bill->id);

        sqlite3_finalize(statement);

        return false;
    }

    sqlite3_finalize(statement);

    return true;

bind_error:

    printf("Failed to bind UPDATE parameter: %s\n",
        sqlite3_errmsg(db));

    sqlite3_finalize(statement);

    return false;
}

bool database_delete_bill(Database* database, int id)
{
    if (database == NULL ||
        database->handle == NULL ||
        id <= 0)
    {
        return false;
    }

    sqlite3* db = (sqlite3*)database->handle;

    const char* sql =
        "DELETE FROM bills "
        "WHERE id = ?;";

    sqlite3_stmt* statement = NULL;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        printf("Failed to prepare DELETE statement: %s\n",
            sqlite3_errmsg(db));

        return false;
    }

    result = sqlite3_bind_int(
        statement,
        1,
        id
    );

    if (result != SQLITE_OK)
    {
        printf("Failed to bind DELETE ID: %s\n",
            sqlite3_errmsg(db));

        sqlite3_finalize(statement);

        return false;
    }

    result = sqlite3_step(statement);

    if (result != SQLITE_DONE)
    {
        printf("Failed to delete bill: %s\n",
            sqlite3_errmsg(db));

        sqlite3_finalize(statement);

        return false;
    }

    if (sqlite3_changes(db) == 0)
    {
        printf("No bill found with ID %d.\n", id);

        sqlite3_finalize(statement);

        return false;
    }

    sqlite3_finalize(statement);

    return true;
}

bool database_get_all_bills(
    Database* database,
    DatabaseBillCallback callback,
    void* context)
{
    if (database == NULL ||
        database->handle == NULL ||
        callback == NULL)
    {
        return false;
    }

    sqlite3* db = (sqlite3*)database->handle;

    const char* sql =
        "SELECT "
        "id, "
        "account_name, "
        "provider, "
        "account_number, "
        "amount_paise, "
        "due_date, "
        "reminder_days, "
        "email_enabled, "
        "sms_enabled, "
        "status "
        "FROM bills "
        "ORDER BY id;";

    sqlite3_stmt* statement = NULL;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        NULL
    );

    if (result != SQLITE_OK)
    {
        printf("Failed to prepare SELECT ALL statement: %s\n",
            sqlite3_errmsg(db));

        return false;
    }

    while ((result = sqlite3_step(statement)) == SQLITE_ROW)
    {
        Bill bill;

        bill_init(&bill);

        bill.id =
            sqlite3_column_int(statement, 0);

        const unsigned char* account_name =
            sqlite3_column_text(statement, 1);

        const unsigned char* provider =
            sqlite3_column_text(statement, 2);

        const unsigned char* account_number =
            sqlite3_column_text(statement, 3);

        sqlite3_int64 amount_paise =
            sqlite3_column_int64(statement, 4);

        const unsigned char* due_date =
            sqlite3_column_text(statement, 5);

        int reminder_days =
            sqlite3_column_int(statement, 6);

        int email_enabled =
            sqlite3_column_int(statement, 7);

        int sms_enabled =
            sqlite3_column_int(statement, 8);

        int status =
            sqlite3_column_int(statement, 9);

        if (account_name == NULL ||
            provider == NULL ||
            account_number == NULL ||
            due_date == NULL)
        {
            printf("Invalid NULL data in database.\n");

            sqlite3_finalize(statement);

            return false;
        }

        if (!bill_set_account_name(
            &bill,
            (const char*)account_name))
        {
            sqlite3_finalize(statement);
            return false;
        }

        if (!bill_set_provider(
            &bill,
            (const char*)provider))
        {
            sqlite3_finalize(statement);
            return false;
        }

        if (!bill_set_account_number(
            &bill,
            (const char*)account_number))
        {
            sqlite3_finalize(statement);
            return false;
        }

        if (!bill_set_amount_paise(
            &bill,
            (int64_t)amount_paise))
        {
            sqlite3_finalize(statement);
            return false;
        }

        if (!date_from_string(
            (const char*)due_date,
            &bill.due_date))
        {
            printf("Invalid date stored in database.\n");

            sqlite3_finalize(statement);

            return false;
        }

        if (!bill_set_reminder_days(
            &bill,
            reminder_days))
        {
            sqlite3_finalize(statement);
            return false;
        }

        bill_set_email_enabled(
            &bill,
            email_enabled != 0
        );

        bill_set_sms_enabled(
            &bill,
            sms_enabled != 0
        );

        if (status < BILL_STATUS_PENDING ||
            status > BILL_STATUS_OVERDUE)
        {
            printf("Invalid bill status in database.\n");

            sqlite3_finalize(statement);

            return false;
        }

        bill_set_status(
            &bill,
            (BillStatus)status
        );

        /*
         * Give the reconstructed Bill
         * to the caller.
         */
        if (!callback(&bill, context))
        {
            printf("Bill callback failed.\n");

            sqlite3_finalize(statement);

            return false;
        }
    }

    if (result != SQLITE_DONE)
    {
        printf("Failed while reading bills: %s\n",
            sqlite3_errmsg(db));

        sqlite3_finalize(statement);

        return false;
    }

    sqlite3_finalize(statement);

    return true;
}