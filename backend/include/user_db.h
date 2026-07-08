#ifndef USER_DB_H
#define USER_DB_H

#include <sqlite3.h>
#include "request_handling.h"

#ifdef __cplusplus
extern "C" {
#endif

int is_valid_password(char *password);
char* hash_password(char *password);

status_t insert_user(sqlite3* db, ConnInfo *user_info);
status_t check_user(sqlite3 *db, ConnInfo *user_info);

void destroy_conn_info(ConnInfo *user_info);
// add password + email constraints

#ifdef __cplusplus
}
#endif

#endif
