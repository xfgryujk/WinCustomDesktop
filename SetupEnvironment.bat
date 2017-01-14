@echo off

set DIRS=Win32\Debug Win32\Release x64\Debug x64\Release

for %%i in (%DIRS%) do (
md %%i\Plugin\Data
xcopy Data %%i\Plugin\Data /e /y
echo [VideoDesktop] > %%i\Plugins.ini
echo Enable=0 >> %%i\Plugins.ini
echo [DesktopBrowser] >> %%i\Plugins.ini
echo Enable=1 >> %%i\Plugins.ini
echo [MaskDesktop] >> %%i\Plugins.ini
echo Enable=0 >> %%i\Plugins.ini
echo [WIMC] >> %%i\Plugins.ini
echo Enable=1 >> %%i\Plugins.ini
)

pause
