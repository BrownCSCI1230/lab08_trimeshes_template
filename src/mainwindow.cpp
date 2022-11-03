#include "mainwindow.h"
#include "Settings.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QLabel>
#include <QGroupBox>
#include <iostream>

void MainWindow::setupUI()
{
    // Create glWidget for OpenGL stuff
    glWidget = new GLWidget;
    glWidget->initializeShapesAndParameters();
    glWidget->setEnabled(true); // specifically allow handling of keyboard and mouse events

    /* Set up the layouts (parent-child diagram):
     *
     *                 hLayout
     *                 /     \
     *           vLayout      glWidget (canvas)
     *          |      \
     * labels,buttons   \
     *                   \
     *             p1Layout,p2Layout (sliders/spinboxes)
     *
     */
    QHBoxLayout *hLayout = new QHBoxLayout(); // horizontal alignment
    QVBoxLayout *vLayout = new QVBoxLayout(); // vertical alignment
    vLayout->setAlignment(Qt::AlignTop);
    hLayout->addLayout(vLayout);
    hLayout->addWidget(glWidget, 1);
    this->setLayout(hLayout);

    // Create labels in sidebox
    QFont font;
    font.setPointSize(16);
    font.setBold(true);
    QLabel *trimesh_label = new QLabel(); // Trimeshes label
    trimesh_label->setText("Trimeshes");
    trimesh_label->setFont(font);
    QLabel *params_label = new QLabel(); // Parameters label
    params_label->setText("Parameters");
    params_label->setFont(font);
    QLabel *param1_label = new QLabel(); // Parameter 1 label
    param1_label->setText("Parameter 1:");
    QLabel *param2_label = new QLabel(); // Parameter 2 label
    param2_label->setText("Parameter 2:");
    QLabel *width_spacer = new QLabel(); // cheat-y way of adding more width to the QVBoxLayout
    width_spacer->setText("                                               ");
    width_spacer->setFont(font);

    // Create button controls to toggle 3D shapes
    triangleCB = new QRadioButton(); // Triangle button
    triangleCB->setText(QStringLiteral("Triangle"));
    triangleCB->setChecked(true); // Default Triangle toggled

    cubeCB = new QRadioButton(); // Cube button
    cubeCB->setText(QStringLiteral("Cube"));

    sphereCB = new QRadioButton(); // Sphere button
    sphereCB->setText(QStringLiteral("Sphere"));

    cylinderCB = new QRadioButton(); // Cylinder button
    cylinderCB->setText(QStringLiteral("Cylinder"));

    coneCB = new QRadioButton(); // Cone button
    coneCB->setText(QStringLiteral("Cone"));

    // Create toggle for showing wireframe / normals
    showWireframeNormals = new QCheckBox();
    showWireframeNormals->setText(QStringLiteral("Show Wireframe and Normals"));
    showWireframeNormals->setChecked(true);

    // Creates the boxes containing the parameter sliders and number boxes
    QGroupBox *p1Layout = new QGroupBox(); // horizonal slider 1 alignment
    QHBoxLayout *l1 = new QHBoxLayout();
    QGroupBox *p2Layout = new QGroupBox(); // horizonal slider 2 alignment
    QHBoxLayout *l2 = new QHBoxLayout();

    // Create slider controls to control parameters
    p1Slider = new QSlider(Qt::Orientation::Horizontal); // Parameter 1 slider
    p1Slider->setTickInterval(1);
    p1Slider->setMinimum(1);
    p1Slider->setMaximum(50);
    p1Slider->setValue(1);

    p1Box = new QSpinBox();
    p1Box->setMinimum(1);
    p1Box->setMaximum(50);
    p1Box->setSingleStep(1);
    p1Box->setValue(1);

    p2Slider = new QSlider(Qt::Orientation::Horizontal); // Parameter 2 slider
    p2Slider->setTickInterval(1);
    p2Slider->setMinimum(1);
    p2Slider->setMaximum(50);
    p2Slider->setValue(1);

    p2Box = new QSpinBox();
    p2Box->setMinimum(1);
    p2Box->setMaximum(50);
    p2Box->setSingleStep(1);
    p2Box->setValue(1);

    // Adds the slider and number box to the parameter layouts
    l1->addWidget(p1Slider);
    l1->addWidget(p1Box);
    p1Layout->setLayout(l1);

    l2->addWidget(p2Slider);
    l2->addWidget(p2Box);
    p2Layout->setLayout(l2);

    // Add the labels and checkbox widgets to vLayout for vertical alignment (order matters!)
    vLayout->addWidget(trimesh_label);
    vLayout->addWidget(triangleCB);
    vLayout->addWidget(cubeCB);
    vLayout->addWidget(sphereCB);
    vLayout->addWidget(cylinderCB);
    vLayout->addWidget(coneCB);
    vLayout->addWidget(width_spacer);
    vLayout->addWidget(params_label);
    vLayout->addWidget(param1_label);
    vLayout->addWidget(p1Layout);
    vLayout->addWidget(param2_label);
    vLayout->addWidget(p2Layout);
    vLayout->addWidget(showWireframeNormals);

    // Connects the sliders and number boxes for the parameters
    connectParam1();
    connectParam2();

    // Connects the toggles for the shapes
    connectTriangle();
    connectCube();
    connectSphere();
    connectCone();
    connectCylinder();

    // Connects the toggle for showing wireframe / normals
    connectWireframeNormals();
}

