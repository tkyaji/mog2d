#ifndef MOGGLWIDGET_H
#define MOGGLWIDGET_H

#include <QGLWidget>
#include <QTimer>
#include "mog/os/mogenginecontroller.h"

class MogGLWidget : public QGLWidget
{
public:
    explicit MogGLWidget(QWidget* parent=Q_NULLPTR,
                         const QGLWidget* shareWidget = Q_NULLPTR, Qt::WindowFlags f=Qt::WindowFlags());

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

private:
    QTimer *timer;
    void drawFrame();
    float fps = 60.0f;
    bool engineStarted = false;

    MogEngineController *engineController;
};

#endif // MOGGLWIDGET_H
