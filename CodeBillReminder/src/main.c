#include <stdio.h>

#include "bill_ui.h"


int main(void)
{
    Bill bill;

    if (!bill_ui_create(&bill))
    {
        printf("\nFailed to create bill.\n");

        return 1;
    }

    printf("\nBill created successfully!\n");

    bill_ui_print(&bill);

    return 0;
}