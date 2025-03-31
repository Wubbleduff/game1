
@echo off


REM -------------------------------- Common --------------------------------

set NAME=game

set SRC=src

REM Avoid C runtime library
REM https://hero.handmade.network/forums/code-discussion/t/94-guide_-_how_to_avoid_c_c++_runtime_on_windows
set COMMON_COMPILE_FLAGS=/Od /c /W4 /WX /EHsc /std:c17 /GS- /Gs9999999
set INCLUDE_DIRS=/I"src"
set DEBUG_COMPILE_FLAGS=/DDEBUG /Zi
set DEBUG_LINK_FLAGS=/NODEFAULTLIB /STACK:0x100000,0x100000 /SUBSYSTEM:WINDOWS /MACHINE:X64 /DEBUG:FULL
set LIBS=kernel32.lib user32.lib gdi32.lib opengl32.lib Ws2_32.lib




REM -------------------------------- Clang --------------------------------

set CLANG_COMPILE_FLAGS=-march=skylake

set CLANG_INTERMEDIATE_SERVER_DIR=.\clang_build_intermediate\server
set CLANG_DEPLOY_DEBUG_SERVER_DIR=.\clang_deploy\debug\server

mkdir %CLANG_INTERMEDIATE_SERVER_DIR%
mkdir %CLANG_DEPLOY_DEBUG_SERVER_DIR%

clang-cl %COMMON_COMPILE_FLAGS% %CLANG_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%CLANG_INTERMEDIATE_SERVER_DIR%\game_state.obj %SRC%\game_state.c
if %errorlevel% neq 0 exit /b %errorlevel%

clang-cl %COMMON_COMPILE_FLAGS% %CLANG_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%CLANG_INTERMEDIATE_SERVER_DIR%\platform_win32_common.obj %SRC%\platform_win32\platform_win32_common.c
if %errorlevel% neq 0 exit /b %errorlevel%

clang-cl %COMMON_COMPILE_FLAGS% %CLANG_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%CLANG_INTERMEDIATE_SERVER_DIR%\platform_win32_crt.obj %SRC%\platform_win32\platform_win32_crt.c
if %errorlevel% neq 0 exit /b %errorlevel%

clang-cl %COMMON_COMPILE_FLAGS% %CLANG_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%CLANG_INTERMEDIATE_SERVER_DIR%\platform_win32_server_main.obj %SRC%\platform_win32\platform_win32_server_main.c
if %errorlevel% neq 0 exit /b %errorlevel%

clang-cl %COMMON_COMPILE_FLAGS% %CLANG_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%CLANG_INTERMEDIATE_SERVER_DIR%\platform_win32_render.obj %SRC%\platform_win32\platform_win32_render.c
if %errorlevel% neq 0 exit /b %errorlevel%

clang-cl %COMMON_COMPILE_FLAGS% %CLANG_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%CLANG_INTERMEDIATE_SERVER_DIR%\platform_win32_input.obj %SRC%\platform_win32\platform_win32_input.c
if %errorlevel% neq 0 exit /b %errorlevel%

clang-cl %COMMON_COMPILE_FLAGS% %CLANG_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%CLANG_INTERMEDIATE_SERVER_DIR%\platform_win32_network_server.obj %SRC%\platform_win32\platform_win32_network_server.c
if %errorlevel% neq 0 exit /b %errorlevel%

lld-link %DEBUG_LINK_FLAGS% %LIBS% %CLANG_INTERMEDIATE_SERVER_DIR%\*.obj /OUT:"%CLANG_INTERMEDIATE_SERVER_DIR%\%NAME%.exe"
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /Y %CLANG_INTERMEDIATE_SERVER_DIR%\%NAME%.exe %CLANG_DEPLOY_DEBUG_SERVER_DIR%
xcopy /Y %CLANG_INTERMEDIATE_SERVER_DIR%\%NAME%.pdb %CLANG_DEPLOY_DEBUG_SERVER_DIR%

echo.
echo %CLANG_DEPLOY_DEBUG_SERVER_DIR%\%NAME%.exe
echo.


set CLANG_INTERMEDIATE_CLIENT_DIR=.\clang_build_intermediate\client
set CLANG_DEPLOY_DEBUG_CLIENT_DIR=.\clang_deploy\debug\client

