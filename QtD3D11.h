#ifndef QTD3D11_H
#define QTD3D11_H

#include <QMainWindow>
#include "ui_QtD3D11.h"

class QtD3D11 : public QMainWindow
{
	Q_OBJECT

public:
	QtD3D11(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~QtD3D11();

private slots:

  void on_actionExit_activated();
  void on_actionOpen_activated();
  void on_actionLightSetting_activated();
  void on_actionTabClose_activated(int index);

  void on_actionRedSpinBox_ValueChanged(double fVal);
  void on_actionGreenSpinBox_ValueChanged(double fVal);
  void on_actionBlueSpinBox_ValueChanged(double fVal);
  
private:
	Ui::QtD3D11Class ui;
};

#endif // QTD3D11_H
