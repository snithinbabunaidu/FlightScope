cmake_minimum_required(VERSION 3.21)

project(FlightScope VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 packages
find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Qml
    Quick
    QuickWidgets
    Location
    Positioning
    Network
    WebSockets
    Charts
)

# Platform-specific Qt modules
if(ANDROID)
    find_package(Qt6 REQUIRED COMPONENTS Core5Compat)
    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android" CACHE INTERNAL "")
endif()

if(IOS)
    find_package(Qt6 REQUIRED COMPONENTS Svg)
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(PROJECT_SOURCES
    src/main.cpp
    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
    src/ui/mainwindow.ui
    src/ui/connectdialog.cpp
    src/ui/connectdialog.h
    src/ui/missioneditor.cpp
    src/ui/missioneditor.h
    src/ui/commandeditordialog.cpp
    src/ui/commandeditordialog.h
    src/ui/mapwidget.cpp
    src/ui/mapwidget.h
    src/ui/compasswidget.cpp
    src/ui/compasswidget.h
    src/ui/hudwidget.cpp
    src/ui/hudwidget.h
    src/comm/linkinterface.h
    src/comm/udplink.cpp
    src/comm/udplink.h
    src/comm/linkmanager.cpp
    src/comm/linkmanager.h
    src/comm/mavlinkrouter.cpp
    src/comm/mavlinkrouter.h
    src/comm/commandbus.cpp
    src/comm/commandbus.h
    src/models/vehiclemodel.cpp
    src/models/vehiclemodel.h
    src/models/healthmodel.cpp
    src/models/healthmodel.h
    src/models/waypoint.cpp
    src/models/waypoint.h
    src/models/missionmodel.cpp
    src/models/missionmodel.h
    src/models/geofencemodel.cpp
    src/models/geofencemodel.h
)

# Resources
set(PROJECT_RESOURCES
    resources/resources.qrc
)

# Platform-specific executable creation
if(ANDROID)
    qt_add_executable(FlightScope MANUAL_FINALIZATION ${PROJECT_SOURCES} ${PROJECT_RESOURCES})

    set_property(TARGET FlightScope PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
                 ${CMAKE_CURRENT_SOURCE_DIR}/android)

    set_property(TARGET FlightScope PROPERTY QT_ANDROID_MIN_SDK_VERSION 28)
    set_property(TARGET FlightScope PROPERTY QT_ANDROID_TARGET_SDK_VERSION 33)

elseif(IOS)
    qt_add_executable(FlightScope MACOSX_BUNDLE ${PROJECT_SOURCES} ${PROJECT_RESOURCES})

    set_target_properties(FlightScope PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER com.flightscope.gcs
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/ios/Info.plist
    )

else()
    # Desktop platforms (Windows, macOS, Linux)
    qt_add_executable(FlightScope ${PROJECT_SOURCES} ${PROJECT_RESOURCES})

    if(WIN32)
        set_target_properties(FlightScope PROPERTIES
            WIN32_EXECUTABLE TRUE
        )
    endif()

    if(APPLE AND NOT IOS)
        set_target_properties(FlightScope PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_GUI_IDENTIFIER com.flightscope.gcs
            MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
            MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        )
    endif()
endif()

# Link Qt libraries
target_link_libraries(FlightScope PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickWidgets
    Qt6::Location
    Qt6::Positioning
    Qt6::Network
    Qt6::WebSockets
    Qt6::Charts
)

# Platform-specific linking
if(ANDROID)
    target_link_libraries(FlightScope PRIVATE Qt6::Core5Compat)
endif()

if(IOS)
    target_link_libraries(FlightScope PRIVATE Qt6::Svg)
endif()

# Compiler definitions
target_compile_definitions(FlightScope PRIVATE
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_BEFORE=0x060000
)

# Install rules
if(ANDROID)
    install(TARGETS FlightScope
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
elseif(IOS)
    install(TARGETS FlightScope
        BUNDLE DESTINATION .
    )
else()
    install(TARGETS FlightScope
        BUNDLE DESTINATION .
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
    )
endif()

# Finalize executable (required for Android/iOS)
qt_finalize_executable(FlightScope)
