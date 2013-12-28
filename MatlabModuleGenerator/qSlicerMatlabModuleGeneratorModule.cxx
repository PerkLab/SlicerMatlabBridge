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

// Qt includes
#include <QDebug> 
#include <QDir>
#include <QtPlugin>
#include <QSettings> 

// VTK includes
#include "vtksys/SystemTools.hxx"

// SlicerQt includes
#include <qSlicerApplication.h>

// MatlabModuleGenerator Logic includes
#include <vtkSlicerMatlabModuleGeneratorLogic.h>

// MatlabModuleGenerator includes
#include "qSlicerMatlabModuleGeneratorModule.h"
#include "qSlicerMatlabModuleGeneratorModuleWidget.h"

const std::string MATLAB_COMMAND_SERVER_SCRIPT_NAME="cli_commandserver.m";

#if defined( _WIN32 ) && !defined(__CYGWIN__)
  // Windows
  static const std::string DEFAULT_MATLAB_PROCESS_PATH="matlab.exe";
#elif defined(__APPLE__)
  // Mac OS X
  static const std::string DEFAULT_MATLAB_PROCESS_PATH="/Applications/MATLAB_XXXXX.app/bin/matlab";
#else
  // Linux
  static const std::string DEFAULT_MATLAB_PROCESS_PATH="/usr/local/bin/matlab";
