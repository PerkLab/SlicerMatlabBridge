/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// MatlabModuleGenerator Logic includes
#include "vtkSlicerMatlabModuleGeneratorLogic.h"

// MRML includes

// VTK includes
#include <vtkNew.h>
#include <vtksys/SystemTools.hxx>

// STD includes
#include <cassert>
#include <algorithm> // for std::remove

// Slicer includes
#include "vtkSlicerConfigure.h" // For Slicer_CLIMODULES_SUBDIR

static const std::string TEMPLATE_NAME="MatlabModuleTemplate";
/* Define case insensitive string compare for all supported platforms. */
#if defined( _WIN32 ) && !defined(__CYGWIN__)
  static const std::string MODULE_PROXY_TEMPLATE_EXTENSION=".bat";
  static const std::string MATLAB_COMMANDER_EXECUTABLE_NAME="MatlabCommander.exe";
#else
  static const std::string MODULE_PROXY_TEMPLATE_EXTENSION="";
  static const std::string MATLAB_COMMANDER_EXECUTABLE_NAME="MatlabCommander";
  #include <sys/stat.h>
#endif
static const std::string MODULE_SCRIPT_TEMPLATE_EXTENSION=".m";
static const std::string MODULE_DEFINITION_TEMPLATE_EXTENSION=".xml";


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerMatlabModuleGeneratorLogic);

//----------------------------------------------------------------------------
vtkSlicerMatlabModuleGeneratorLogic::vtkSlicerMatlabModuleGeneratorLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerMatlabModuleGeneratorLogic::~vtkSlicerMatlabModuleGeneratorLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerMatlabModuleGeneratorLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerMatlabModuleGeneratorLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerMatlabModuleGeneratorLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerMatlabModuleGeneratorLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerMatlabModuleGeneratorLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerMatlabModuleGeneratorLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{  
}

//---------------------------------------------------------------------------
const char* vtkSlicerMatlabModuleGeneratorLogic
::GetMatlabModuleDirectory()
{
  // Find out where the generated Matlab modules are
  std::string dir=this->GetModuleShareDirectory()+"/../../../../../MatlabModules";
  this->MatlabModuleDirectory=vtksys::SystemTools::CollapseFullPath(dir.c_str());     
  return this->MatlabModuleDirectory.c_str();
}

//---------------------------------------------------------------------------
const char* vtkSlicerMatlabModuleGeneratorLogic
::GetMatlabCommandServerDirectory()
{
  // Find out where the commandserver Matlab files are
  std::string dir=this->GetModuleShareDirectory()+"/../../../../"+Slicer_CLIMODULES_BIN_DIR+"/commandserver";
  this->MatlabCommandServerDirectory=vtksys::SystemTools::CollapseFullPath(dir.c_str());  
  return this->MatlabCommandServerDirectory.c_str();
}

//---------------------------------------------------------------------------
const char* vtkSlicerMatlabModuleGeneratorLogic
::GetMatlabCommanderPath()
{
  // Find out where the MatlabCommander CLI is
  // Retrieve the bin directory from the known path of the share directory 
  std::string climodulesDir=this->GetModuleShareDirectory()+"/../../../../"+Slicer_CLIMODULES_BIN_DIR;
  climodulesDir=vtksys::SystemTools::CollapseFullPath(climodulesDir.c_str());
  this->MatlabCommanderPath=climodulesDir+"/"+MATLAB_COMMANDER_EXECUTABLE_NAME;
#if defined( _WIN32 ) && !defined(__CYGWIN__)
  if (!vtksys::SystemTools::FileExists(this->MatlabCommanderPath.c_str(),true))
  {
    // The executable does not exist at the expected location, so probably it is a Windows build tree
    // where executables are in the Debug or Release subdirectory
    #ifdef _DEBUG
      this->MatlabCommanderPath=climodulesDir+"/Debug/"+MATLAB_COMMANDER_EXECUTABLE_NAME;
    #else
      this->MatlabCommanderPath=climodulesDir+"/Release/"+MATLAB_COMMANDER_EXECUTABLE_NAME;
    #endif
  }
#endif
  return this->MatlabCommanderPath.c_str();
}

