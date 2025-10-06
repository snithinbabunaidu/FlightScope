# FlightScope Ground Control Station

FlightScope is a modular Ground Control Station (GCS) for UAVs, built with an operator-first design. It supports mission planning, live flight monitoring, health and safety checks, and post-flight analysis. With real-time telemetry, mapping, and log playback, FlightScope delivers clarity across the full mission lifecycle.

![FlightScope](resources/screenshots/main_window.png)

## Features

### Phase 1 - Core Telemetry (✅ Complete)
- ✅ **MAVLink Communication**: Full MAVLink v2 protocol support with UDP transport
- ✅ **Live Telemetry Display**: Real-time attitude, position, velocity, and battery data
- ✅ **System Health Monitoring**: GPS fix status, satellite count, EKF health indicators
- ✅ **Connection Management**: Automatic reconnection with exponential backoff
- ✅ **Multi-platform Support**: Windows, Linux, macOS compatible

### Phase 2 - Flight Planning (In Progress)
- 🔨 Waypoint creation and editing
- 🔨 Mission upload/download
- 🔨 Flight plan validation
- 🔨 Mission file save/load

### Phase 3 - Map Integration (Planned)
- 📋 Interactive map with vehicle tracking
- 📋 Waypoint visualization
- 📋 Flight path rendering
- 📋 Geofence display

### Phase 4 - Advanced Features (Planned)
- 📋 Parameter management
- 📋 Log download and analysis
- 📋 Real-time graphing
- 📋 Custom commands (takeoff, land, RTL)

## Requirements

### Software
- **Qt 6.9.3** or later
- **MinGW 13.1.0** (Windows) or GCC/Clang (Linux/macOS)
- **Git** for version control
- **Mission Planner** or **MAVProxy** for SITL testing (optional)

### Hardware (for real flight)
- Flight controller with MAVLink support (ArduPilot, PX4)
- Telemetry radio or WiFi connection
- Computer with network connectivity

## Quick Start

### 1. Clone the Repository

```bash
git clone https://github.com/snithinbabunaidu/FlightScope.git
cd FlightScope
```

### 2. Build the Application

#### Windows
```cmd
build.bat
```

#### Linux/macOS
```bash
qmake FlightScope.pro
make -j4
```

### 3. Run FlightScope

#### Windows
```cmd
run_flightscope.bat
```

#### Linux/macOS
```bash
./FlightScope
```

## Connecting to a Vehicle

### Option 1: Connect to Mission Planner SITL (Recommended for Testing)

1. **Start Mission Planner SITL**:
   - Open Mission Planner
   - Go to: `SIMULATION` tab
   - Click `Multirotor` → `ArduCopter`
   - Wait for SITL to start

2. **Configure Mission Planner UDP Output**:
   - In Mission Planner, press `Ctrl+F`
   - Click `Mavlink` button
   - Under "UDP Outbound", add: `127.0.0.1:14552`
   - Click `Add`

3. **Connect FlightScope**:
   - Click `Connect` button
   - Select "ArduPilot SITL (Listen on 14552)" preset
   - Click `OK`
   - Status bar should show "Connected" with live telemetry

### Option 2: Connect to MAVProxy SITL

```bash
# Start MAVProxy with UDP output
mavproxy.py --master=tcp:127.0.0.1:5760 --out=udp:127.0.0.1:14552

# In FlightScope: Connect using preset "ArduPilot SITL (Listen on 14552)"
```

### Option 3: Connect to Real Hardware

1. **UDP over WiFi**:
   - Configure your flight controller to send MAVLink to your computer's IP on port 14552
   - In FlightScope: `Connect` → `Custom Configuration`
   - Set Local Port: `14552`
   - Enable `Server Mode`
   - Click `OK`

2. **Serial Radio**:
   - Use a USB-to-serial adapter or telemetry radio
   - In FlightScope: `Connect` → `Custom Configuration`
   - Select serial port and baud rate (usually 57600)
   - Click `OK`

## Configuration

### Connection Settings

FlightScope supports multiple connection types:

- **UDP Server Mode** (Listen): FlightScope binds to a port and waits for vehicles to send data
  - Best for: SITL, WiFi telemetry, multiple vehicles
  - Example: Listen on `0.0.0.0:14552`

- **UDP Client Mode** (Connect): FlightScope actively sends data to a specific address/port
  - Best for: Connecting to specific GCS or companion computer
  - Example: Connect to `192.168.1.100:14550`

- **Serial Mode**: Direct connection via USB or telemetry radio
  - Best for: Traditional telemetry radios, USB connections
  - Common baud rates: 57600, 115200

### Presets

FlightScope includes connection presets:

| Preset | Type | Description |
|--------|------|-------------|
| ArduPilot SITL | UDP Server | Listen on 14552 for Mission Planner |
| PX4 SITL | UDP Server | Listen on 14540 for PX4 |
| WiFi Telemetry | UDP Server | Listen on 14550 (standard MAVLink port) |
| USB Serial | Serial | Connect via USB at 57600 baud |

## User Interface

### Main Window

- **Menu Bar**: File operations, help, about
- **Toolbar**: Quick access to connect/disconnect
- **Telemetry Dock** (right side):
  - Armed status
  - Flight mode
  - Altitude
  - Ground speed
  - Battery voltage and percentage
- **System Health Dock** (bottom right):
  - GPS fix type (No Fix, 2D, 3D, RTK Fixed)
  - Satellite count
  - GPS HDOP (accuracy indicator)
- **Status Bar** (bottom):
  - Connection status
  - Round-trip time (RTT)
  - Packet loss percentage
  - Last message timestamp

### Keyboard Shortcuts

