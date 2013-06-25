#!/bin/sh
# If this proxy is called with the --xml parameter then just return the module decriptor XML file,
# otherwise run the associated Matlab script.

MODULE_NAME="MatlabModuleTemplate"

if [ $1 = "--xml" ]; then
  cat $MODULE_NAME.xml
  exit 0
fi

# Make this .sh file's location the working directory (that's where the MatlabCommander is)
cd "$( dirname "$0" )"

# Forward parameters to the Matlab CLI
$SLICER_HOME/Slicer --launcher-no-splash --launch MatlabCommander --call-matlab-function $MODULE_NAME $*
exit $?
