#include "mogglwidget.h"
#include "mog/Constants.h"
#include "mog/core/Engine.h"
#include <QGLFormat>
#include <QtOpenGL>

MogGLWidget::MogGLWidget(QWidget* parent, const QGLWidget* shareWidget, Qt::WindowFlags f) :
    QGLWidget(parent, shareWidget, f) {
    QGLFormat glFormat;
    glFormat.setVersion(1, 1);
    this->setFormat(glFormat);
}

void MogGLWidget::initializeGL() {
    this->timer = new QTimer(this);
    QObject::connect(this->timer, &QTimer::timeout, this, &MogGLWidget::drawFrame);
    this->timer->start((int)(1.0f / this->fps * 1000.0f));

    this->engineController = new MogEngineController();
    this->engineController->startEngine();
}

void MogGLWidget::resizeGL(int width, int height) {
    this->engineController->resize(width, height);
}

void MogGLWidget::paintGL() {
    if (!this->engineStarted) {
        this->engineController->startEngine();
        this->engineStarted = false;
    }
    this->makeCurrent();
    this->engineController->drawFrame();
}

void MogGLWidget::drawFrame() {
    this->updateGL();
}
