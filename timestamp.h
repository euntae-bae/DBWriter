#include <time.h>
#include <string.h>

typedef struct Timestamp {
    int year;
    int month;
    int day;
    int hour;
    int min;
    int second;
    int ms;
} Timestamp;

int getLastDay(int year, int month) {
    const int DAY_LIST[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (month == 2) {
        if (year % 4 == 0) { // 윤년 (2월 29일까지 있음)
            return 29;
        }
        return 28;
    }
    return DAY_LIST[month - 1];
}

// [20210805063439.726]

Timestamp str2ts(const char *ts) {
    Timestamp retTime;
    int offset = 1;
    int curValue = 0;
    int digit;
    int i;

    memset(&retTime, 0, sizeof(retTime));

    // get year
    for (i = 0; i < 4; i++) {
        digit = ts[i + offset] - '0';
        curValue *= 10;
        curValue += digit;
    }
    retTime.year = curValue;

    // get month
    offset = i + offset;
    curValue = 0;
    for (i = 0; i < 2; i++) {
        digit = ts[i + offset] - '0';
        curValue *= 10;
        curValue += digit;
    }
    retTime.month = curValue;
    
    // get day
    offset = i + offset;
    curValue = 0;
    for (i = 0; i < 2; i++) {
        digit = ts[i + offset] - '0';
        curValue *= 10;
        curValue += digit;
    }
    retTime.day = curValue;

    // get hour
    offset = i + offset;
    curValue = 0;
    for (i = 0; i < 2; i++) {
        digit = ts[i + offset] - '0';
        curValue *= 10;
        curValue += digit;
    }
    retTime.hour = curValue;

    // get min
    offset = i + offset;
    curValue = 0;
    for (i = 0; i < 2; i++) {
        digit = ts[i + offset] - '0';
        curValue *= 10;
        curValue += digit;
    }
    retTime.min = curValue;

    // get sec
    offset += i;
    curValue = 0;
    for (i = 0; i < 2; i++) {
        digit = ts[i + offset] - '0';
        curValue *= 10;
        curValue += digit;
    }
    retTime.second = curValue;

    // get ms
    offset = offset + i + 1;
    curValue = 0;
    i = 0;
    while (ts[i + offset] != ']') {
        digit = ts[i + offset] - '0';
        curValue *= 10;
        curValue += digit;
        i++;
    }
    retTime.ms = curValue;
    // printf("%d/%d/%d %d:%d:%d.%d\n", retTime.year, retTime.month, retTime.day, retTime.hour, retTime.min, retTime.second, retTime.ms);
    return retTime;
}

struct tm ts2tm(const Timestamp *ts) {
    struct tm retTm;
    memset(&retTm, 0, sizeof(struct tm));
    retTm.tm_year = ts->year - 1900;
    retTm.tm_mon = ts->month - 1;
    retTm.tm_mday = ts->day;
    retTm.tm_hour = ts->hour;
    retTm.tm_min = ts->min;
    retTm.tm_sec = ts->second;
    return retTm;
}

Timestamp tm2ts(const struct tm *t) {
    Timestamp ts;
    memset(&ts, 0, sizeof(ts));
    ts.year = t->tm_year + 1900;
    ts.month = t->tm_mon + 1;
    ts.day = t->tm_mday;
    ts.hour = t->tm_hour;
    ts.min = t->tm_min;
    ts.second = t->tm_sec;
    return ts;
}

Timestamp getEndTime(const Timestamp *ts, int elapsedTime) {
    Timestamp ret;
    int carry;
    int lastDay;

    memcpy(&ret, ts, sizeof(Timestamp));
    ret.second += elapsedTime;
    if (ret.second >= 60) {
        ret.second -= 60;
        carry = 1;
    }
    else
        carry = 0;
    
    ret.min += carry;
    if (ret.min >= 60) {
        ret.min -= 60;
        carry = 1;
    }
    else
        carry = 0;
    
    ret.hour += carry;
    if (ret.hour >= 24) {
        ret.hour -= 24;
        carry = 1;
    }
    else
        carry = 0;

    ret.day += carry;
    lastDay = getLastDay(ret.year, ret.month);
    if (ret.day > lastDay) {
        ret.day -= lastDay;
        carry = 1;
    }
    else
        carry = 0;

    ret.month += carry;
    if (ret.month > 12) {
        ret.month -= 12;
        carry = 1;
    }
    else
        carry = 0;

    ret.year += carry;
    return ret;
}

void displayTm(const struct tm *time) {
    // 1900 based year
    // 0 based month
    printf("%04d/%02d/%02d %02d:%02d:%02d", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday, 
    time->tm_hour, time->tm_min, time->tm_sec);
}