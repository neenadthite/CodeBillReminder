#ifndef BILL_UI_H
#define BILL_UI_H

#include "bill.h"


/*
 * Read a complete Bill from the user.
 *
 * Returns true if the bill was successfully created.
 */
bool bill_ui_create(Bill* bill);


/*
 * Display a Bill.
 */
void bill_ui_print(const Bill* bill);

#endif