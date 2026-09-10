#ifndef DATE_H
#define DATE_H

#include <stdbool.h>

typedef struct
{
    int year;
    int month;
    int day;

} Date;


/* Create a date */
Date date_create(int year, int month, int day);


/* Get today's date */
Date date_today(void);


/* Validate a date */
bool date_is_valid(Date date);


/* Add or subtract days */
Date date_add_days(Date date, int days);


/* Calculate number of days between two dates */
int date_days_until(Date from, Date to);


/* Date comparison */
bool date_equal(Date a, Date b);
bool date_less(Date a, Date b);
bool date_greater(Date a, Date b);


/* Convert date to YYYY-MM-DD */
void date_to_string(
    Date date,
    char *buffer,
    int buffer_size
);

bool date_from_string(const char* string, Date* date);

#endif