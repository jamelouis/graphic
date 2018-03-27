#ifndef LIGHTSETTING_H
#define LIGHTSETTING_H

#include <QDialog>

namespace Ui {
class LightSetting;
}

class LightSetting : public QDialog
{
    Q_OBJECT

public:
    explicit LightSetting(QWidget *parent = 0);
    ~LightSetting();

private:
    Ui::LightSetting *ui;
};

#endif // LIGHTSETTING_H
