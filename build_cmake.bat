@echo off
REM FlightScope CMake Build Script for Windows

echo ========================================
echo FlightScope CMake Build Script
echo ========================================

REM Set Qt and MinGW paths
SET PATH=C:\Qt\Tools\mingw1310_64\bin;C:\Qt\6.9.3\mingw_64\bin;%PATH%
SET Qt6_DIR=C:\Qt\6.9.3\mingw_64\lib\cmake\Qt6

REM Clean previous build
if exist build-cmake rmdir /s /q build-cmake
mkdir build-cmake
cd build-cmake

REM Configure with CMake
echo.
echo [1/3] Configuring with CMake...
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=C:\Qt\6.9.3\mingw_64

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake configuration failed!
    cd ..
    pause
    exit /b 1
)

REM Build
echo.
echo [2/3] Building FlightScope...
cmake --build . --config Release

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Build failed!
    cd ..
    pause
    exit /b 1
)

REM Deploy Qt dependencies
echo.
echo [3/3] Deploying Qt dependencies...
windeployqt release\FlightScope.exe --qmldir ..\resources\qml

if %ERRORLEVEL% NEQ 0 (
    echo WARNING: windeployqt failed, but executable may still work
)

echo.
echo ========================================
echo Build completed successfully!
echo Executable: build-cmake\release\FlightScope.exe
echo ========================================
echo.

cd ..
pause
