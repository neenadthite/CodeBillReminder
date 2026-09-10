#include <stdio.h>

#include "date.h"

static void print_date(const char *label, Date date)
{
    char buffer[16];

    date_to_string(
        date,
        buffer,
        sizeof(buffer)
    );

    printf("%s%s\n", label, buffer);
}


int main(void)
{
    Date today = date_create(2026, 9, 3);

    Date due_date = date_create(2026, 9, 15);

    Date reminder_date =
        date_add_days(due_date, -5);

    print_date("Today          : ", today);

    print_date("Due Date       : ", due_date);

    print_date(
        "Reminder Date  : ",
        reminder_date
    );

    printf(
        "Days remaining : %d\n",
        date_days_until(today, due_date)
    );

    return 0;
}