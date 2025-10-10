@echo off
REM FlightScope Portable Package Creator
REM Creates a clean, shareable zip file

echo ========================================
echo FlightScope Portable Package Creator
echo ========================================

if not exist "release\FlightScope.exe" (
    echo ERROR: Release build not found!
    echo Please run build.bat first.
    pause
    exit /b 1
)

echo.
echo [1/3] Creating clean package directory...
if exist "FlightScope-Portable" rmdir /s /q FlightScope-Portable
mkdir FlightScope-Portable

echo.
echo [2/3] Copying files...
xcopy /E /I /Y release FlightScope-Portable

REM Remove unnecessary build artifacts (keep only runtime files)
del /Q FlightScope-Portable\*.o 2>nul
del /Q FlightScope-Portable\*.cpp 2>nul

REM Create README
echo FlightScope Ground Control Station v1.0.0 > FlightScope-Portable\README.txt
echo ============================================ >> FlightScope-Portable\README.txt
echo. >> FlightScope-Portable\README.txt
echo QUICK START: >> FlightScope-Portable\README.txt
echo 1. Double-click FlightScope.exe >> FlightScope-Portable\README.txt
echo 2. Click "Connect" button >> FlightScope-Portable\README.txt
echo 3. Select connection type: >> FlightScope-Portable\README.txt
echo    - UDP Server (for ArduPilot SITL): 127.0.0.1:14550 >> FlightScope-Portable\README.txt
echo    - TCP (for PX4): 127.0.0.1:5760 >> FlightScope-Portable\README.txt
echo    - Serial (for real drone via USB) >> FlightScope-Portable\README.txt
echo 4. Click Connect! >> FlightScope-Portable\README.txt
echo. >> FlightScope-Portable\README.txt
echo FEATURES: >> FlightScope-Portable\README.txt
echo - Real-time MAVLink telemetry >> FlightScope-Portable\README.txt
echo - Mission planning with waypoint editor >> FlightScope-Portable\README.txt
echo - Interactive map with drone tracking >> FlightScope-Portable\README.txt
echo - Geofence management >> FlightScope-Portable\README.txt
echo - Live HUD and flight data >> FlightScope-Portable\README.txt
echo - Responsive UI (resize window to see layouts!) >> FlightScope-Portable\README.txt
echo. >> FlightScope-Portable\README.txt
echo SYSTEM REQUIREMENTS: >> FlightScope-Portable\README.txt
echo - Windows 10 or later (64-bit) >> FlightScope-Portable\README.txt
echo - No installation required! >> FlightScope-Portable\README.txt
echo. >> FlightScope-Portable\README.txt
echo For more information: https://github.com/yourrepo/FlightScope >> FlightScope-Portable\README.txt

echo.
echo [3/3] Creating ZIP file...
if exist "FlightScope-v1.0.0-Windows-Portable.zip" del FlightScope-v1.0.0-Windows-Portable.zip
powershell -Command "Compress-Archive -Path FlightScope-Portable\* -DestinationPath FlightScope-v1.0.0-Windows-Portable.zip -Force"

echo.
echo ========================================
echo SUCCESS!
echo ========================================
echo.
echo Package created: FlightScope-v1.0.0-Windows-Portable.zip
echo Size:
powershell -Command "(Get-Item FlightScope-v1.0.0-Windows-Portable.zip).Length / 1MB | ForEach-Object { '{0:N2} MB' -f $_ }"
echo.
echo READY TO SHARE!
echo 1. Send this ZIP to anyone with Windows
echo 2. They unzip and run FlightScope.exe
echo 3. No installation needed!
echo.
echo ========================================

pause
