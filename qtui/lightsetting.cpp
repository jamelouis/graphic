#include "lightsetting.h"
#include "ui_lightsetting.h"

LightSetting::LightSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LightSetting)
{
    ui->setupUi(this);

    ui->RedSpinBox->setRange(-1.0, 1.0);
    ui->RedSpinBox->setSingleStep(0.1);
    ui->GreenSpinBox->setRange(-1.0, 1.0);
    ui->GreenSpinBox->setSingleStep(0.1);
    ui->BlueSpinBox->setRange(-1.0, 1.0);
    ui->BlueSpinBox->setSingleStep(0.1);

    
    connect(ui->RedSpinBox, SIGNAL(valueChanged(double)), parent, SLOT(on_actionRedSpinBox_ValueChanged(double)));
    connect(ui->GreenSpinBox, SIGNAL(valueChanged(double)), parent, SLOT(on_actionGreenSpinBox_ValueChanged(double)));
    connect(ui->BlueSpinBox, SIGNAL(valueChanged(double)), parent, SLOT(on_actionBlueSpinBox_ValueChanged(double)));
}

LightSetting::~LightSetting()
{
    delete ui;
}
