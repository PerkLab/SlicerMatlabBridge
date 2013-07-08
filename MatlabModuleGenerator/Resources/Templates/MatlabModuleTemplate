#!/bin/sh
# If this proxy is called with the --xml parameter then just return the module decriptor XML file,
# otherwise run the associated Matlab script.
# MatlabBridge proxy version: 1.0

MODULE_NAME="MatlabModuleTemplate"

if [ $1 = "--xml" ]; then
  cat $MODULE_NAME.xml
  exit 0
fi

# Make the MatlabCommander path the current working directory (otherwise it does not find the .so file)
cd "$( dirname "$SLICER_MATLAB_COMMANDER_PATH" )"

# Forward parameters to the Matlab CLI
"$SLICER_HOME/Slicer" --launcher-no-splash --launch "$SLICER_MATLAB_COMMANDER_PATH" --call-matlab-function $MODULE_NAME $*
exit $?
