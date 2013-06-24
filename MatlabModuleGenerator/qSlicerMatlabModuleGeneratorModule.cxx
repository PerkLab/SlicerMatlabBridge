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

const std::string MATLAB_COMMAND_SERVER_SCRIPT_NAME="commandserver.m";

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
  return "This is a loadable module bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerMatlabModuleGeneratorModule::acknowledgementText()const
{
  return "This work was was partially funded by NIH grant 3P41RR013218-12S1";
}

//-----------------------------------------------------------------------------
QStringList qSlicerMatlabModuleGeneratorModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Jean-Christophe Fillion-Robin (Kitware)");
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
  return QStringList() << "Matlab";
}

//-----------------------------------------------------------------------------
QStringList qSlicerMatlabModuleGeneratorModule::dependencies() const
{
  return QStringList();
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

  // Get Matlab executable path from application settings and store in the logic
  QSettings settings;
  QString matlabExecutablePath = settings.value("Matlab/MatlabExecutablePath",QString("Matlab.exe")).toString(); 
  moduleGeneratorLogic->SetMatlabExecutablePath(matlabExecutablePath.toLatin1());

  // Set Matlab executable and commandserver script paths in environment variables
  // for MatlabCommander CLI module
  // Ideally, the app->setEnvironmentVariable method should be used, but somehow app->setEnvironmentVariable 
  // causes failure in the sample data volume (and probably in other modules, too, as Python cannot find the "os" symbol)    
  std::string matlabEnvVar=std::string("SLICER_MATLAB_EXECUTABLE_PATH=")+std::string(matlabExecutablePath.toLatin1());
  vtksys::SystemTools::PutEnv(matlabEnvVar.c_str());
  std::string scriptEnvVar=std::string("SLICER_MATLAB_COMMAND_SERVER_SCRIPT_PATH=")+moduleGeneratorLogic->GetMatlabScriptDirectory()+"/"+MATLAB_COMMAND_SERVER_SCRIPT_NAME;
  vtksys::SystemTools::PutEnv(scriptEnvVar.c_str());
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
