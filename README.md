# Bill Reminder System

A lightweight **Bill Reminder System written in C17** with SQLite database storage and automated email notifications.

The application can be used locally through a command-line interface and can also run automatically on a cloud runner using **GitHub Actions**.

---

## 🚀 Features

* Add and store bills
* View all stored bills
* Mark bills as **PAID**
* Delete bills
* Calculate reminder dates automatically
* Check whether a bill's reminder date has been reached
* Send email reminders through Gmail SMTP
* Store bill information using SQLite
* Run reminder checks non-interactively using a command-line argument
* Automated daily execution using GitHub Actions
* Secure email credentials using GitHub Actions Secrets
* Linux-compatible C17 build for cloud execution

---

## 🏗️ System Overview

```text
                    Bill Reminder System
                           │
             ┌─────────────┴─────────────┐
             │                           │
        Local Execution             GitHub Actions
             │                           │
             ▼                           ▼
       CLI Application             Linux Runner
             │                           │
             └─────────────┬─────────────┘
                           ▼
                    SQLite Database
                           │
                           ▼
                    Reminder Engine
                           │
              ┌────────────┴────────────┐
              │                         │
         Status = PAID             Status ≠ PAID
              │                         │
          Skip reminder             Check reminder date
                                        │
                              ┌─────────┴─────────┐
                              │                   │
                         Not due yet             Due
                              │                   │
                           No email          Email enabled?
                                                  │
                                           ┌──────┴──────┐
                                           │             │
                                          No            Yes
                                           │             │
                                        No email     Send email
```

---

## 🔔 Reminder Logic

A bill is considered for notification when:

1. The bill status is **not `PAID`**
2. The reminder date has been reached
3. Email notification is enabled

The current implementation intentionally uses the bill's **status** as the primary control for reminders.

Example:

```text
Bill Status: PAID
Reminder Date: Reached

→ Reminder skipped
```

Whereas:

```text
Bill Status: PENDING
Reminder Date: Reached
Email: Enabled

→ Email reminder sent
```

---

## 🖥️ Command-Line Interface

The application provides the following menu:

```text
================================
         BILL REMINDER
================================
1. Add Bill
2. View Bills
3. Mark Bill as paid
4. Delete Bill
5. Check Reminders
6. Exit
================================
Select option:
```

### Add Bill

Users can enter:

* Account name
* Provider
* Account number
* Amount
* Due date
* Number of reminder days
* Email notification preference

The reminder date is calculated automatically:

```text
Reminder Date = Due Date - Reminder Days
```

---

### Mark Bill as Paid

A bill can be manually marked as paid using its Bill ID:

```text
Select option: 3
Enter Bill ID to mark as paid: 1

Bill ID 1 marked as PAID.
```

Once marked as paid, the reminder engine skips that bill.

---

## 🗄️ Database

The project uses **SQLite** for persistent local bill storage.

The database is located at:

```text
CodeBillReminder/database/bills.db
```

The database contains information such as:

* Bill ID
* Account name
* Provider
* Account number
* Amount
* Due date
* Reminder days
* Email notification status
* Bill status

Amounts are stored internally as **integer paise** using `int64_t` to avoid floating-point currency precision issues.

---

## 📁 Project Structure

```text
CodeBillReminder/
│
├── .github/
│   └── workflows/
│       └── reminder.yaml
│
├── CodeBillReminder/
│   │
│   ├── config/
│   │   └── email.conf
│   │
│   ├── database/
│   │   └── bills.db
│   │
│   ├── external/
│   │   └── sqlite/
│   │       ├── sqlite3.c
│   │       └── sqlite3.h
│   │
│   ├── include/
│   │   ├── bill.h
│   │   ├── bill_manager.h
│   │   ├── bill_ui.h
│   │   ├── database.h
│   │   ├── date.h
│   │   ├── email_config.h
│   │   ├── notification.h
│   │   ├── notification_manager.h
│   │   └── reminder.h
│   │
│   ├── src/
│   │   ├── main.c
│   │   ├── bill.c
│   │   ├── bill_manager.c
│   │   ├── bill_ui.c
│   │   ├── database.c
│   │   ├── date.c
│   │   ├── email_config.c
│   │   ├── notification.c
│   │   ├── notification_manager.c
│   │   └── reminder.c
│   │
│   └── tests/
│
└── README.md
```

---

## ⚙️ Local Build

The project is written in **C17**.

### Windows

The project can be developed and built using Visual Studio.

### Linux

The application can be built using GCC:

```bash
gcc -std=c17 -Wall -Wextra \
  -Iinclude \
  -Iexternal/sqlite \
  src/main.c \
  src/bill.c \
  src/bill_manager.c \
  src/bill_ui.c \
  src/database.c \
  src/date.c \
  src/email_config.c \
  src/notification.c \
  src/notification_manager.c \
  src/reminder.c \
  external/sqlite/sqlite3.c \
  -lcurl \
  -o CodeBillReminder
```

---

## ▶️ Running the Application

### Interactive mode

```bash
./CodeBillReminder
```

The interactive CLI provides bill management and manual reminder checking.

### Automated reminder mode

For non-interactive execution:

```bash
./CodeBillReminder --check-reminders
```

