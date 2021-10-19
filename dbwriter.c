#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mongoc/mongoc.h>
#include <bson/bson.h>
#include "timestamp.h"
#define BUF_SIZE 256
#define USER_ID         "userId"
/*
collection userGaitData
  userId: String,
  startTimestamp: Date,
  endTimestamp: Date,
  gaitSpeed: Number,
  strideLength: Number
*/
typedef struct UserGaitData
{
    char userId[BUF_SIZE];
    struct tm startTime;
    struct tm endTime;
    float gaitSpeed;
    float strideLength;
} UserGaitData;

FILE *logStartTime;
FILE *logElapsedTime;
FILE *logGaitSpeed;
FILE *logStep;

int openLogFiles(void) {
    const char *LOG_START_TIME = "log-start-time.txt";
    const char *LOG_ELAPSED_TIME = "log-elapsed-time.txt";
    const char *LOG_GAIT_SPEED = "log-gait-speed.txt";
    const char *LOG_STEP = "sensor-step.txt";

    startTimeLog = fopen(LOG_START_TIME, "rt");
    if (!logStartTime) {
        return -1;
    }

    logElapsedTime = fopen(LOG_ELAPSED_TIME, "rt");
    if (!logElapsedTime) {
        return -1;
    }

    logGaitSpeed = fopen(LOG_GAIT_SPEED, "rt");
    if (!logGaitSpeed) {
        return -1;
    }

    logStep = fopen(LOG_STEP, "rt");
    if (!logStep) {
        return -1;
    }

    return 0;
}

void readLogFiles(UserGaitData *list) {
    char buffer[BUF_SIZE];
    Timestamp curTime;
    int curElapsedTime, curStep;
    float curGaitSpeed;
    float curStrideLength;
    int curIdx;

    curIdx = 0;
    while (!feof(logStartTime)) {
        fgets(buf, BUF_SIZE, logStartTime);
        fscanf(logElapsedTime, "delta time: %d\n", &curElapsedTime);
        fscanf(logGaitSpeed, "%d\n", &curGaitSpeed);
        fscanf(logStep, "%d\n", &curStep);

        curTime = getTime(buf);
        curStrideLength = (2.0f * curGaitSpeed * (float)curElapsedTime) / (float)curStep;

        strcpy(list[curIdx].userId, USER_ID);
        list[curIdx].startTime = curTime;
        list[curIdx].gaitSpeed = curGaitSpeed;
        list[curIdx].strideLength = curStrideLength;
        list[curIdx].endTime = getEndTime(&curTime, curElapsedTime);
        curIdx++;
    }
}

void closeLogFiles(void) {
    fclose(logStartTime);
    fclose(logElapsedTime);
    fclose(logGaitSpeed);
    fclose(logStep);
}

int main(int argc, char *argv[])
{
    const char *uri_string = "mongodb://localhost:27017";
    mongoc_uri_t *uri;
    mongoc_client_t *client;
    mongoc_database_t *database;
    mongoc_collection_t *collection;
    bson_t *command, reply, *insert;
    bson_error_t error;
    char *str;
    bool retval;

    UserGaitData userDataList[BUF_SIZE];

    if (openLogFiles() == -1) {
        fputs("E: file open error\n", stderr);
        return -1;
    }

    readLogFiles(userDataList);

    while (!feof(logStartTime)) {
        fgets(buffer, BUF_SIZE, logStartTime);
        printf("%s", buffer);
        getTime(buffer);
    }

    closeLogFiles();
    return 0;

    // Required to initialize libmongoc's internals
    mongoc_init();

    // Optionally get MongoDB URI from command line
    if (argc > 1) {
        uri_string = argv[1];
    }

    // Safely create a MongoDB URI object from the given string
    uri = mongoc_uri_new_with_error(uri_string, &error);
    if (!uri)
    {
        fprintf(stderr,
                "failed to parse URI: %s\n"
                "error message:       %s\n",
                uri_string,
                error.message);
        return EXIT_FAILURE;
    }

    // Create a new client instance
    client = mongoc_client_new_from_uri(uri);
    if (!client)
    {
        return EXIT_FAILURE;
    }

    /* Register the application name so we can track it in the profile logs on the server. This can also be done from the URI (see other examples). */
    mongoc_client_set_appname(client, "dbwriter");

    /* Get a handle on the database "db_name" and collection "coll_name" */
    database = mongoc_client_get_database(client, "gaitdb");
    collection = mongoc_client_get_collection(client, "gaitdb", "userGaitData");

    /* Do work. This example pings the database, prints the result as JSON and performs an insert */
    command = BCON_NEW("ping", BCON_INT32(1));

    retval = mongoc_client_command_simple(client, "admin", command, NULL, &reply, &error);

    if (!retval)
    {
        fprintf(stderr, "%s\n", error.message);
        return EXIT_FAILURE;
    }

    str = bson_as_json(&reply, NULL);
    printf("%s\n", str);

    insert = BCON_NEW("hello", BCON_UTF8("world"));

    if (!mongoc_collection_insert_one(collection, insert, NULL, NULL, &error))
    {
        fprintf(stderr, "%s\n", error.message);
    }

    bson_destroy(insert);
    bson_destroy(&reply);
    bson_destroy(command);
    bson_free(str);

    // Release our handles and clean up libmongoc
    mongoc_collection_destroy(collection);
    mongoc_database_destroy(database);
    mongoc_uri_destroy(uri);
    mongoc_client_destroy(client);
    mongoc_cleanup();

    return EXIT_SUCCESS;
}