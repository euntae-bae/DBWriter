#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mongoc/mongoc.h>
#include <bson/bson.h>
#include "timestamp.h"
#define BUF_SIZE 256
#define USER_ID         "userid"
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
int listLength;

void retrieveUserGaitData(const UserGaitData *list, int len) {
    int i;
    for (i = 0; i < len; i++) {
        printf("[%03d] %s\t", i, list[i].userId);
        displayTm(&list[i].startTime);
        printf("\t");
        displayTm(&list[i].endTime);
        printf("\t");
        printf("%f\t%f\n", list[i].gaitSpeed, list[i].strideLength);
    }
    printf("total: %d entries\n", i);
}

int openLogFiles(void) {
    const char *LOG_START_TIME = "log-start-time.txt";
    const char *LOG_ELAPSED_TIME = "log-elapsed-time.txt";
    const char *LOG_GAIT_SPEED = "log-gait-speed.txt";
    const char *LOG_STEP = "sensor-step.txt";

    logStartTime = fopen(LOG_START_TIME, "rt");
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
    char buf[BUF_SIZE];
    Timestamp curStartTime, curEndTime;
    int curElapsedTime, curStep;
    float curGaitSpeed;
    float curStrideLength;

    listLength = 0;
    while (!feof(logStartTime)) {
        fgets(buf, BUF_SIZE, logStartTime);
        fscanf(logElapsedTime, "delta time: %d\n", &curElapsedTime);
        fscanf(logGaitSpeed, "-> v: %f\n", &curGaitSpeed); // -> v: 1.18548
        fscanf(logStep, "%d\n", &curStep);

        curStartTime = str2ts(buf);
        curEndTime = getEndTime(&curStartTime, curElapsedTime);
        curStrideLength = (2.0f * curGaitSpeed * (float)curElapsedTime) / (float)curStep;

        strcpy(list[listLength].userId, USER_ID);
        list[listLength].startTime = ts2tm(&curStartTime);
        list[listLength].endTime = ts2tm(&curEndTime);
        list[listLength].gaitSpeed = curGaitSpeed;
        list[listLength].strideLength = curStrideLength;
        listLength++;
    }
}

void closeLogFiles(void) {
    fclose(logStartTime);
    fclose(logElapsedTime);
    fclose(logGaitSpeed);
    fclose(logStep);
}

void appendData() {

}

// BSON_APPEND_DATE_TIME(document, "startTime")
int main(int argc, char *argv[])
{
    const char *uri_string = "mongodb://hpclab:hpc1314@localhost:27017/?authSource=gaitdb";
    mongoc_uri_t *uri;
    mongoc_client_t *client;
    mongoc_database_t *database;
    mongoc_collection_t *collection;
    bson_error_t error;
    char *str;
    bool retval;

    bson_t *document;

    UserGaitData userDataList[BUF_SIZE];

    if (openLogFiles() == -1) {
        fputs("E: file open error\n", stderr);
        return -1;
    }

    readLogFiles(userDataList);
    retrieveUserGaitData(userDataList, listLength);

    closeLogFiles();


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

    printf("listLength: %d\n", listLength);
    int i;
    for (i = 0; i < listLength; i++) {
        document = BCON_NEW(
            "userId", BCON_UTF8(userDataList[i].userId),
            "startTime", BCON_DATE_TIME(mktime(&userDataList[i].startTime) * 1000),
            "endTime", BCON_DATE_TIME(mktime(&userDataList[i].endTime) * 1000),
            "gaitSpeed", BCON_DOUBLE(userDataList[i].gaitSpeed),
            "strideLength", BCON_DOUBLE(userDataList[i].strideLength)
        );
        str = bson_as_canonical_extended_json (document, NULL);
        printf("[%03d] %s\n", i, str);

        if (!mongoc_collection_insert_one(collection, document, NULL, NULL, &error)) {
            fprintf(stderr, "%s\n", error.message);
            return EXIT_FAILURE;
        }

        bson_free(str);
        bson_destroy(document);
    }
    
    // Release our handles and clean up libmongoc
    mongoc_collection_destroy(collection);
    mongoc_database_destroy(database);
    mongoc_uri_destroy(uri);
    mongoc_client_destroy(client);
    mongoc_cleanup();

    return EXIT_SUCCESS;
}