This mode:

1. Opens the SQLite database
2. Loads all bills
3. Evaluates reminder conditions
4. Sends email notifications where applicable
5. Exits automatically

This mode is designed specifically for automated execution through GitHub Actions.

---

## 📧 Email Configuration

The application uses Gmail SMTP for email notifications.

For local execution, the configuration file is:

```text
config/email.conf
```

Example:

```text
SMTP_HOST=smtp.gmail.com
SMTP_PORT=587
SMTP_USERNAME=your_email@gmail.com
SMTP_PASSWORD=your_app_password
EMAIL_FROM=your_email@gmail.com
EMAIL_TO=recipient@gmail.com
```

### Gmail Authentication

A **Google App Password** should be used instead of the normal Gmail account password.

---

## 🔐 GitHub Actions Security

Email credentials are **not stored in the repository**.

GitHub Actions Secrets are used:

```text
SMTP_HOST
SMTP_PORT
SMTP_USERNAME
SMTP_PASSWORD
EMAIL_FROM
EMAIL_TO
```

During workflow execution, the configuration file is generated dynamically from these secrets.

The workflow does not require the real email configuration file to be committed to Git.

---

## ☁️ GitHub Actions Automation

The project includes:

```text
.github/workflows/reminder.yaml
```

The workflow:

1. Checks out the `C_Code` branch
2. Installs GCC and libcurl
3. Builds the C17 application
4. Generates the email configuration from GitHub Secrets
5. Runs:

```bash
./CodeBillReminder --check-reminders
```

The workflow is also configured for daily scheduled execution.

Current schedule:

```yaml
schedule:
  - cron: '30 2 * * *'
```

This corresponds to:

```text
02:30 UTC
08:00 IST
```

A manual `workflow_dispatch` trigger is also available.

---

## 🔄 Automated Reminder Flow

```text
GitHub Actions
      │
      ▼
Checkout C_Code
      │
      ▼
Install GCC + libcurl
      │
      ▼
Build C17 application
      │
      ▼
Create email.conf
from GitHub Secrets
      │
      ▼
Open SQLite database
      │
      ▼
Load bills
      │
      ▼
Check bill status
      │
      ├── PAID ──────────────► Skip
      │
      └── PENDING/OVERDUE
                │
                ▼
        Check reminder date
                │
                ▼
         Email enabled?
                │
                ▼
          Gmail SMTP
                │
                ▼
        Email notification
```

---

## 🧪 Example

Suppose a bill has:

```text
Account        : Electricity
Due Date       : 2026-09-21
Reminder Days  : 5
Reminder Date  : 2026-09-16
Status         : PENDING
Email          : Enabled
```

On or after:

```text
2026-09-16
```

the bill becomes eligible for a reminder.

If the user marks the bill as paid:

```text
Status : PAID
```

the reminder engine skips the bill.

---

## 🛡️ Design Considerations

### Currency Precision

Bill amounts are stored as integer paise:

```c
int64_t amount_paise;
```

This avoids floating-point errors when storing monetary values.

### Date Handling

Dates are represented using a dedicated `Date` structure:

```c
typedef struct
{
    int year;
    int month;
    int day;
} Date;
```

The application provides date validation, date arithmetic, and reminder-date calculation.

### Modular Architecture

The application separates responsibilities into:

```text
Bill
 ├── Bill Manager
 ├── Bill UI
 ├── Database
 ├── Date
 ├── Reminder
 └── Notification
       └── Email
```

This makes individual components easier to test and extend.

---

## ⚠️ Current Limitations

The current release is an **MVP**.

### Reminder history persistence

The application currently uses the bill's `PAID` status to prevent reminders for bills that have already been paid.

A persistent reminder-history mechanism is planned for a future version.

### GitHub Actions database persistence

The GitHub Actions runner is ephemeral. Changes made to the SQLite database during a workflow execution are not currently persisted back to the repository.

Therefore, persistent reminder-history tracking across independent workflow runs is intentionally deferred.

### Notification Channels

The current release focuses on:

```text
Email
```

SMS functionality is not part of the current release.

---

## 🔮 Future Enhancements

Planned improvements include:

* Persistent reminder history
* Cloud database/storage
* Improved notification tracking
* Additional notification channels
* Automated testing
* Better configuration management
* Richer CLI interface
* Containerized deployment
* Improved logging and monitoring

---

## 🧰 Technologies Used

* **C17**
* **SQLite**
* **libcurl**
* **Gmail SMTP**
* **Git**
* **GitHub Actions**
* **Visual Studio**
* **GCC / Linux**

---

## 📌 Project Status

**MVP — Functional**

The current implementation supports local bill management and automated email reminders through GitHub Actions.

```text
Local Application        ✅
SQLite Database          ✅
Bill Management          ✅
Mark as Paid             ✅
Reminder Engine          ✅
Email Notifications      ✅
Gmail SMTP               ✅
GitHub Actions           ✅
Scheduled Execution      ✅
GitHub Secrets           ✅
```

---

## 👤 Author

**Ninad Thite**

Embedded Systems / C / RTOS / Software Development

---

## 📄 License

Add your preferred open-source license here.

For example:

```text
Copyright © Ninad Thite
```

