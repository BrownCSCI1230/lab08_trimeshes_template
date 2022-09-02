#pragma once

#include <QMainWindow>
#include <QSlider>
#include <QSpinBox>
#include <QRadioButton>
#include <QCheckBox>

#include "glwidget.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    void setupUI();
    ~MainWindow();

private:
    GLWidget *glWidget;

    QSlider *p1Slider;
    QSlider *p2Slider;
    QSpinBox *p1Box;
    QSpinBox *p2Box;
    QCheckBox *showWireframeNormals;

    QRadioButton *triangleCB;
    QRadioButton *cubeCB;
    QRadioButton *sphereCB;
    QRadioButton *cylinderCB;
    QRadioButton *coneCB;

    void connectParam1();
    void connectParam2();;
    void connectWireframeNormals();

    void connectTriangle();
    void connectCube();
    void connectSphere();
    void connectCylinder();
    void connectCone();

private slots:
    void onValChangeP1(int newValue);
    void onValChangeP2(int newValue);
    void onWireframeNormalsChange();

    void onTriChange();
    void onCubeChange();
    void onSphereChange();
    void onCylinderChange();
    void onConeChange();
};
