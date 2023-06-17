#pragma once

#include "CppHeaders.h"
#include "LibsHeaders.h"

#include "sqlite/sqlite3.h"

namespace Beryll
{
    class SqliteNULL{}; // Represent NULL value in std::variant<...> after SELECT.

    class DataBaseException : public std::runtime_error
    {
    public:
        DataBaseException(const std::string& what) : std::runtime_error(what) {}
        ~DataBaseException() override {}
    };

    // https://www.sqlite.org/lang_createtable.html
    // 1. SQLite uses dynamic typing. Content can be stored as INTEGER, REAL, TEXT, BLOB, NULL.
    // 2. A column declared INTEGER PRIMARY KEY will autoincrement. You should pass NULL to this column.
    // 3. SQLite is threadsafe only if compiled with the SQLITE_THREADSAFE preprocessor macro set to 1.
    // 4. SQL specifies that single-quotes in strings are escaped by putting two single quotes in a row.
    //    Example: INSERT INTO xyz VALUES('5 O''clock');
    // 5. Prefer list column names when INSERT.
    //    Example: "INSERT INTO BattlesProgress(ID, MaxAvailableBattle) VALUES(NULL, 4);".

    // This wrapper can store and read only 3 types: INTEGER(int), REAL(float), TEXT(std::string).
    // Empty object of SqliteNULL will returned if sqlite cell store NULL.
    // Throw DataBaseException in case of errors.
    class DataBase final
    {
    public:
        DataBase() = delete;
        ~DataBase() = delete;

        // First openDataBase, then setSqlQuery, then bind params if need, then execute.
        static void openDataBase(const std::string& name);

        static void setSqlQuery(const std::string& sqlQuery);

        // After every execution parameters are reset.
        // You should bind them before every execution.
        static void bindParameterInt(const char* paramName, const int paramValue);
        static void bindParameterFloat(const char* paramName, const float paramValue);
        static void bindParameterString(const char* paramName, const std::string& paramValue);

        // Executes query without return values.
        static void executeNotSelectQuery();

        // Return values if some rows was fetched from DB.
        // First vector represent row.
        // Second vector represent set of columns in row.
        // Every column can has type: int, float, std::string, NULL
        static std::vector<std::vector<std::variant<int, float, std::string, SqliteNULL>>> executeSelectQuery();

    private:
        static sqlite3* m_DB;
        static sqlite3_stmt* m_stmt;
    };
}

