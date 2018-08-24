#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int db_open(const char* dbname, HDB* phdb){
    if (sqlite3_open(dbname, phdb) != SQLITE_OK){
        //loge("sqlite3_open %s failed!", dbname);
        return SQL_ERR;
    }

    return SQL_OK;
}

int db_close(HDB hdb){
    return sqlite3_close(hdb);
}

int db_exec_no_result(HDB hdb, const char* sql){
    char *errmsg;
    //logi("sql: %s", sql);
    if (sqlite3_exec(hdb, sql, NULL, NULL, &errmsg) != SQLITE_OK){
        //loge("sqlite3_exec sql: %s err: %s", sql, errmsg);
        return SQL_ERR;
    }
    return SQL_OK;
}

int db_exec_with_result(HDB hdb, const char* sql, dbRecords* records){
    int row, col;
    char **results;
    char *errmsg;
    //logi("sql: %s", sql);
    if (sqlite3_get_table(hdb, sql, &results, &row, &col, &errmsg) != SQLITE_OK){
        //loge("sqlite3_get_table sql: %s err: %s", sql, errmsg);
        return SQL_ERR;
    }

    // convert char ** to dbRecords
    for (int r = 1; r <= row; ++r){ // note: row = head + data_row;
        dbRecord record;
        for (int c = 0; c < col; ++c){
            record[results[c]] = results[r*col + c];
        }
        records->push_back(record);
    }
    sqlite3_free_table(results);
    return SQL_OK;
}

int db_exec_cb(HDB hdb, const char* sql, db_callback cb, void* userdata){
    char *errmsg;
    //logi("sql: %s", sql);
    if (sqlite3_exec(hdb, sql, cb, userdata, &errmsg) != SQLITE_OK){
        //loge("sqlite3_exec sql: %s err: %s", sql, errmsg);
        return SQL_ERR;
    }
    return SQL_OK;
}
/////////////////////////////////////////////////////////////////////////////////
// select count(*) from table;
int dbtable_select_count(HDB hdb, const char* table){
    char sql[64];
    sprintf(sql ,"select count(*) from %s;", table);
    dbRecords records;
    if (db_exec_with_result(hdb, sql, &records) == SQL_OK)
        return atoi(records[0]["count(*)"].c_str());

    return 0;
}

// select col_names from table where condition;
int dbtable_select(HDB hdb, const char* table, const char* col_names, const char* where, dbRecords* records, const char* limit){
    if (!table || !records)
        return SQL_ERR;

    char sql[128];
    strcpy(sql, "select ");
    if (col_names && strlen(col_names) > 0){
        strcat(sql, col_names);
    }else{
        strcat(sql, "*");
    }
    strcat(sql, " from ");
    strcat(sql, table);
    if (where && strlen(where) > 0){
        strcat(sql, " where ");
        strcat(sql, where);
    }
    if (limit && strlen(limit) > 0){
        strcat(sql, " limit ");
        strcat(sql, limit);
    }
    strcat(sql, ";");
    return db_exec_with_result(hdb, sql, records);
}

// insert into table (col_names) values (col_values);
int dbtable_insert(HDB hdb, const char* table, const char* col_names, const char* col_values){
    if (!table || !col_values)
        return SQL_ERR;

    char sql[1024];
    if (col_names && strlen(col_names) > 0){
        sprintf(sql, "insert into %s (%s) values (%s);", table, col_names, col_values);
    }else{
        sprintf(sql, "insert into %s values (%s);", table, col_values);
    }
    return db_exec_no_result(hdb, sql);
}

// update table set changes where condition;
int dbtable_update(HDB hdb, const char* table, const char* changes, const char* where){
    if (!table || !changes)
        return SQL_ERR;

    char sql[1024];
    if (where && strlen(where) > 0){
        sprintf(sql, "update %s set %s where %s;", table, changes, where);
    }else{
        sprintf(sql,  "update %s set %s;", table, changes);
    }
    return db_exec_no_result(hdb, sql);
}

// delete from table where condition;
int dbtable_delete(HDB hdb, const char* table, const char* where){
    if (!table)
        return SQL_ERR;

    char sql[128];
    if (where && strlen(where) > 0){
        sprintf(sql, "delete from %s where %s;", table, where);
    }else{
        sprintf(sql, "delete * from %s;", table);
    }
    return db_exec_no_result(hdb, sql);
}
////////////////////////////////////////////////////////////////////////////////
