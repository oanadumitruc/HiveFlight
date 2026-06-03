@echo off
REM Build script for Windows


setlocal
set SCRIPT_DIR=%~dp0
set BUILD_DIR=%SCRIPT_DIR%build

cmake -S "%SCRIPT_DIR%" -B "%BUILD_DIR%" -DHIVEFLIGHT_BUILD_OPENGL_VIEWER=ON
if errorlevel 1 exit /b %errorlevel%

cmake --build "%BUILD_DIR%" --parallel
if errorlevel 1 exit /b %errorlevel%
echo.
echo Build complete.
echo Executables are in: %BUILD_DIR%
echo If OpenGL/GLU/GLUT were found, run: %BUILD_DIR%hiveflight_gl_viewer.exe
endlocal