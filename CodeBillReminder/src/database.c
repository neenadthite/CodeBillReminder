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
        "amount REAL NOT NULL,"
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