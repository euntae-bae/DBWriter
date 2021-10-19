#include <stdio.h>
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

    printf("%d\n", getLastDay(2020, 2));
    printf("%d\n", getLastDay(1980, 2));
    printf("%d\n", getLastDay(1979, 3));
    printf("%d\n", getLastDay(2021, 5));
    printf("%d\n", getLastDay(2021, 8));
    printf("%d\n", getLastDay(1970, 11));
    printf("%d\n", getLastDay(2021, 2));
    return 0;

    //printf("%d\n", getYearFromTimestamp("[20210805063439.726]"));
    //getTime("[20210805063439.726]");

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