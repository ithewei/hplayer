#ifndef DB_H_INCLUDED
#define DB_H_INCLUDED

#include "sqlite3.h"
#include <vector>
#include <string>
#include <map>
using namespace std;

#define SQL_OK SQLITE_OK
#define SQL_ERR SQLITE_ERROR

typedef sqlite3* HDB;
typedef int (*db_callback)(void* userdata, int argc, char *col_values[], char *col_names[]);
typedef std::map<std::string, std::string> key_value;
typedef key_value dbRecord;
typedef std::vector<dbRecord> dbRecords;

int db_open(const char* dbname, HDB* phdb);
int db_close(HDB hdb);

int db_exec_no_result(HDB hdb, const char* sql);
int db_exec_with_result(HDB hdb, const char* sql, dbRecords* records);
int db_exec_cb(HDB hdb, const char* sql, db_callback cb, void* userdata);

// select count(*) from table;
int dbtable_select_count(HDB hdb, const char* table);
// select col_names from table where condition;
int dbtable_select(HDB hdb, const char* table, const char* col_names, const char* where, dbRecords* records, const char* limit = NULL);
// insert into table (col_names) values (col_values);
int dbtable_insert(HDB hdb, const char* table, const char* col_names, const char* col_values);
// update table set changes where condition;
int dbtable_update(HDB hdb, const char* table, const char* changes, const char* where);
// delete from table where condition;
int dbtable_delete(HDB hdb, const char* table, const char* where);

#endif // DB_H_INCLUDED
