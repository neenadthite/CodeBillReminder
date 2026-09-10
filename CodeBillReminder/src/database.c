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
        "status INTEGER NOT NULL"
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
        "status"
        ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";

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