//******************************** Handles Parameter 1 UI Changes ********************************//
void MainWindow::connectParam1()
{
    connect(p1Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP1);
    connect(p1Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeP1);
}

void MainWindow::onValChangeP1(int newValue)
{
    p1Slider->setValue(newValue);
    p1Box->setValue(newValue);
    settings.shapeParameter1 = p1Slider->value();
    glWidget->settingsChange();
}

//******************************** Handles Parameter 2 UI Changes ********************************//
void MainWindow::connectParam2()
{
    connect(p2Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP2);
    connect(p2Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeP2);
}

void MainWindow::onValChangeP2(int newValue)
{
    p2Slider->setValue(newValue);
    p2Box->setValue(newValue);
    settings.shapeParameter2 = p2Slider->value();
    glWidget->settingsChange();
}

//******************************** Handles Shape Type UI Changes ********************************//
// triangle
void MainWindow::connectTriangle()
{
    connect(triangleCB, &QRadioButton::clicked, this, &MainWindow::onTriChange);
}

void MainWindow::onTriChange()
{
    settings.shapeType = SHAPE_TRIANGLE;
    p1Slider->setMinimum(1);
    p2Slider->setMinimum(1);
    p1Slider->setValue(1);
    p2Slider->setValue(1);
    glWidget->settingsChange();
}

// cube
void MainWindow::connectCube()
{
    connect(cubeCB, &QRadioButton::clicked, this, &MainWindow::onCubeChange);
}

void MainWindow::onCubeChange()
{
    settings.shapeType = SHAPE_CUBE;
    p1Slider->setMinimum(1);
    p2Slider->setMinimum(1);
    p1Slider->setValue(1);
    p2Slider->setValue(1);
    glWidget->settingsChange();
}

// sphere
void MainWindow::connectSphere()
{
    connect(sphereCB, &QRadioButton::clicked, this, &MainWindow::onSphereChange);
}

void MainWindow::onSphereChange()
{
    settings.shapeType = SHAPE_SPHERE;
    p1Slider->setMinimum(2);
    p2Slider->setMinimum(3);
    p1Slider->setValue(2);
    p2Slider->setValue(3);
    glWidget->settingsChange();
}

// cylinder
void MainWindow::connectCylinder()
{
    connect(cylinderCB, &QRadioButton::clicked, this, &MainWindow::onCylinderChange);
}

void MainWindow::onCylinderChange()
{
    settings.shapeType = SHAPE_CYLINDER;
    p1Slider->setMinimum(1);
    p2Slider->setMinimum(3);
    p1Slider->setValue(1);
    p2Slider->setValue(3);
    glWidget->settingsChange();
}

// cone
void MainWindow::connectCone()
{
    connect(coneCB, &QRadioButton::clicked, this, &MainWindow::onConeChange);
}

void MainWindow::onConeChange()
{
    settings.shapeType = SHAPE_CONE;
    p1Slider->setMinimum(1);
    p2Slider->setMinimum(3);
    p1Slider->setValue(1);
    p2Slider->setValue(3);
    glWidget->settingsChange();
}

//***************************** Handles Wireframe/Normals UI Changes *****************************//
void MainWindow::connectWireframeNormals()
{
    connect(showWireframeNormals, &QCheckBox::clicked, this, &MainWindow::onWireframeNormalsChange);
}

void MainWindow::onWireframeNormalsChange()
{
    settings.showWireframeNormals = !settings.showWireframeNormals;
    glWidget->settingsChange();
}

MainWindow::~MainWindow()
{
    delete(glWidget);
    delete(p1Slider);
    delete(p1Box);
    delete(p2Box);
    delete(triangleCB);
    delete(cubeCB);
    delete(sphereCB);
    delete(cylinderCB);
    delete(coneCB);
    delete(showWireframeNormals);
}
