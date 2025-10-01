cmake_minimum_required(VERSION 3.16)

project(FlightScope VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6 and the required components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# Define the source files for the executable
qt_add_executable(FlightScope
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    LinkInterface.h
    mavlinkrouter.cpp
    mavlinkrouter.h
)

target_compile_definitions(FlightScope PRIVATE MAVLINK_DIALECT=common)

target_include_directories(FlightScope PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party"
)

# Link the executable against the required Qt modules
target_link_libraries(FlightScope PRIVATE
    Qt::Core
    Qt::Widgets
    Qt::Network # <-- Added missing Network module for QUdpSocket
)

# Set properties for macOS and Windows packaging
set_target_properties(FlightScope PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.FlightScope
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    WIN32_EXECUTABLE TRUE
)

# Add the smoke test sub-project
add_subdirectory(tst_smoketest)

# Finalize the executable (required for Qt6)
qt_finalize_executable(FlightScope)
