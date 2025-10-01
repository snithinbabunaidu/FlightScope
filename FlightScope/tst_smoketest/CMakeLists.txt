cmake_minimum_required(VERSION 3.16)

project(SmokeTest LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Test)

qt_add_executable(tst_smoketest
    tst_smoketest.cpp
    ../mainwindow.cpp
    ../mavlinkrouter.cpp
    ../vehiclemodel.cpp
    ../healthmodel.cpp
)

target_include_directories(tst_smoketest PRIVATE
    "${CMAKE_SOURCE_DIR}/third_party"
)

target_compile_definitions(tst_smoketest PRIVATE MAVLINK_DIALECT=common)

target_link_libraries(tst_smoketest PRIVATE
    Qt::Test
    Qt::Widgets
    Qt::Network
)

add_test(NAME tst_smoketest COMMAND tst_smoketest)
