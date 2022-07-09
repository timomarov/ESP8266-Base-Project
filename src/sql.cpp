#include "sql.h"
// **************************************************
// ******************* SQL BLOCK ********************
// **************************************************
// попытка вынести подключение MySQL_Generic.h в HEADER файл приводит к ошибке линковщика !!!
#include <MySQL_Generic.h>
MySQL_Connection conn((Client *)&client);
MySQL_Query *query_mem;
IPAddress sqlIp(10, 11, 12, 250);

void sendSqlQuery(String sql)
{
    char *sqlUser = &config.sqlUser[0];
    char *sqlPwd = &config.sqlPwd[0];
    Serial.print(F("SQL: Connection to SQL server... "));
    // if (conn.connect(server, server_port, user, password))
    if (conn.connectNonBlocking(sqlIp, config.sqlPort, sqlUser, sqlPwd) != RESULT_FAIL)
    {
        Serial.println("[OK]");
        delay(500);
        MySQL_Query query_mem = MySQL_Query(&conn); // Initiate the query class instance
        if (conn.connected())
        {
            Serial.print(F("SQL: Send query ("));
            Serial.print(sql);
            Serial.print(F(") "));
            if (!query_mem.execute(sql.c_str())) // Execute the query, KH, check if valid before fetching
            {
                Serial.println(F("[FAIL]"));
            }
            else
            {
                Serial.println(F("[OK]"));
            }
        }
        else
        {
            Serial.println(F("Disconnected from Server."));
        }
        conn.close();
    }
    else
    {
        Serial.println(F("[FAIL]"));
    }
}