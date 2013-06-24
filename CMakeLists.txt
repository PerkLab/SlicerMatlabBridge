cmake_minimum_required(VERSION 2.8.9)

project(MatlabBridge)

# ------------------------------------------------------------------------
set(MATLABBRIDGE_VERSION_MAJOR "0")
set(MATLABBRIDGE_VERSION_MINOR "1")
set(MATLABBRIDGE_VERSION_PATCH "0")
set(MATLABBRIDGE_VERSION ${MATLABBRIDGE_VERSION_MAJOR}.${MATLABBRIDGE_VERSION_MINOR}.${MATLABBRIDGE_VERSION_PATCH})


#-----------------------------------------------------------------------------
set(EXTENSION_HOMEPAGE "http://www.slicer.org/slicerWiki/index.php/Documentation/Nightly/Extensions/MatlabBridge")
set(EXTENSION_CATEGORY "Developer Tools")
set(EXTENSION_CONTRIBUTORS "Andras Lasso (PerkLab, Queen's University), Jean-Christophe Fillion-Robin (Kitware), Kevin Wang (Radiation Medicine Program, Princess Margaret Hospital, University Health Network Toronto)")
set(EXTENSION_DESCRIPTION "Extension allowing to run Matlab scripts directly from 3D Slicer, as command-line interface (CLI) modules. Version ${MATLABBRIDGE_VERSION}")
set(EXTENSION_ICONURL "http://viewvc.slicer.org/viewvc.cgi/Slicer4/trunk/Extensions/Testing/MatlabBridge/MatlabBridge.png?revision=21746&view=co")
set(EXTENSION_SCREENSHOTURLS "http://wiki.slicer.org/slicerWiki/images/4/42/Slicer-r19441-MatlabBridge-screenshot.png")

#-----------------------------------------------------------------------------
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
add_subdirectory(MatlabCommander)
add_subdirectory(MatlabModuleGenerator )

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_CPACK})
