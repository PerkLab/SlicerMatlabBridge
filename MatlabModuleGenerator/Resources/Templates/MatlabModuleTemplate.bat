@set MODULE_NAME=MatlabModuleTemplate
@rem MatlabBridge proxy version: 1.0

@echo off
if not "%1"=="--xml" goto :launch_module

:print_xml
rem Print CLI descriptor XML
type %MODULE_NAME%.xml
rem Return with success
exit /b 0

:launch_module
rem Print error messages if any of the required environment variables are not set
if not defined SLICER_MATLAB_COMMANDER_PATH echo ERROR: SLICER_MATLAB_COMMANDER_PATH environment variable is not defined. Make sure you installed the MatlabBridge extension.
if not defined SLICER_MATLAB_COMMAND_SERVER_SCRIPT_PATH echo ERROR: SLICER_MATLAB_COMMAND_SERVER_SCRIPT_PATH environment variable is not defined. Make sure you installed the MatlabBridge extension.
if not defined SLICER_MATLAB_EXECUTABLE_PATH echo ERROR: SLICER_MATLAB_EXECUTABLE_PATH environment variable is not defined. Make sure you installed the MatlabBridge extension and set the path to the Matlab executable.
rem Make the .bat file location to be the working directory (that's where the MatlabCommander.exe is)
pushd "%~dp0"
rem Forward parameters to the Matlab CLI
%SLICER_HOME%/Slicer.exe --launcher-no-splash --launch %SLICER_MATLAB_COMMANDER_PATH% --call-matlab-function %MODULE_NAME% %*
if errorlevel 1 exit /b 1
rem Return with success
exit /b 0
