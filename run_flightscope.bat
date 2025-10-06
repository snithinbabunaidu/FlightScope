@echo off
REM FlightScope Launch Script
REM This script sets up the Qt environment and runs FlightScope

SET PATH=C:\Qt\Tools\mingw1310_64\bin;C:\Qt\6.9.3\mingw_64\bin;%PATH%

echo Starting FlightScope Ground Control Station...
start "" release\FlightScope.exe

echo FlightScope has been launched!
pause
