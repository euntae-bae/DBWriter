#include <stdio.h>
#include <time.h>
#include "timestamp.h"
#define LOG_START_TIME      "log-start-time.txt"
#define LOG_ELAPSED_TIME    "log-elapsed-time.txt"
#define BUF_SIZE 256

int main(void) {
    FILE *logStartTime = fopen(LOG_START_TIME, "rt");
    FILE *logElapsedTime = fopen(LOG_ELAPSED_TIME, "rt");

    char buf[BUF_SIZE];
    int curStep, curElapsedTime;
    int i;

    time_t rawtime;
    struct tm *curTime;
    time(&rawtime);
    curTime = localtime(&rawtime);
    displayTm(curTime);

    Timestamp ts1 = str2ts("[20210805063439.726]");
    Timestamp ts2 = getEndTime(&ts1, 36);
    struct tm tm1 = ts2tm(&ts1);
    struct tm tm2 = ts2tm(&ts2);
    displayTm(&tm1);
    displayTm(&tm2);

    Timestamp ts3 = str2ts("[20191231235926.591]");
    Timestamp ts4 = getEndTime(&ts3, 51);
    struct tm tm3 = ts2tm(&ts3);
    struct tm tm4 = ts2tm(&ts4);
    displayTm(&tm3);
    displayTm(&tm4);

    return 0;

/*
    if (!logStartTime) {
        fputs("E: can't open file", stderr);
        return -1;
    }

    if (!logElapsedTime) {
        fputs("E: can't open file\n", stderr);
        return -1;
    }

    i = 0;
    while (!feof(logStartTime)) {
        fgets(buf, BUF_SIZE, logStartTime);
        printf("%03d ", i++);
        getTime(buf);
    }

    i = 0;
    while (!feof(logElapsedTime)) {
        fscanf(logElapsedTime, "delta time: %d\n", &curElapsedTime);
        printf("%03d %d\n", i++, curElapsedTime);
    }

    fclose(logStartTime);
    fclose(logElapsedTime);
    return 0;
}
*/
}