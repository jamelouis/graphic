
#ifndef D3D_RENDER_WIDGET_HPP_INC
#define D3D_RENDER_WIDGET_HPP_INC 1

#include <QWidget>
#include "SimpleCamera.h"
#include "D3DRender.h"


class D3DRenderWidget : public QWidget {
  Q_OBJECT
  Q_DISABLE_COPY(D3DRenderWidget)

public:

  D3DRenderWidget(QWidget* parent, const std::string& filename);
  virtual ~D3DRenderWidget();

  virtual QPaintEngine* paintEngine() const { return NULL; }

  void setLightDirX(double x);
  void setLightDirY(double y);
  void setLightDirZ(double z);

protected:

  virtual void resizeEvent(QResizeEvent* evt);
  virtual void paintEvent(QPaintEvent* evt);
  virtual void wheelEvent(QWheelEvent* evt);
  virtual void mousePressEvent(QMouseEvent* evt);
  virtual void mouseReleaseEvent(QMouseEvent* evt);
  virtual void mouseMoveEvent(QMouseEvent* evt);

private:

    QPointF         mousePrev_;
    D3DRender       d3dRender_;
    SimpleCamera    simpleCamera_;
    
};

#endif
