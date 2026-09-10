#include "date.h"

#include <stdio.h>
#include <time.h>


static bool is_leap_year(int year)
{
    if ((year % 400) == 0)
    {
        return true;
    }

    if ((year % 100) == 0)
    {
        return false;
    }

    return (year % 4) == 0;
}


static int days_in_month(int year, int month)
{
    switch (month)
    {
        case 1:  return 31;
        case 2:  return is_leap_year(year) ? 29 : 28;
        case 3:  return 31;
        case 4:  return 30;
        case 5:  return 31;
        case 6:  return 30;
        case 7:  return 31;
        case 8:  return 31;
        case 9:  return 30;
        case 10: return 31;
        case 11: return 30;
        case 12: return 31;

        default:
            return 0;
    }
}


Date date_create(int year, int month, int day)
{
    Date date;

    date.year = year;
    date.month = month;
    date.day = day;

    return date;
}


bool date_is_valid(Date date)
{
    if (date.year < 1)
    {
        return false;
    }

    if (date.month < 1 || date.month > 12)
    {
        return false;
    }

    int max_day = days_in_month(
        date.year,
        date.month
    );

    if (date.day < 1 || date.day > max_day)
    {
        return false;
    }

    return true;
}


Date date_today(void)
{
    time_t current_time = time(NULL);

    struct tm *local_time =
        localtime(&current_time);

    if (local_time == NULL)
    {
        return date_create(1970, 1, 1);
    }

    return date_create(
        local_time->tm_year + 1900,
        local_time->tm_mon + 1,
        local_time->tm_mday
    );
}


Date date_add_days(Date date, int days)
{
    while (days > 0)
    {
        date.day++;

        if (date.day >
            days_in_month(date.year, date.month))
        {
            date.day = 1;
            date.month++;

            if (date.month > 12)
            {
                date.month = 1;
                date.year++;
            }
        }

        days--;
    }

    while (days < 0)
    {
        date.day--;

        if (date.day < 1)
        {
            date.month--;

            if (date.month < 1)
            {
                date.month = 12;
                date.year--;
            }

            date.day =
                days_in_month(
                    date.year,
                    date.month);
        }

        days++;
    }

    return date;
}


static long date_to_day_number(Date date)
{
    long total_days = 0;

    for (int year = 1;
         year < date.year;
         year++)
    {
        total_days +=
            is_leap_year(year) ? 366 : 365;
    }

    for (int month = 1;
         month < date.month;
         month++)
    {
        total_days +=
            days_in_month(
                date.year,
                month);
    }

    total_days += date.day;

    return total_days;
}


int date_days_until(Date from, Date to)
{
    long from_days =
        date_to_day_number(from);

    long to_days =
        date_to_day_number(to);

    return (int)(to_days - from_days);
}


bool date_equal(Date a, Date b)
{
    return
        a.year == b.year &&
        a.month == b.month &&
        a.day == b.day;
}


bool date_less(Date a, Date b)
{
    if (a.year != b.year)
    {
        return a.year < b.year;
    }

    if (a.month != b.month)
    {
        return a.month < b.month;
    }

    return a.day < b.day;
}


bool date_greater(Date a, Date b)
{
    if (date_equal(a, b))
    {
        return false;
    }

    return !date_less(a, b);
}


void date_to_string(
    Date date,
    char *buffer,
    int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0)
    {
        return;
    }

    snprintf(
        buffer,
        (size_t)buffer_size,
        "%04d-%02d-%02d",
        date.year,
        date.month,
        date.day
    );
}