mkdir %CLANG_INTERMEDIATE_CLIENT_DIR%
mkdir %CLANG_DEPLOY_DEBUG_CLIENT_DIR%

clang-cl %COMMON_COMPILE_FLAGS% %CLANG_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%CLANG_INTERMEDIATE_CLIENT_DIR%\game_state.obj %SRC%\game_state.c
if %errorlevel% neq 0 exit /b %errorlevel%

clang-cl %COMMON_COMPILE_FLAGS% %CLANG_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%CLANG_INTERMEDIATE_CLIENT_DIR%\platform_win32_common.obj %SRC%\platform_win32\platform_win32_common.c
if %errorlevel% neq 0 exit /b %errorlevel%

clang-cl %COMMON_COMPILE_FLAGS% %CLANG_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%CLANG_INTERMEDIATE_CLIENT_DIR%\platform_win32_crt.obj %SRC%\platform_win32\platform_win32_crt.c
if %errorlevel% neq 0 exit /b %errorlevel%

clang-cl %COMMON_COMPILE_FLAGS% %CLANG_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%CLANG_INTERMEDIATE_CLIENT_DIR%\platform_win32_client_main.obj %SRC%\platform_win32\platform_win32_client_main.c
if %errorlevel% neq 0 exit /b %errorlevel%

clang-cl %COMMON_COMPILE_FLAGS% %CLANG_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%CLANG_INTERMEDIATE_CLIENT_DIR%\platform_win32_render.obj %SRC%\platform_win32\platform_win32_render.c
if %errorlevel% neq 0 exit /b %errorlevel%

clang-cl %COMMON_COMPILE_FLAGS% %CLANG_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%CLANG_INTERMEDIATE_CLIENT_DIR%\platform_win32_input.obj %SRC%\platform_win32\platform_win32_input.c
if %errorlevel% neq 0 exit /b %errorlevel%

clang-cl %COMMON_COMPILE_FLAGS% %CLANG_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%CLANG_INTERMEDIATE_CLIENT_DIR%\platform_win32_network_client.obj %SRC%\platform_win32\platform_win32_network_client.c
if %errorlevel% neq 0 exit /b %errorlevel%

lld-link %DEBUG_LINK_FLAGS% %LIBS% %CLANG_INTERMEDIATE_CLIENT_DIR%\*.obj /OUT:"%CLANG_INTERMEDIATE_CLIENT_DIR%\%NAME%.exe"
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /Y %CLANG_INTERMEDIATE_CLIENT_DIR%\%NAME%.exe %CLANG_DEPLOY_DEBUG_CLIENT_DIR%
xcopy /Y %CLANG_INTERMEDIATE_CLIENT_DIR%\%NAME%.pdb %CLANG_DEPLOY_DEBUG_CLIENT_DIR%

echo.
echo %CLANG_DEPLOY_DEBUG_CLIENT_DIR%\%NAME%.exe
echo.


REM -------------------------------- MSVC --------------------------------

set MSVC_INTERMEDIATE_SERVER_DIR=.\msvc_build_intermediate\server
set MSVC_DEPLOY_DEBUG_SERVER_DIR=.\msvc_deploy\debug\server

mkdir %MSVC_INTERMEDIATE_SERVER_DIR%
mkdir %MSVC_DEPLOY_DEBUG_SERVER_DIR%

cl %COMMON_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%MSVC_INTERMEDIATE_SERVER_DIR%\game_state.obj %SRC%\game_state.c
if %errorlevel% neq 0 exit /b %errorlevel%

cl %COMMON_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%MSVC_INTERMEDIATE_SERVER_DIR%\platform_win32_common.obj %SRC%\platform_win32\platform_win32_common.c
if %errorlevel% neq 0 exit /b %errorlevel%

cl %COMMON_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%MSVC_INTERMEDIATE_SERVER_DIR%\platform_win32_crt.obj %SRC%\platform_win32\platform_win32_crt.c
if %errorlevel% neq 0 exit /b %errorlevel%

cl %COMMON_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%MSVC_INTERMEDIATE_SERVER_DIR%\platform_win32_server_main.obj %SRC%\platform_win32\platform_win32_server_main.c
if %errorlevel% neq 0 exit /b %errorlevel%