#endif

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerMatlabModuleGeneratorModule, qSlicerMatlabModuleGeneratorModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerMatlabModuleGeneratorModulePrivate
{
public:
  qSlicerMatlabModuleGeneratorModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerMatlabModuleGeneratorModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerMatlabModuleGeneratorModulePrivate
::qSlicerMatlabModuleGeneratorModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerMatlabModuleGeneratorModule methods

//-----------------------------------------------------------------------------
qSlicerMatlabModuleGeneratorModule
::qSlicerMatlabModuleGeneratorModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerMatlabModuleGeneratorModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerMatlabModuleGeneratorModule::~qSlicerMatlabModuleGeneratorModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerMatlabModuleGeneratorModule::helpText()const
{
  return "The MatlabModuleGenerator module is used for generating a skeleton Matlab module that the user can further customize to implement the intended functionality. The module also allows the user to specify the location of the Matlab executable.";
}

//-----------------------------------------------------------------------------
QString qSlicerMatlabModuleGeneratorModule::acknowledgementText()const
{
  return "This work is part of the SparKit project, funded by An Applied Cancer Research Unit of Cancer Care Ontario with funds provided by the Ministry of Health and Long-Term Care and the Ontario Consortium for Adaptive Interventions in Radiation Oncology (OCAIRO) to provide free, open-source toolset for radiotherapy and related image-guided interventions.";
}

//-----------------------------------------------------------------------------
QStringList qSlicerMatlabModuleGeneratorModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Andras Lasso (PerkLab, Queen's University)");
  moduleContributors << QString("Jean-Christophe Fillion-Robin (Kitware)");
  moduleContributors << QString("Kevin Wang (Radiation Medicine Program, Princess Margaret Hospital, University Health Network Toronto)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerMatlabModuleGeneratorModule::icon()const
{
  return QIcon(":/Icons/MatlabModuleGenerator.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerMatlabModuleGeneratorModule::categories() const
{
  return QStringList() << "Developer Tools.Matlab";
}

//-----------------------------------------------------------------------------
QStringList qSlicerMatlabModuleGeneratorModule::dependencies() const
{
  return QStringList();
}

QString qSlicerMatlabModuleGeneratorModule::getMatlabExecutablePath() const
{
  // 1. Slicer.ini application settings
  // If the Matlab path is defined in the Slicer settings then use that
  QSettings settings;
  QString matlabExecutablePath = settings.value("Matlab/MatlabExecutablePath","").toString(); 
  if (!matlabExecutablePath.isEmpty())
  {
    return matlabExecutablePath;
  }

  // 2. SLICER_MATLAB_EXECUTABLE_PATH environment variable
  // If the Matlab path is defined in an environment variable then use that
  const char* matlabExePathEnvValue=getenv("SLICER_MATLAB_EXECUTABLE_PATH");
  if (matlabExePathEnvValue && strlen(matlabExePathEnvValue)>0)
  {
    return QString(matlabExePathEnvValue);
  }

  // 3. App Paths
#if defined( _WIN32 ) && !defined(__CYGWIN__)
  QSettings matlabExePathRegistryKey("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\matlab.exe",QSettings::NativeFormat);
  QString matlabExePathRegistryValue=matlabExePathRegistryKey.value(".", NULL).toString(); // "." reads the (Default) value
  if (!matlabExePathRegistryValue.isEmpty())
  {
    return matlabExePathRegistryValue;
  }
#else
  std::vector<std::string> hints;
  std::string foundMatlabPath = vtksys::SystemTools::FindProgram("matlab", hints, false); // false: search system path
  if (!foundMatlabPath.empty())
  {
    return QString(foundMatlabPath.c_str());
  }
#endif

  // 4. Hardcoded default string
  return QString(DEFAULT_MATLAB_PROCESS_PATH.c_str());
}

//-----------------------------------------------------------------------------
void qSlicerMatlabModuleGeneratorModule::setup()
{
  this->Superclass::setup();

  qSlicerApplication * app = qSlicerApplication::application();
  if (!app)
  {
    qCritical() << "Fail to initalize qSlicerMatlabModuleGeneratorModule module: application is invalid";
    return;
  }
  vtkSlicerMatlabModuleGeneratorLogic* moduleGeneratorLogic= vtkSlicerMatlabModuleGeneratorLogic::SafeDownCast(this->logic());
  if (moduleGeneratorLogic == 0)
  {
    qCritical() << "Fail to initalize qSlicerMatlabModuleGeneratorModule module: logic is invalid";
    return;
  }

  //Uncomment this to allow some time to attach the debugger
  //vtksys::SystemTools::Delay(10000);

  // Get Matlab executable path and store in the logic
  QString matlabExecutablePath = getMatlabExecutablePath();
  moduleGeneratorLogic->SetMatlabExecutablePath(matlabExecutablePath.toLatin1());

  // Append Matlab module path to the additional paths
  QStringList additionalPaths = app->revisionUserSettings()->value("Modules/AdditionalPaths").toStringList();
  QString pathToAppend=moduleGeneratorLogic->GetMatlabModuleDirectory();
  QDir dirToCheck(pathToAppend);
  bool isPathToAppendAlreadyInAdditionalPaths=false;
  foreach(const QString& path, additionalPaths)
  {
    if (dirToCheck == QDir(path))
    {
      isPathToAppendAlreadyInAdditionalPaths=true;
      break;
    }
  }
  if (!isPathToAppendAlreadyInAdditionalPaths)
  {
    additionalPaths << pathToAppend;
    app->revisionUserSettings()->setValue("Modules/AdditionalPaths",additionalPaths); 
  }  

  // Set Matlab executable and commandserver script paths in environment variables
  // for MatlabCommander CLI module
  // Ideally, the app->setEnvironmentVariable method should be used, but somehow app->setEnvironmentVariable 
  // causes failure in the sample data volume (and probably in other modules, too, as Python cannot find the "os" symbol)    
  std::string matlabEnvVar=std::string("SLICER_MATLAB_EXECUTABLE_PATH=")+std::string(matlabExecutablePath.toLatin1());
  vtksys::SystemTools::PutEnv(matlabEnvVar.c_str());
  std::string scriptEnvVar=std::string("SLICER_MATLAB_COMMAND_SERVER_SCRIPT_PATH=")+moduleGeneratorLogic->GetMatlabCommandServerDirectory()+"/"+MATLAB_COMMAND_SERVER_SCRIPT_NAME;
  vtksys::SystemTools::PutEnv(scriptEnvVar.c_str());
  std::string commanderEnvVar=std::string("SLICER_MATLAB_COMMANDER_PATH=")+moduleGeneratorLogic->GetMatlabCommanderPath();
  vtksys::SystemTools::PutEnv(commanderEnvVar.c_str());
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerMatlabModuleGeneratorModule
::createWidgetRepresentation()
{
  return new qSlicerMatlabModuleGeneratorModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerMatlabModuleGeneratorModule::createLogic()
{
  return vtkSlicerMatlabModuleGeneratorLogic::New();
}
