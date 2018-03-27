#include "QtD3D11.h"
#include "D3DRenderWidget.h"
#include <QFileDialog>
#include "qtui/lightsetting.h"


QtD3D11::QtD3D11(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
{
    ui.setupUi(this);
    setCentralWidget(ui.tabWidget);

    // remove default tab and tab2 in tab widget.
    ui.tabWidget->removeTab(1);
    ui.tabWidget->removeTab(0);
    ui.tabWidget->setTabsClosable(true);

    connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(on_actionOpen_activated()));
    connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(on_actionExit_activated()));
    connect(ui.actionLightSetting, SIGNAL(triggered()), this, SLOT(on_actionLightSetting_activated()));
    connect(ui.tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(on_actionTabClose_activated(int)));
}

QtD3D11::~QtD3D11()
{
}

void QtD3D11::on_actionExit_activated() {
  close();
}

void QtD3D11::on_actionOpen_activated()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), tr("Obj Model(*.obj)"));

    std::string strFileName = fileName.toLocal8Bit().constData();
    auto start = strFileName.find_last_of("//")+1;
    auto offset = strFileName.rfind(".") - start;
    std::string strModelName = strFileName.substr(start, offset);

    D3DRenderWidget* widget = new D3DRenderWidget(ui.tabWidget, strFileName);
    ui.tabWidget->addTab(widget, strModelName.c_str());
    ui.tabWidget->setCurrentWidget(widget);
}

void QtD3D11::on_actionLightSetting_activated()
{
    LightSetting lightSettingDialog(this);
    lightSettingDialog.setModal(false);
    lightSettingDialog.show();
    lightSettingDialog.exec();
}

void QtD3D11::on_actionTabClose_activated(int index)
{
    ui.tabWidget->removeTab(index);
}

void QtD3D11::on_actionRedSpinBox_ValueChanged(double fVal)
{
   D3DRenderWidget* d3dWidget = (D3DRenderWidget*)ui.tabWidget->currentWidget();
   if(d3dWidget)
        d3dWidget->setLightDirX(fVal);

}

void QtD3D11::on_actionGreenSpinBox_ValueChanged(double fVal)
{
    D3DRenderWidget* d3dWidget = (D3DRenderWidget*)ui.tabWidget->currentWidget();
    if (d3dWidget)
        d3dWidget->setLightDirY(fVal);
}

void QtD3D11::on_actionBlueSpinBox_ValueChanged(double fVal)
{
    D3DRenderWidget* d3dWidget = (D3DRenderWidget*)ui.tabWidget->currentWidget();
    if (d3dWidget)
        d3dWidget->setLightDirZ(fVal);
}