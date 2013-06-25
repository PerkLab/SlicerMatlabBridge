@set MODULE_NAME=MatlabModuleTemplate

@echo off
if "%1"=="--xml" goto :print_xml

rem Make the .bat file location to be the working directory (that's where the MatlabCommander.exe is)
pushd "%~dp0"
rem Forward parameters to the Matlab CLI
"%SLICER_HOME%/Slicer.exe" --launcher-no-splash --launch MatlabCommander.exe --call-matlab-function %MODULE_NAME% %*
if errorlevel 1 exit /b 1
rem Return with success
exit /b 0

rem Print CLI descriptor XML
:print_xml
type %MODULE_NAME%.xml
rem Return with success
exit /b 0
