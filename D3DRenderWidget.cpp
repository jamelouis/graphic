
#define NOMINMAX

#include "D3DRenderWidget.h"
#include <QMouseEvent>
#include <QTimer>




D3DRenderWidget::D3DRenderWidget(QWidget* parent, const std::string& filename)
  : QWidget(parent) 
{
  d3dRender_.createDevice(width(),height(), (HWND)winId());
  d3dRender_.setSimpleCamera(&simpleCamera_);
  d3dRender_.Init(filename);

  setAttribute(Qt::WA_PaintOnScreen, true);
  setAttribute(Qt::WA_NativeWindow, true);
  

  QTimer* timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(update()));
  timer->start(16);
}

D3DRenderWidget::~D3DRenderWidget() 
{
  d3dRender_.Unit();
}

void D3DRenderWidget::resizeEvent(QResizeEvent* evt) 
{
  d3dRender_.resize(width(), height());
}

void D3DRenderWidget::paintEvent(QPaintEvent* evt) 
{
  d3dRender_.render();
}



void D3DRenderWidget::wheelEvent(QWheelEvent* evt) 
{
  float delta      = (float)evt->delta();
  simpleCamera_.cameraDistance_ -= (delta / 120.0f / 4.0f) * 10.0;
  simpleCamera_.cameraDistance_  = std::max(1.0f, simpleCamera_.cameraDistance_);
  update();
}

void D3DRenderWidget::mousePressEvent(QMouseEvent* evt) 
{
  if(evt->button() == Qt::LeftButton) {
    mousePrev_      = evt->pos();
  } else {
    evt->ignore();
  }
}

void D3DRenderWidget::mouseReleaseEvent(QMouseEvent* evt) 
{
  evt->ignore();
}

void D3DRenderWidget::mouseMoveEvent(QMouseEvent* evt)
{
  if(evt->buttons() & Qt::LeftButton) {
    QPointF curr  = evt->pos();
    QPointF delta = curr - mousePrev_;
    mousePrev_    = curr;

    simpleCamera_.cameraAngleY_ -= (delta.x()) / 200.0f;
    simpleCamera_.cameraAngleX_ += (delta.y()) / 200.0f;

    simpleCamera_.cameraAngleX_ = std::min(1.57f, std::max(-1.57f, simpleCamera_.cameraAngleX_));
  
    update();
  } else {
    evt->ignore();
  }
}

void D3DRenderWidget::setLightDirX(double x)
{
    d3dRender_.cpuCommonBuffer.light.vLightDir.x = x;
}

void D3DRenderWidget::setLightDirY(double y)
{
    d3dRender_.cpuCommonBuffer.light.vLightDir.y = y;
}

void D3DRenderWidget::setLightDirZ(double z)
{
    d3dRender_.cpuCommonBuffer.light.vLightDir.z = z;
}