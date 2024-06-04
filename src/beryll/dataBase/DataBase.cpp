#include "DataBase.h"
#include "beryll/core/Log.h"

namespace Beryll
{
    sqlite3* DataBase::m_DB = nullptr;
    sqlite3_stmt* DataBase::m_stmt = nullptr;

    void DataBase::openDataBase(const std::string& name)
    {
        std::string internalStoragePath;
#if defined(ANDROID)
        internalStoragePath = SDL_AndroidGetInternalStoragePath();
#elif defined(APPLE)

#else
        BR_ASSERT(false, "%s", "Can not find internal storage. Unknown platform.");
#endif

        if(internalStoragePath.empty())
        {
            std::string errorMessage = "Can not find internal store path for database.";
            throw DataBaseException(errorMessage);
        }

        internalStoragePath += "/";
        internalStoragePath += name;
        BR_INFO("DataBase path: %s", internalStoragePath.c_str());

        // The database is opened for reading and writing and is created if it does not already exist.
        // To drop a database, delete the file on drive.
        //sqlite3_close(m_DB);
        if(!m_DB)
        {
            BR_INFO("%s", "Database object == nullptr. Creating it.");

            int errorCode = sqlite3_open(internalStoragePath.c_str(), &m_DB);

            if(errorCode != SQLITE_OK)
            {
                std::string errorMessage = "Error in openDataBase(): ";
                errorMessage += sqlite3_errmsg(m_DB);
                throw DataBaseException(errorMessage);
            }
        }
        else
        {
            BR_INFO("%s", "Database object created. Use it.");
        }
    }

    void DataBase::setSqlQuery(const std::string& sqlQuery)
    {
        BR_ASSERT((m_DB != nullptr), "%s", "Database object == nullptr. Call openDataBase() first.");

        sqlite3_finalize(m_stmt);

        int errorCode = sqlite3_prepare_v2(m_DB, sqlQuery.c_str(), sqlQuery.size(), &m_stmt, nullptr);
        if(errorCode != SQLITE_OK)
        {
            std::string errorMessage = "Error in setSqlQuery(): ";
            errorMessage += sqlite3_errmsg(m_DB);
            throw DataBaseException(errorMessage);
        }
    }

    void DataBase::bindParameterLongLongInt(const char* paramName, const long long int paramValue)
    {
        BR_ASSERT((m_DB != nullptr), "%s", "Database object == nullptr. Call openDataBase() first.");

        int paramIndex = sqlite3_bind_parameter_index(m_stmt, paramName);
        if(paramIndex == 0)
        {
            std::string errorMessage = "Can not find parameter ";
            errorMessage += paramName;
            throw DataBaseException(errorMessage);
        }

        int errorCode = sqlite3_bind_int64(m_stmt, paramIndex, paramValue);
        if(errorCode != SQLITE_OK)
        {
            std::string errorMessage = "Error in bindParameterInt(): ";
            errorMessage += sqlite3_errmsg(m_DB);
            throw DataBaseException(errorMessage);
        }
    }

    void DataBase::bindParameterDouble(const char* paramName, const double paramValue)
    {
        BR_ASSERT((m_DB != nullptr), "%s", "Database object == nullptr. Call openDataBase() first.");

        int paramIndex = sqlite3_bind_parameter_index(m_stmt, paramName);
        if(paramIndex == 0)
        {
            std::string errorMessage = "Can not find parameter ";
            errorMessage += paramName;
            throw DataBaseException(errorMessage);
        }

        int errorCode = sqlite3_bind_double(m_stmt, paramIndex, paramValue);
        if(errorCode != SQLITE_OK)
        {
            std::string errorMessage = "Error in bindParameterDouble(): ";
            errorMessage += sqlite3_errmsg(m_DB);
            throw DataBaseException(errorMessage);
        }
    }

    void DataBase::bindParameterString(const char* paramName, const std::string& paramValue)
    {
        BR_ASSERT((m_DB != nullptr), "%s", "Database object == nullptr. Call openDataBase() first.");

        int paramIndex = sqlite3_bind_parameter_index(m_stmt, paramName);
        if(paramIndex == 0)
        {
            std::string errorMessage = "Can not find parameter ";
            errorMessage += paramName;
            throw DataBaseException(errorMessage);
        }

        int errorCode = sqlite3_bind_text(m_stmt, paramIndex, paramValue.c_str(), paramValue.size(), SQLITE_TRANSIENT);
        if(errorCode != SQLITE_OK)
        {
            std::string errorMessage = "Error in bindParameterString(): ";
            errorMessage += sqlite3_errmsg(m_DB);
            throw DataBaseException(errorMessage);
        }
    }

    void DataBase::executeNotSelectQuery()
    {
        BR_ASSERT((m_DB != nullptr), "%s", "Database object == nullptr. Call openDataBase() first.");

        int errorCode = sqlite3_step(m_stmt);
        sqlite3_reset(m_stmt);

        if(errorCode != SQLITE_DONE)
        {
            std::string errorMessage = "Error in executeNotSelectQuery(): ";
            errorMessage += sqlite3_errmsg(m_DB);
            throw DataBaseException(errorMessage);
        }
    }

    std::vector<std::vector<std::variant<long long int, double, std::string, SqliteNULL>>> DataBase::executeSelectQuery()
    {
        BR_ASSERT((m_DB != nullptr), "%s", "Database object == nullptr. Call openDataBase() first.");

        std::vector<std::vector<std::variant<long long int, double, std::string, SqliteNULL>>> rows;

        while(true)
        {
            int errorCode = sqlite3_step(m_stmt);

            if(errorCode != SQLITE_ROW && errorCode != SQLITE_DONE)
            {
                sqlite3_reset(m_stmt);

                std::string errorMessage = "Error in executeSelectQuery(): ";
                errorMessage += sqlite3_errmsg(m_DB);
                throw DataBaseException(errorMessage);
            }

            if(rows.size() > 5000)
            {
                BR_WARN("%s", "Database module fetched more than 5 000 rows. Probably error.")
            }

            if(errorCode == SQLITE_DONE)
            {
                sqlite3_reset(m_stmt);

                return rows;
            }

            // row is set of columns.
            // Each column can be: long long int, double, std::string, NULL.
            std::vector<std::variant<long long int, double, std::string, SqliteNULL>> row;
            row.reserve(sqlite3_data_count(m_stmt));

            for(int i = 0; i < sqlite3_data_count(m_stmt); ++i)
            {
                int columnType = sqlite3_column_type(m_stmt, i);

                if(columnType == SQLITE_NULL)
                {
                    row.emplace_back(SqliteNULL{});
                }
                else if(columnType == SQLITE_INTEGER)
                {
                    row.emplace_back(sqlite3_column_int64(m_stmt, i));
                }
                else if(columnType == SQLITE_FLOAT)
                {
                    row.emplace_back(sqlite3_column_double(m_stmt, i));
                }
                else if(columnType == SQLITE_TEXT)
                {
                    row.emplace_back(std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_stmt, i))));
                }
                else if(columnType == SQLITE_BLOB)
                {
                    // BLOB is for a binary large object that is a collection of binary data stored as a value in the database.
                    // By using the BLOB, you can store the documents, images, and other multimedia files in the database.
                }
            }

            rows.push_back(row);
        }
    }
}
