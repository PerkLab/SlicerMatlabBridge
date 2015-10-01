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
#include <QDesktopServices>
#include <QSettings> 
#include <QUrl>

// VTK includes
#include <vtkStdString.h>

// SlicerQt includes
#include "qSlicerMatlabModuleGeneratorModuleWidget.h"
#include "ui_qSlicerMatlabModuleGeneratorModuleWidget.h"
#include "qSlicerApplication.h"
#include "qSlicerModuleManager.h"
#include "qSlicerAbstractCoreModule.h"
#include <vtkSlicerCLIModuleLogic.h>

// Module includes
#include "vtkSlicerMatlabModuleGeneratorLogic.h"


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerMatlabModuleGeneratorModuleWidgetPrivate: public Ui_qSlicerMatlabModuleGeneratorModuleWidget
{
  Q_DECLARE_PUBLIC(qSlicerMatlabModuleGeneratorModuleWidget);
protected:
  qSlicerMatlabModuleGeneratorModuleWidget* const q_ptr;
  vtkStdString lastInterfaceDefinitionFilename;
  vtkStdString lastMatlabFunctionFilename;
public:
  qSlicerMatlabModuleGeneratorModuleWidgetPrivate(qSlicerMatlabModuleGeneratorModuleWidget& object);
  vtkSlicerMatlabModuleGeneratorLogic* logic() const;
};

//-----------------------------------------------------------------------------
// qSlicerMatlabModuleGeneratorModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerMatlabModuleGeneratorModuleWidgetPrivate::qSlicerMatlabModuleGeneratorModuleWidgetPrivate(qSlicerMatlabModuleGeneratorModuleWidget& object)
: q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
vtkSlicerMatlabModuleGeneratorLogic*
qSlicerMatlabModuleGeneratorModuleWidgetPrivate::logic() const
{
  Q_Q(const qSlicerMatlabModuleGeneratorModuleWidget);
  return vtkSlicerMatlabModuleGeneratorLogic::SafeDownCast(q->logic());
} 

//-----------------------------------------------------------------------------
// qSlicerMatlabModuleGeneratorModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerMatlabModuleGeneratorModuleWidget::qSlicerMatlabModuleGeneratorModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerMatlabModuleGeneratorModuleWidgetPrivate(*this) )
{
}

