# FlightScope - Quick Start Guide

## Project Status ✅

**Initial setup complete!** The FlightScope Ground Control Station project is now up and running with:

- ✅ Qt 6.9.3 project structure
- ✅ MAVLink v2 library integrated
- ✅ Basic application with MainWindow UI
- ✅ Code formatting and static analysis configured
- ✅ Testing framework with QTest
- ✅ Build scripts for Windows
- ✅ Git repository initialized and synced

## Building the Application

### Option 1: Using the Build Script (Recommended)
```batch
build.bat
```

### Option 2: Manual Build
```batch
SET PATH=C:\Qt\Tools\mingw1310_64\bin;C:\Qt\6.9.3\mingw_64\bin;%PATH%
qmake FlightScope.pro
mingw32-make -j4
```

## Running the Application

### Option 1: Using the Launch Script (Recommended)
```batch
run_flightscope.bat
```

### Option 2: Direct Execution
```batch
SET PATH=C:\Qt\Tools\mingw1310_64\bin;C:\Qt\6.9.3\mingw_64\bin;%PATH%
release\FlightScope.exe
```

## Running Tests

```batch
cd tests
SET PATH=C:\Qt\Tools\mingw1310_64\bin;C:\Qt\6.9.3\mingw_64\bin;%PATH%
qmake tests.pro
mingw32-make
release\tests.exe -v2
```

## Project Structure

```
FlightScope/
├── src/                    # Source code
│   ├── main.cpp           # Application entry point
│   └── ui/                # UI components
│       ├── mainwindow.h
│       ├── mainwindow.cpp
│       └── mainwindow.ui
├── resources/             # Resources (QML, images, styles)
│   ├── qml/              # QML files
│   └── resources.qrc     # Qt resource file
├── tests/                # Unit tests
│   ├── test_main.cpp
│   └── tests.pro
├── third-party/          # External libraries
│   └── mavlink/          # MAVLink v2 library
├── .clang-format         # Code formatting rules
├── .clang-tidy           # Static analysis rules
├── FlightScope.pro       # Main Qt project file
├── build.bat             # Build script
└── run_flightscope.bat   # Launch script
```

## Current Features

The application currently includes:

1. **Main Window UI**
   - Menu bar (File, Help)
   - Toolbar with Connect/Disconnect buttons
   - Status bar with connection status and link stats
   - About dialog

2. **Testing Framework**
   - Smoke test to verify MainWindow creation
   - All tests passing ✅

3. **MAVLink Integration**
   - MAVLink v2 C library headers available
   - Ready for Phase 1 implementation

## Next Steps - Phase 1: Core MVP

According to the blueprint, the next steps are:

### Task 1.1: Connectivity & MAVLink Core
- [ ] Create LinkInterface abstract class (thread-safe, QObject-based)
- [ ] Implement MavlinkRouter class
- [ ] Handle HEARTBEAT and TIMESYNC messages
- [ ] Build Connection UI dialog
- [ ] Implement reconnect policy with exponential backoff

### Task 1.2: Vehicle Model & Health
- [ ] Create VehicleModel class with Q_PROPERTY telemetry
- [ ] Create HealthModel class with EKF/GPS status
- [ ] UI integration with health panel

### Task 1.3: PFD/HUD (QML)
- [ ] Embed QQuickWidget
- [ ] Create artificial horizon QML component
- [ ] Data binding to VehicleModel

## Development Tools

- **Qt Creator**: Primary IDE (recommended)
- **Qt Designer**: For editing .ui files
- **qmake**: Build system
- **MAVLink**: Drone communication protocol

## Connecting to Mission Planner (SITL)

When ready to test MAVLink connectivity, you can connect to Mission Planner's SITL:

### ArduPilot SITL
- UDP Port: 14550

### PX4 SITL
- UDP Port: 14540

## Useful Commands

### Format code
```batch
clang-format -i src/**/*.cpp src/**/*.h
```

### Run static analysis
```batch
clang-tidy src/**/*.cpp -- -I./src -I./third-party
```

### Clean build
```batch
mingw32-make clean
```

## Resources

- [Qt Documentation](https://doc.qt.io/)
- [MAVLink Developer Guide](https://mavlink.io/en/)
- [FlightScope Blueprint](FlightScope%20Blueprint.pdf)

## Support

For issues or questions about the project structure, refer to:
- GitHub Issues: https://github.com/snithinbabunaidu/FlightScope/issues
- Blueprint PDF for detailed task breakdown

---

**Happy Coding! 🚁**
