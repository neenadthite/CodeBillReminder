cmake_minimum_required(VERSION 3.20)

project(BillReminder
    VERSION 1.0
    LANGUAGES C
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(bill_reminder
    src/main.c
    src/date.c
    src/bill.c
    src/bill_ui.c
    src/bill_manager.c
)

target_include_directories(bill_reminder
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)