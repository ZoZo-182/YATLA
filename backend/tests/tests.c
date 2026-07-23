#include "../lib/munit/munit.h"
#include "../include/request_handling.h"
#include "../include/user_db.h"
#include <sodium.h>
#include <sqlite3.h>
#include <stdio.h>

#define UNUSED(x) (void)(x)

static ConnInfo user = {NULL, "Bob", "Smith", "chickenbirriataco@gmail.com", "yummy"};


static MunitResult test_pwhash(const MunitParameter params[], void* user_data_or_fixture) {
  UNUSED(params);
  UNUSED(user_data_or_fixture);

  char *result = hash_password(user.password);

  munit_assert_not_null(result);
  munit_assert_string_not_equal(result, "");

  free(result);
  return MUNIT_OK;
}


static MunitResult test_insert_user(const MunitParameter params[], void* user_data_or_fixture) {
  UNUSED(params);

  sqlite3 *db = (sqlite3*)user_data_or_fixture;
  munit_assert_not_null(db);

  status_t result = insert_user(db, &user);
  munit_assert_int(SUCCESS, ==, result);

  return MUNIT_OK;
}

static MunitResult test_check_user(const MunitParameter params[], void* user_data_or_fixture) {
  UNUSED(params);

  sqlite3 *db = (sqlite3*)user_data_or_fixture;
  munit_assert_not_null(db);

  status_t insert_result = insert_user(db, &user);
  munit_assert_int(SUCCESS, ==, insert_result);
  
  status_t correct_login = check_user(db, &user); 
  munit_assert_int(SUCCESS, ==, correct_login);

  return MUNIT_OK;
}

static void* test_setup(const MunitParameter params[], void* user_dat) {
  UNUSED(params);
  UNUSED(user_dat);

  char *err_msg = NULL;

  sqlite3 *test_db;
  int rc = sqlite3_open(":memory:", &test_db);
  if (rc != SQLITE_OK) {
    if (test_db != NULL) {
      fprintf(stderr, "error opening db: %s\n", sqlite3_errmsg(test_db));
      sqlite3_close(test_db);
    }
    return NULL;
  }

  const char *sql = "CREATE TABLE IF NOT EXISTS users ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "first_name TEXT NOT NULL,"
    "last_name TEXT NOT NULL,"
    "email TEXT UNIQUE NOT NULL,"
    "password TEXT NOT NULL)";

  rc = sqlite3_exec(test_db, sql, 0, 0, &err_msg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Error when trying to execute (SQL): %s\n", err_msg);
    sqlite3_free(err_msg);
    sqlite3_close(test_db);
    return NULL;
  } 
  return test_db;
}

static void test_tear_down(void *fixture) {
  sqlite3 *test_db = (sqlite3*)fixture;
  if (test_db != NULL) {
    sqlite3_close(test_db);
  }
}

static MunitTest tests[] = {
  {
    "/hash-pw-test",
    test_pwhash,
    NULL,
    NULL,
    MUNIT_TEST_OPTION_NONE,
    NULL
  },
  {
    "/insert-user-test",
    test_insert_user,
    test_setup,
    test_tear_down,
    MUNIT_TEST_OPTION_NONE,
    NULL
  },
  {
    "/check-user-test",
    test_check_user,
    test_setup,
    test_tear_down,
    MUNIT_TEST_OPTION_NONE,
    NULL
  },
  {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite suite = {
  "/my-tests",
  tests,
  NULL,
  1,
  MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char* argv[]) {
  if (sodium_init() < 0) {
    fprintf(stderr, "sodium lib not initalized\n");
  }
  return munit_suite_main(&suite, NULL, argc, argv);
}
