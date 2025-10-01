cmake_minimum_required(VERSION 3.16)

project(SmokeTest LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Test Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test Widgets)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(SmokeTest tst_smoketest.cpp)
add_test(NAME SmokeTest COMMAND SmokeTest)

target_link_libraries(SmokeTest PRIVATE Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Widgets)
