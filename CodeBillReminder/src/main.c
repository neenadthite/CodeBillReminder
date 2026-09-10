#include <stdio.h>
#include "date.h"

int main(void)
{
    Date date;

    if (date_from_string("2026-09-20", &date))
    {
        printf("Date parsed successfully.\n");

        printf("Year  : %d\n", date.year);
        printf("Month : %d\n", date.month);
        printf("Day   : %d\n", date.day);
    }
    else
    {
        printf("Date parsing failed.\n");
    }

    return 0;
}