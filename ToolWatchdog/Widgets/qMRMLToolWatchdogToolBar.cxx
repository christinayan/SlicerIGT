/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Julien Finet, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// Qt includes
#include <QMenu>
#include <QInputDialog>
#include <QToolButton>
#include <QLabel>
#include <QList>

// qMRML includes
#include "qMRMLToolWatchdogToolBar.h"
#include "qMRMLSceneViewMenu.h"
#include "qMRMLNodeFactory.h"

// MRML includes
#include <vtkMRMLViewNode.h>
#include <vtkMRMLSceneViewNode.h>
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkWeakPointer.h>

//-----------------------------------------------------------------------------
class qMRMLToolWatchdogToolBarPrivate
{
  QVTK_OBJECT
  Q_DECLARE_PUBLIC(qMRMLToolWatchdogToolBar);
protected:
  qMRMLToolWatchdogToolBar* const q_ptr;
public:
  qMRMLToolWatchdogToolBarPrivate(qMRMLToolWatchdogToolBar& object);
  void init();
  void setMRMLScene(vtkMRMLScene* newScene);
  QList<QAction*>*                         ActionsListPtr;
  //QAction*                         SceneViewAction;
  //qMRMLSceneViewMenu*              SceneViewMenu;

  // TODO In LayoutManager, use GetActive/IsActive flag ...
  vtkWeakPointer<vtkMRMLViewNode>  ActiveMRMLThreeDViewNode;
  vtkSmartPointer<vtkMRMLScene>    MRMLScene;

public slots:
  void OnMRMLSceneStartBatchProcessing();
  void OnMRMLSceneEndBatchProcessing();
  void updateWidgetFromMRML();
  void createSceneView();
};

//--------------------------------------------------------------------------
// qMRMLToolWatchdogToolBarPrivate methods

//---------------------------------------------------------------------------
qMRMLToolWatchdogToolBarPrivate::qMRMLToolWatchdogToolBarPrivate(qMRMLToolWatchdogToolBar& object)
  : q_ptr(&object)
{
  this->ActionsListPtr = NULL;
  //this->SceneViewAction = 0;
  //this->SceneViewMenu = 0;
}

// --------------------------------------------------------------------------
void qMRMLToolWatchdogToolBarPrivate::updateWidgetFromMRML()
{
  Q_Q(qMRMLToolWatchdogToolBar);
  // Enable buttons
  q->setEnabled(this->MRMLScene != 0);
  //this->LabelsList->setEnabled(this->ActiveMRMLThreeDViewNode != 0);
}

//---------------------------------------------------------------------------
void qMRMLToolWatchdogToolBarPrivate::init()
{
  Q_Q(qMRMLToolWatchdogToolBar);

  // Screenshot button
  if(this->ActionsListPtr == NULL)
  {
    this->ActionsListPtr = new QList<QAction*>;
    QLabel* transformLabel = new QLabel(q);
    transformLabel->setToolTip(q->tr("Each square indicates the state of the tools watched by the ToolWatchdog module"));
    transformLabel->setText("Tools watched:");
    //this->LabelsList->setIcon(QIcon(":/Icons/ToolWatchdog.png"));
    this->ActionsListPtr->push_back(q->addWidget(transformLabel));
    //QObject::connect(this->LabelsListPtr, SIGNAL(triggered()),
    //                 q, SIGNAL(screenshotButtonClicked()));
    
  }
  //// Scene View buttons
  //this->SceneViewAction = new QAction(q);
  //this->SceneViewAction->setIcon(QIcon(":/Icons/ViewCamera.png"));
  //this->SceneViewAction->setText(q->tr("Scene view"));
  //this->SceneViewAction->setToolTip(q->tr("ToolWatchdog and name a scene view."));
  //QObject::connect(this->SceneViewAction, SIGNAL(triggered()),
  //                 q, SIGNAL(sceneViewButtonClicked()));
  //q->addAction(this->SceneViewAction);

  //// Scene view menu
  //QToolButton* sceneViewMenuButton = new QToolButton(q);
  //sceneViewMenuButton->setText(q->tr("Restore view"));
  //sceneViewMenuButton->setIcon(QIcon(":/Icons/ViewCameraSelect.png"));
  //sceneViewMenuButton->setToolTip(QObject::tr("Restore or delete saved scene views."));
  //this->SceneViewMenu = new qMRMLSceneViewMenu(sceneViewMenuButton);
  //sceneViewMenuButton->setMenu(this->SceneViewMenu);
  //sceneViewMenuButton->setPopupMode(QToolButton::InstantPopup);
  ////QObject::connect(q, SIGNAL(mrmlSceneChanged(vtkMRMLScene*)),
  ////                 this->SceneViewMenu, SLOT(setMRMLScene(vtkMRMLScene*)));
  //q->addWidget(sceneViewMenuButton);
  //QObject::connect(q, SIGNAL(toolButtonStyleChanged(Qt::ToolButtonStyle)),
  //                sceneViewMenuButton,
  //                SLOT(setToolButtonStyle(Qt::ToolButtonStyle)));
}
// --------------------------------------------------------------------------
void qMRMLToolWatchdogToolBarPrivate::setMRMLScene(vtkMRMLScene* newScene)
{
  if (newScene == this->MRMLScene)
    {
    return;
    }
/*
  this->qvtkReconnect(this->MRMLScene, newScene, vtkMRMLScene::StartBatchProcessEvent,
                      this, SLOT(OnMRMLSceneStartBatchProcessing()));

  this->qvtkReconnect(this->MRMLScene, newScene, vtkMRMLScene::EndBatchProcessEvent,
                      this, SLOT(OnMRMLSceneEndBatchProcessing()));

*/
  this->MRMLScene = newScene;

  //this->SceneViewMenu->setMRMLScene(newScene);

  // Update UI
  this->updateWidgetFromMRML();
}