cl %COMMON_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%MSVC_INTERMEDIATE_SERVER_DIR%\platform_win32_render.obj %SRC%\platform_win32\platform_win32_render.c
if %errorlevel% neq 0 exit /b %errorlevel%

cl %COMMON_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%MSVC_INTERMEDIATE_SERVER_DIR%\platform_win32_input.obj %SRC%\platform_win32\platform_win32_input.c
if %errorlevel% neq 0 exit /b %errorlevel%

cl %COMMON_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%MSVC_INTERMEDIATE_SERVER_DIR%\platform_win32_network_server.obj %SRC%\platform_win32\platform_win32_network_server.c
if %errorlevel% neq 0 exit /b %errorlevel%

link %DEBUG_LINK_FLAGS% %LIBS% %MSVC_INTERMEDIATE_SERVER_DIR%\*.obj /OUT:"%MSVC_INTERMEDIATE_SERVER_DIR%\%NAME%.exe"
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /Y %MSVC_INTERMEDIATE_SERVER_DIR%\%NAME%.exe %MSVC_DEPLOY_DEBUG_SERVER_DIR%
xcopy /Y %MSVC_INTERMEDIATE_SERVER_DIR%\%NAME%.pdb %MSVC_DEPLOY_DEBUG_SERVER_DIR%

echo.
echo %MSVC_DEPLOY_DEBUG_SERVER_DIR%\%NAME%.exe
echo.


set MSVC_INTERMEDIATE_CLIENT_DIR=.\msvc_build_intermediate\client
set MSVC_DEPLOY_DEBUG_CLIENT_DIR=.\msvc_deploy\debug\client

mkdir %MSVC_INTERMEDIATE_CLIENT_DIR%
mkdir %MSVC_DEPLOY_DEBUG_CLIENT_DIR%

cl %COMMON_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%MSVC_INTERMEDIATE_CLIENT_DIR%\game_state.obj %SRC%\game_state.c
if %errorlevel% neq 0 exit /b %errorlevel%

cl %COMMON_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%MSVC_INTERMEDIATE_CLIENT_DIR%\platform_win32_common.obj %SRC%\platform_win32\platform_win32_common.c
if %errorlevel% neq 0 exit /b %errorlevel%

cl %COMMON_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%MSVC_INTERMEDIATE_CLIENT_DIR%\platform_win32_crt.obj %SRC%\platform_win32\platform_win32_crt.c
if %errorlevel% neq 0 exit /b %errorlevel%

cl %COMMON_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%MSVC_INTERMEDIATE_CLIENT_DIR%\platform_win32_client_main.obj %SRC%\platform_win32\platform_win32_client_main.c
if %errorlevel% neq 0 exit /b %errorlevel%

cl %COMMON_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%MSVC_INTERMEDIATE_CLIENT_DIR%\platform_win32_render.obj %SRC%\platform_win32\platform_win32_render.c
if %errorlevel% neq 0 exit /b %errorlevel%

cl %COMMON_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%MSVC_INTERMEDIATE_CLIENT_DIR%\platform_win32_input.obj %SRC%\platform_win32\platform_win32_input.c
if %errorlevel% neq 0 exit /b %errorlevel%

cl %COMMON_COMPILE_FLAGS% %DEBUG_COMPILE_FLAGS% %INCLUDE_DIRS% /Fo%MSVC_INTERMEDIATE_CLIENT_DIR%\platform_win32_network_client.obj %SRC%\platform_win32\platform_win32_network_client.c
if %errorlevel% neq 0 exit /b %errorlevel%

link %DEBUG_LINK_FLAGS% %LIBS% %MSVC_INTERMEDIATE_CLIENT_DIR%\*.obj /OUT:"%MSVC_INTERMEDIATE_CLIENT_DIR%\%NAME%.exe"
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /Y %MSVC_INTERMEDIATE_CLIENT_DIR%\%NAME%.exe %MSVC_DEPLOY_DEBUG_CLIENT_DIR%
xcopy /Y %MSVC_INTERMEDIATE_CLIENT_DIR%\%NAME%.pdb %MSVC_DEPLOY_DEBUG_CLIENT_DIR%

echo.
echo %MSVC_DEPLOY_DEBUG_CLIENT_DIR%\%NAME%.exe
echo.