//---------------------------------------------------------------------------
const char* vtkSlicerMatlabModuleGeneratorLogic
::GenerateModule(const char* inputModuleName)
{  
  this->GenerateModuleResult.clear();

  std::string fullModuleName=inputModuleName; // module name, including spaces  
  std::string moduleNameNoSpaces=inputModuleName; // module name without spaces
  moduleNameNoSpaces.erase(std::remove(moduleNameNoSpaces.begin(), moduleNameNoSpaces.end(), ' '), moduleNameNoSpaces.end());

  vtkStdString result;
  bool success=true;

  std::string targetDir=GetMatlabModuleDirectory();

  if (! vtksys::SystemTools::MakeDirectory(targetDir.c_str()))
  {
    this->GenerateModuleResult+="ERROR: Unable to create Matlab module directory '"+targetDir+"'\n";
    success=false;
  } 
 
  // Matlab .m file
  if (!CreateFileFromTemplate(this->GetModuleShareDirectory()+"/"+TEMPLATE_NAME+MODULE_SCRIPT_TEMPLATE_EXTENSION,
    targetDir+"/"+moduleNameNoSpaces+MODULE_SCRIPT_TEMPLATE_EXTENSION, TEMPLATE_NAME, moduleNameNoSpaces, result))
  {
    success=false;
  }
  this->GenerateModuleResult+=result+"\n";

  // Module description .xml file
  if (!CreateFileFromTemplate(this->GetModuleShareDirectory()+"/"+TEMPLATE_NAME+MODULE_DEFINITION_TEMPLATE_EXTENSION,
    targetDir+"/"+moduleNameNoSpaces+MODULE_DEFINITION_TEMPLATE_EXTENSION, TEMPLATE_NAME, fullModuleName, result))
  {
    success=false;
  }
  this->GenerateModuleResult+=result+"\n";

  // Proxy .bat or .sh file
  std::string proxyTargetFilePath=targetDir+"/"+moduleNameNoSpaces+MODULE_PROXY_TEMPLATE_EXTENSION;
  if (!CreateFileFromTemplate(this->GetModuleShareDirectory()+"/"+TEMPLATE_NAME+MODULE_PROXY_TEMPLATE_EXTENSION,
    proxyTargetFilePath, TEMPLATE_NAME, moduleNameNoSpaces, result))
  {
    success=false;
  }
  else
  {
#if defined( _WIN32 ) && !defined(__CYGWIN__)
      // no need to change file attributes to make the proxy executable
#else
    mode_t mode=S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH;
    vtksys::SystemTools::GetPermissions(proxyTargetFilePath.c_str(), mode);
    mode |= S_IXUSR|S_IXGRP|S_IXOTH; // set executable permission for user, group, and others
    if (!vtksys::SystemTools::SetPermissions(proxyTargetFilePath.c_str(),mode)) 
    {
      result+=" Failed to set the file permission to allow execution - Slicer may not discover this module until the file permission is set allow execution.";
    }
#endif
  }
  this->GenerateModuleResult+=result+"\n";

  if (success)
  {
    this->GenerateModuleResult+="\nModule generation was successful.\nEdit the module descriptor .xml and the .m file then restart Slicer.";
  }
  else
  {
    this->GenerateModuleResult+="\nModule generation failed";
  }
  return this->GenerateModuleResult.c_str();
}

//---------------------------------------------------------------------------
bool vtkSlicerMatlabModuleGeneratorLogic
::CreateFileFromTemplate(const vtkStdString& templateFilename, const vtkStdString& targetFilename, const vtkStdString& originalString, const vtkStdString& modifiedString, vtkStdString &result)
{
  result.clear();

  if (vtksys::SystemTools::FileExists(targetFilename.c_str(),true))
  {
    // Prevent accidental overwriting of existing valuable file with auto-generated file
    result="Cannot create file, it already exists:\n "+targetFilename;
    return false;
  }

  // Open input file
  fstream templateFile;
  templateFile.open(templateFilename.c_str(), fstream::in);
  if (!templateFile.is_open())
  {
    result="Template file not found:\n "+templateFilename;
    return false;
  }

  // Open output file
  std::ofstream targetFile;
  targetFile.open(targetFilename.c_str() );
  if (!targetFile.is_open())
  {
    result="Target file cannot be opened for writing:\n "+targetFilename;
    templateFile.close(); 
    return false;
  }  

  // Copy line-by-line while replacing the original string with the modified string
  for (std::string line; std::getline(templateFile, line); ) 
  {
    if (line.length()>0 && originalString.length())
    {
      // search and replace
      size_t idx = 0;
      for (;;) 
      {
        idx = line.find( originalString, idx);
        if (idx == std::string::npos)  break;
        line.replace( idx, originalString.length(), modifiedString);
        idx += modifiedString.length();
      }
    }
    targetFile << line << std::endl;
  }

  // Close input and output files
  templateFile.close(); 
  targetFile.close(); 

  result="File created:\n "+targetFilename;
  return true;
}

//---------------------------------------------------------------------------
void vtkSlicerMatlabModuleGeneratorLogic
::SetMatlabExecutablePath(const char* matlabExePath)
{
  this->MatlabExecutablePath = (matlabExePath==NULL)?"":matlabExePath;

  std::string matlabEnvVar=std::string("SLICER_MATLAB_EXECUTABLE_PATH=")+this->MatlabExecutablePath;
  vtksys::SystemTools::PutEnv(matlabEnvVar.c_str());
}


//---------------------------------------------------------------------------
const char* vtkSlicerMatlabModuleGeneratorLogic
::GetMatlabExecutablePath()
{
  return this->MatlabExecutablePath.c_str();
}