// --------------------------------------------------------------------------
void qMRMLToolWatchdogToolBarPrivate::OnMRMLSceneStartBatchProcessing()
{
  Q_Q(qMRMLToolWatchdogToolBar);
  q->setEnabled(false);
}

// --------------------------------------------------------------------------
void qMRMLToolWatchdogToolBarPrivate::OnMRMLSceneEndBatchProcessing()
{
  this->updateWidgetFromMRML();
}

// --------------------------------------------------------------------------
void qMRMLToolWatchdogToolBarPrivate::createSceneView()
{
  Q_Q(qMRMLToolWatchdogToolBar);

  // Ask user for a name
  bool ok = false;
  QString sceneViewName = QInputDialog::getText(q, QObject::tr("SceneView Name"),
                                                QObject::tr("SceneView Name:"), QLineEdit::Normal,
                                                "View", &ok);
  if (!ok || sceneViewName.isEmpty())
    {
    return;
    }

  // Create scene view
  qMRMLNodeFactory nodeFactory;
  nodeFactory.setMRMLScene(this->MRMLScene);
  nodeFactory.setBaseName("vtkMRMLSceneViewNode", sceneViewName);
  vtkMRMLNode * newNode = nodeFactory.createNode("vtkMRMLSceneViewNode");
  vtkMRMLSceneViewNode * newSceneViewNode = vtkMRMLSceneViewNode::SafeDownCast(newNode);
  newSceneViewNode->StoreScene();
}

// --------------------------------------------------------------------------
// qMRMLToolWatchdogToolBar methods

// --------------------------------------------------------------------------
qMRMLToolWatchdogToolBar::qMRMLToolWatchdogToolBar(const QString& title, QWidget* parentWidget)
  :Superclass(title, parentWidget)
   , d_ptr(new qMRMLToolWatchdogToolBarPrivate(*this))
{
  Q_D(qMRMLToolWatchdogToolBar);
  d->init();
}

// --------------------------------------------------------------------------
qMRMLToolWatchdogToolBar::qMRMLToolWatchdogToolBar(QWidget* _parent)
  : Superclass(_parent)
  , d_ptr(new qMRMLToolWatchdogToolBarPrivate(*this))
{
  Q_D(qMRMLToolWatchdogToolBar);
  d->init();
}

void qMRMLToolWatchdogToolBar
::SetFirstlabel(char * watchDogNodeName)
{
  Q_D(qMRMLToolWatchdogToolBar);
  QString tooltip("Each square indicates the state of the tools watched by the ToolWatchdog module: ");
  tooltip.append(watchDogNodeName);
  QLabel* firstLabel = (QLabel*)this->widgetForAction(d->ActionsListPtr->at(0));
  firstLabel->setToolTip( tooltip );
  firstLabel->setText(QString(watchDogNodeName).mid(4,11));
}

