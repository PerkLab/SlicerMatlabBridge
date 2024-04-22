cmake_minimum_required(VERSION 3.5)

project(MatlabBridge)

#-----------------------------------------------------------------------------
OPTION(MATLABBRIDGE_ENABLE_MATLAB_TESTING "Enable using of Matlab for testing. Turn this option requires Matlab to be installed on this computer." OFF)

# ------------------------------------------------------------------------
set(MATLABBRIDGE_VERSION_MAJOR "0")
set(MATLABBRIDGE_VERSION_MINOR "14")
set(MATLABBRIDGE_VERSION_PATCH "0")
set(MATLABBRIDGE_VERSION ${MATLABBRIDGE_VERSION_MAJOR}.${MATLABBRIDGE_VERSION_MINOR}.${MATLABBRIDGE_VERSION_PATCH})

#-----------------------------------------------------------------------------
set(EXTENSION_HOMEPAGE "https://www.slicer.org/slicerWiki/index.php/Documentation/Nightly/Extensions/MatlabBridge")
set(EXTENSION_CATEGORY "Developer Tools")
set(EXTENSION_CONTRIBUTORS "Andras Lasso (PerkLab, Queen's University), Jean-Christophe Fillion-Robin (Kitware), Kevin Wang (Radiation Medicine Program, Princess Margaret Hospital, University Health Network Toronto)")
set(EXTENSION_DESCRIPTION "The Matlab Bridge extension allows running Matlab scripts as command-line interface (CLI) modules directly from 3D Slicer. The only prerequisites for running Matlab scripts are having this extension and Matlab installed on the 3D Slicer computer (building of 3D Slicer, MEX files, etc. is not needed). Extension version: ${MATLABBRIDGE_VERSION}.")
set(EXTENSION_ICONURL "https://www.slicer.org/slicerWiki/images/e/e8/MatlabBridgeLogo.png")
set(EXTENSION_SCREENSHOTURLS "https://www.slicer.org/slicerWiki/images/2/2f/MatlabBridgeScreenshot1.png https://www.slicer.org/slicerWiki/images/1/16/MatlabBridgeScreenshot2.png https://www.slicer.org/slicerWiki/images/b/b9/MatlabBridgeScreenshot3.png")
set(EXTENSION_DEPENDS SlicerOpenIGTLink)

#-----------------------------------------------------------------------------
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
add_subdirectory(MatlabCommander)
add_subdirectory(MatlabModuleGenerator )

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_GENERATE_CONFIG})
include(${Slicer_EXTENSION_CPACK})
