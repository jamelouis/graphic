#include "QtD3D11.h"
#include "D3DRenderWidget.h"

QtD3D11::QtD3D11(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

  D3DRenderWidget* widget = new D3DRenderWidget(this);
  setCentralWidget(widget);
}

QtD3D11::~QtD3D11()
{
}

void QtD3D11::on_actionExit_activated() {
  close();
}