void qMRMLToolWatchdogToolBar
::ToolNodeAdded(char * label)
{
  Q_D(qMRMLToolWatchdogToolBar);

  QLabel* toolLabel = new QLabel(this);
  toolLabel->setToolTip(this->tr("Tool in row %1").arg(d->ActionsListPtr->size()));
  toolLabel->setText(QString(label).left(6));

  toolLabel->setAlignment(Qt::AlignCenter);
  toolLabel->setStyleSheet("QLabel { background-color: green; min-width: 2em; max-height: 2em;}");
  d->ActionsListPtr->push_back(this->addWidget(toolLabel));
  //QObject::connect(this->LabelsListPtr, SIGNAL(triggered()),
  //                 this, SIGNAL(screenshotButtonClicked()));
  
}
void qMRMLToolWatchdogToolBar
::SwapToolNodes(int toolA, int toolB )
{
  Q_D(qMRMLToolWatchdogToolBar);

  //d->ActionsListPtr->swap(toolA+1,toolB+1);
  QLabel* toolLabelA = (QLabel*)this->widgetForAction(d->ActionsListPtr->at(toolA+1));
  QLabel* toolLabelB = (QLabel*)this->widgetForAction(d->ActionsListPtr->at(toolB+1));
  QString TempLabel = toolLabelA->text();
  toolLabelA->setText(toolLabelB->text());
  toolLabelB->setText(TempLabel);
  //this->widgetForAction(d->ActionsListPtr->at(toolA+1))->setIconText(this->widgetForAction(d->ActionsListPtr->at(toolB+1))->iconText());
  //this->widgetForAction(d->ActionsListPtr->at(toolB+1))->SetIconText(toolLabel);

}




void qMRMLToolWatchdogToolBar
::ToolNodeDeleted()
{
  Q_D(qMRMLToolWatchdogToolBar);
  this->removeAction(d->ActionsListPtr->back());
  d->ActionsListPtr->pop_back();
}


void qMRMLToolWatchdogToolBar
::DeleteToolNode(int row)
{
  Q_D(qMRMLToolWatchdogToolBar);
  this->removeAction(d->ActionsListPtr->at(row+1));
  d->ActionsListPtr->removeAt(row+1);
}



void qMRMLToolWatchdogToolBar
::SetNodeStatus(int row, bool status )
{
  Q_D(qMRMLToolWatchdogToolBar);
  if(d->ActionsListPtr!= NULL)
  {
    if(d->ActionsListPtr->size()>1&&row+1<d->ActionsListPtr->size())
    {
      if(status)
      {
        this->widgetForAction(d->ActionsListPtr->at(row+1))->setStyleSheet("QLabel { background-color: rgb(45,224,90); min-width: 2em; max-height: 2em;}");
      }
      else
      {
        this->widgetForAction(d->ActionsListPtr->at(row+1))->setStyleSheet("QLabel { background-color: red; min-width: 2em; max-height: 2em;}");
      }
    }
  }
  return;
  
  //QObject::connect(this->LabelsListPtr, SIGNAL(triggered()),
  //                 this, SIGNAL(screenshotButtonClicked()));
  //this->addWidget(d->LabelsListPtr->back());
}

void qMRMLToolWatchdogToolBar
::SetNodeLabel(int row,const char * toolLabel)
{
  Q_D(qMRMLToolWatchdogToolBar);
  if(d->ActionsListPtr!= NULL)
  {
    if(d->ActionsListPtr->size()>1&&row+1<d->ActionsListPtr->size())
    {
      QLabel* label= (QLabel*)(this->widgetForAction(d->ActionsListPtr->at(row+1)));
      label->setText(QString(toolLabel));
    }
  }
  return;
}


//---------------------------------------------------------------------------
qMRMLToolWatchdogToolBar::~qMRMLToolWatchdogToolBar()
{
}

// --------------------------------------------------------------------------
void qMRMLToolWatchdogToolBar::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qMRMLToolWatchdogToolBar);
  d->setMRMLScene(scene);
}

// --------------------------------------------------------------------------
void qMRMLToolWatchdogToolBar::setActiveMRMLThreeDViewNode(
  vtkMRMLViewNode * newActiveMRMLThreeDViewNode)
{
  Q_D(qMRMLToolWatchdogToolBar);
  d->ActiveMRMLThreeDViewNode = newActiveMRMLThreeDViewNode;
  d->updateWidgetFromMRML();
}