//-----------------------------------------------------------------------------
qSlicerMatlabModuleGeneratorModuleWidget::~qSlicerMatlabModuleGeneratorModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerMatlabModuleGeneratorModuleWidget::setup()
{
  Q_D(qSlicerMatlabModuleGeneratorModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  // Determine the value of Matlab/ExitMatlab in QSettings
  // and set checkbox_ExitMatlab accordingly
  QSettings settings;
  if (settings.contains("Matlab/ExitMatlabOnApplicationExit"))
  {
    bool exitMatlab = settings.value("Matlab/ExitMatlabOnApplicationExit").toBool();
    d->checkBox_ExitMatlab->setChecked(exitMatlab);
  }
  else
  {
    d->checkBox_ExitMatlab->setChecked(true);
    settings.setValue("Matlab/ExitMatlabOnApplicationExit",true);
  }

  connect( QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(applicationAboutToQuit()) );

  connect( d->lineEdit_MatlabExecutablePath, SIGNAL(currentPathChanged(QString)), this, SLOT(matlabExecutablePathChanged(QString)) );
  connect( d->checkBox_ExitMatlab, SIGNAL(stateChanged(int)), this, SLOT(exitMatlabChanged(int)) );

  connect( d->pushButton_GenerateModule, SIGNAL(clicked()), this, SLOT(generateModuleClicked()) );

  connect( d->pushButton_EditInterfaceDefinition, SIGNAL(clicked()), this, SLOT(editInterfaceDefinitionClicked()) );
  connect( d->pushButton_EditMatlabFunction, SIGNAL(clicked()), this, SLOT(editMatlabFunctionClicked()) );
  connect( d->pushButton_RestartApplication, SIGNAL(clicked()), this, SLOT(restartApplicationClicked()) );

  d->lineEdit_MatlabExecutablePath->setCurrentPath(d->logic()->GetMatlabExecutablePath());
  d->lineEdit_MatlabScriptDirectory->setText(d->logic()->GetMatlabModuleDirectory());
}

//-----------------------------------------------------------------------------
void qSlicerMatlabModuleGeneratorModuleWidget::applicationAboutToQuit()
{
  bool exitMatlab = true;
  QSettings settings;
  if (settings.contains("Matlab/ExitMatlabOnApplicationExit"))
  {
    exitMatlab = settings.value("Matlab/ExitMatlabOnApplicationExit").toBool();
  }
  if (exitMatlab)
  {
    qDebug("Shutting down Matlab");
    qSlicerModuleManager* moduleManager = qSlicerApplication::application()->moduleManager();
    qSlicerAbstractCoreModule* matlabCommanderModule = moduleManager->module("MatlabCommander");
    if (matlabCommanderModule)
    {
      vtkSlicerCLIModuleLogic* matlabCommanderLogic = vtkSlicerCLIModuleLogic::SafeDownCast(matlabCommanderModule->logic());
      vtkMRMLCommandLineModuleNode* cmdNode = matlabCommanderLogic->CreateNodeInScene();
      cmdNode->SetParameterAsBool("exitmatlab", true);
      matlabCommanderLogic->ApplyAndWait(cmdNode);
    }
    else
    {
      qWarning("MatlabCommander module is not found");
    }
  }
}

//-----------------------------------------------------------------------------
void qSlicerMatlabModuleGeneratorModuleWidget::matlabExecutablePathChanged(QString path)
{
  Q_D(qSlicerMatlabModuleGeneratorModuleWidget);

  QSettings settings;
  settings.setValue("Matlab/MatlabExecutablePath",path);

  d->logic()->SetMatlabExecutablePath(path.toLatin1());
}

//-----------------------------------------------------------------------------
void qSlicerMatlabModuleGeneratorModuleWidget::exitMatlabChanged(int state)
{
  Q_D(qSlicerMatlabModuleGeneratorModuleWidget);
  QSettings settings;
  settings.setValue("Matlab/ExitMatlabOnApplicationExit",state==Qt::Checked);
}

//-----------------------------------------------------------------------------
void qSlicerMatlabModuleGeneratorModuleWidget::generateModuleClicked()
{
  Q_D(qSlicerMatlabModuleGeneratorModuleWidget);
  QString moduleName=d->lineEdit_GeneratorModuleName->text();
  vtkStdString result=d->logic()->GenerateModule(moduleName.toLatin1(), d->lastInterfaceDefinitionFilename, d->lastMatlabFunctionFilename);
  d->textEdit_GeneratorResults->setText(QString(result));

  d->pushButton_EditInterfaceDefinition->setEnabled(!d->lastInterfaceDefinitionFilename.empty());
  d->pushButton_EditMatlabFunction->setEnabled(!d->lastMatlabFunctionFilename.empty());
}

//-----------------------------------------------------------------------------
void qSlicerMatlabModuleGeneratorModuleWidget::editInterfaceDefinitionClicked()
{
  Q_D(qSlicerMatlabModuleGeneratorModuleWidget);
  QString filename = QString::fromLatin1("file:///")+QString::fromLatin1(d->lastInterfaceDefinitionFilename);
  QDesktopServices::openUrl(QUrl(filename, QUrl::TolerantMode));
}

//-----------------------------------------------------------------------------
void qSlicerMatlabModuleGeneratorModuleWidget::editMatlabFunctionClicked()
{
  Q_D(qSlicerMatlabModuleGeneratorModuleWidget);
  QString filename = QString::fromLatin1("file:///")+QString::fromLatin1(d->lastMatlabFunctionFilename);
  QDesktopServices::openUrl(QUrl(filename, QUrl::TolerantMode));
}

//-----------------------------------------------------------------------------
void qSlicerMatlabModuleGeneratorModuleWidget::restartApplicationClicked()
{
  Q_D(qSlicerMatlabModuleGeneratorModuleWidget);
  qSlicerApplication::application()->confirmRestart();
}
