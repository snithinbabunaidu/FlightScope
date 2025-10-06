@echo off
REM FlightScope Build Script
REM This script builds the FlightScope application

SET PATH=C:\Qt\Tools\mingw1310_64\bin;C:\Qt\6.9.3\mingw_64\bin;%PATH%

echo ========================================
echo Building FlightScope...
echo ========================================

REM Run qmake
qmake FlightScope.pro
if %ERRORLEVEL% NEQ 0 (
    echo qmake failed!
    pause
    exit /b 1
)

REM Run make
mingw32-make -j4
if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo ========================================
echo Build completed successfully!
echo ========================================
echo.
echo Executable location: release\FlightScope.exe
echo.
echo Run 'run_flightscope.bat' to start the application
echo.
pause