- `Ctrl+O`: Connect
- `Ctrl+D`: Disconnect
- `Ctrl+Q`: Quit
- `F11`: Fullscreen toggle

## Development

### Project Structure

```
FlightScope/
├── src/
│   ├── comm/           # Communication layer
│   │   ├── linkinterface.h      # Abstract link interface
│   │   ├── udplink.h/cpp        # UDP implementation
│   │   ├── linkmanager.h/cpp    # Link lifecycle management
│   │   └── mavlinkrouter.h/cpp  # MAVLink parsing
│   ├── models/         # Data models
│   │   ├── vehiclemodel.h/cpp   # Telemetry data
│   │   └── healthmodel.h/cpp    # System health data
│   ├── ui/             # User interface
│   │   ├── mainwindow.h/cpp     # Main window
│   │   ├── mainwindow.ui        # UI layout
│   │   └── connectdialog.h/cpp  # Connection dialog
│   └── main.cpp        # Application entry point
├── tests/              # Unit tests
│   ├── test_main.cpp          # Smoke tests
│   ├── test_udplink.cpp       # UDP link tests
│   └── test_mavlinkrouter.cpp # MAVLink router tests
├── third-party/        # External libraries
│   └── mavlink/        # MAVLink C library
├── resources/          # Application resources
│   └── resources.qrc   # Qt resource file
└── build/              # Build output (ignored)
```

### Building from Source

#### Prerequisites

1. Install Qt 6.9.3+:
   - Download from: https://www.qt.io/download
   - Components needed: Qt Core, Qt Widgets, Qt Network, Qt Quick

2. Install compiler:
   - **Windows**: MinGW 13.1.0 (included with Qt)
   - **Linux**: `sudo apt install build-essential`
   - **macOS**: Xcode Command Line Tools

#### Build Steps

```bash
# Configure
qmake FlightScope.pro

# Build (parallel with 4 cores)
make -j4   # Linux/macOS
mingw32-make -j4  # Windows
```

#### Debug Build

```bash
qmake CONFIG+=debug FlightScope.pro
make -j4
```

### Running Tests

FlightScope includes comprehensive unit tests for the communication layer.

```bash
# Windows
run_tests.bat

# Linux/macOS
./run_tests.sh
```

Tests cover:
- UDP link creation and connection
- Data transmission and reception
- MAVLink message parsing
- Error handling and recovery
- Packet loss calculation

### Debug Logging

FlightScope automatically creates `flightscope_debug.log` with detailed debug output including:
- Connection status changes
- MAVLink message types received
- Remote address updates
- Heartbeat reception
- Errors and warnings

The log file is created in the same directory as the executable.

### Code Style

- **C++ Standard**: C++17
- **Qt Version**: Qt 6.9.3+
- **Indentation**: 4 spaces
- **Naming**:
  - Classes: `PascalCase`
  - Methods: `camelCase`
  - Member variables: `m_camelCase`
  - Constants: `UPPER_SNAKE_CASE`

## Troubleshooting

### Connection Issues

**Problem**: "Failed to bind UDP socket: The address is protected"

**Solution**: Another application is using the port. Try:
1. Close Mission Planner or other GCS software
2. Use a different port (e.g., 14551, 14552)
3. Check Windows Firewall settings

---

**Problem**: Connection shows "Reconnecting..." repeatedly

**Solution**:
1. Verify Mission Planner is sending data to the correct port
2. Check Wireshark for UDP packets arriving
3. Review `flightscope_debug.log` for errors
4. Ensure flight controller is sending HEARTBEAT messages

---

**Problem**: No telemetry data displayed

**Solution**:
1. Check connection status (should show "Connected")
2. Verify MAVLink messages are being received (check debug log)
3. Ensure vehicle is armed and sending full telemetry
4. Try disconnecting and reconnecting

### Build Issues

**Problem**: "qmake: command not found"

**Solution**: Add Qt to your PATH:
```bash
# Windows
set PATH=C:\Qt\6.9.3\mingw_64\bin;%PATH%

# Linux/macOS
export PATH=/path/to/Qt/6.9.3/gcc_64/bin:$PATH
```

---

**Problem**: Missing MAVLink headers

**Solution**: Ensure the `third-party/mavlink` directory exists with MAVLink C headers. Clone the submodule:
```bash
git submodule update --init --recursive
```

## Contributing

Contributions are welcome! Please follow these guidelines:

1. **Fork the repository**
2. **Create a feature branch**: `git checkout -b feature/amazing-feature`
3. **Write tests** for new functionality
4. **Follow code style** guidelines
5. **Commit changes**: Use descriptive commit messages
6. **Push to branch**: `git push origin feature/amazing-feature`
7. **Open Pull Request**

### Reporting Issues

When reporting bugs, please include:
- FlightScope version
- Operating system and version
- Qt version
- Steps to reproduce
- `flightscope_debug.log` contents
- Screenshots (if applicable)

## License

This project is licensed under the MIT License - see LICENSE file for details.

## Acknowledgments

- **MAVLink**: https://mavlink.io/
- **Qt Framework**: https://www.qt.io/
- **ArduPilot**: https://ardupilot.org/
- **PX4**: https://px4.io/

## Roadmap

- [x] Phase 1: Core Telemetry & Connection
- [ ] Phase 2: Flight Planning & Waypoints
- [ ] Phase 3: Map Integration
- [ ] Phase 4: Advanced Features
- [ ] Phase 5: Testing & Deployment

## Contact

**Project**: https://github.com/snithinbabunaidu/FlightScope

**Issues**: https://github.com/snithinbabunaidu/FlightScope/issues

---

🤖 Built with [Claude Code](https://claude.com/claude-code)
