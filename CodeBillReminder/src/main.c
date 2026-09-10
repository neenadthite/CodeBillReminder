#include <stdio.h>

#include "bill_manager.h"
#include "bill_ui.h"


static Bill create_test_bill(
    const char* account,
    const char* provider,
    double amount,
    Date due_date)
{
    Bill bill;

    bill_init(&bill);

    bill_set_account_name(
        &bill,
        account
    );

    bill_set_provider(
        &bill,
        provider
    );

    bill_set_account_number(
        &bill,
        "123456789"
    );

    bill_set_amount(
        &bill,
        amount
    );

    bill_set_due_date(
        &bill,
        due_date
    );

    bill_set_reminder_days(
        &bill,
        5
    );

    bill_set_email_enabled(
        &bill,
        true
    );

    bill_set_sms_enabled(
        &bill,
        false
    );

    return bill;
}


int main(void)
{
    BillManager manager;

    if (!bill_manager_init(&manager))
    {
        printf(
            "Failed to initialize BillManager.\n"
        );

        return 1;
    }


    Bill electricity =
        create_test_bill(
            "Electricity",
            "MSEDCL",
            2450.50,
            date_create(2026, 9, 15)
        );


    Bill internet =
        create_test_bill(
            "Internet",
            "Airtel",
            999.00,
            date_create(2026, 9, 12)
        );


    Bill credit_card =
        create_test_bill(
            "Credit Card",
            "HDFC",
            18500.00,
            date_create(2026, 9, 20)
        );


    bill_manager_add(
        &manager,
        &electricity
    );

    bill_manager_add(
        &manager,
        &internet
    );

    bill_manager_add(
        &manager,
        &credit_card
    );


    bill_manager_print_all(
        &manager
    );


    printf(
        "\nTotal bills: %zu\n",
        bill_manager_count(&manager)
    );


    bill_manager_free(
        &manager
    );

    return 0;
}