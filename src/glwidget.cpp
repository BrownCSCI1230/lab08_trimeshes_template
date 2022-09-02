#include "glwidget.h"
#include "Settings.h"

#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>
#include <iostream>
#include "glm/gtx/transform.hpp"

/**
 * ==================================================
 *                  Shape Shaders
 * ==================================================
 */
static const char *vertexShaderSourceCore =
    "#version 330 core\n"
    "layout(location = 0) in vec4 vertex;\n"
    "layout(location = 1) in vec3 normal;\n"
    "out vec3 vert;\n"
    "out vec3 vertNormal;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "uniform mat3 normalMatrix;\n"
    "void main() {\n"
    "   vert = vec3(mvMatrix * vertex);;\n"
    "   vertNormal = normalMatrix * normal;\n"
    "   gl_Position = projMatrix * mvMatrix * vertex;\n"
    "}\n";
static const char *fragmentShaderSourceCore =
    "#version 330 core\n"
    "in vec3 vert;\n"
    "in vec3 vertNormal;\n"
    "out vec4 fragColor;\n"
    "uniform vec3 lightPos;\n"
    "void main() {\n"
    "   vec3 L = normalize(lightPos - vert);\n"
    "   float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
    "   vec3 color = vec3(1.0, 0.78, 0.0);\n"
    "   vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
    "   fragColor = vec4(col, 1.0);\n"
    "}\n";

/**
 * ==================================================
 *                  Wireframe Shaders
 * ==================================================
 */
static const char *wireframeVertexShaderSourceCore =
    "#version 330 core\n"
    "layout(location = 0) in vec4 vertex;\n"
    "layout(location = 1) in vec3 normal;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "uniform mat3 normalMatrix;\n"
    "void main() {\n"
    "   gl_Position = projMatrix * mvMatrix * vertex;\n"
    "}\n";
static const char *wireframeFragmentShaderSourceCore =
    "#version 330 core\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "   fragColor = vec4(vec3(0.0), 1.0);\n"
    "}\n";

/**
 * ==================================================
 *                  Normals Shaders
 * ==================================================
 */
/// ~~~ Arrow Body ~~~
static const char *normalsVertexShaderSourceCore =
    "#version 330 core\n"
    "layout(location = 0) in vec4 vertex;\n"
    "layout(location = 1) in vec3 normal;\n"
    "out vec4 vertNormal;\n"
    "void main() {\n"
    "   gl_Position = vertex;\n"
    "   vertNormal = vec4(normal, 0.0);\n"
    "}\n";
static const char *normalsGeometryShaderSourceCore =
    "#version 330 core\n"
    "layout(triangles) in;\n"
    "layout(line_strip, max_vertices = 6) out;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "in vec4 vertNormal[];\n"
    "const float NORMAL_LINE_LENGTH = 0.1;\n"
    "void main() {\n"
    "   int i;\n"
    "   for (i = 0; i < gl_in.length(); i++) {\n"
    "       vec4 P = projMatrix * mvMatrix * gl_in[i].gl_Position.xyzw;\n"
    "       vec4 N = projMatrix * mvMatrix * vertNormal[i];\n"
    "       gl_Position = P;\n"
    "       EmitVertex();\n"
    "       gl_Position = P + N * NORMAL_LINE_LENGTH;\n"
    "       EmitVertex();\n"
    "       EndPrimitive();\n"
    "   }\n"
    "}\n";
static const char *normalsFragmentShaderSourceCore =
    "#version 330 core\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "    fragColor = vec4(vec3(0.0), 1.0);\n"
    "}\n";

/// ~~~ Arrow Head ~~~
static const char *normalsTipVertexShaderSourceCore =
    "#version 330 core\n"
    "layout(location = 0) in vec4 vertex;\n"
    "layout(location = 1) in vec3 normal;\n"
    "out vec4 vertNormal;\n"
    "void main() {\n"
    "   gl_Position = vertex;\n"
    "   vertNormal = vec4(normal, 0.0);\n"
    "}\n";
static const char *normalsTipGeometryShaderSourceCore =
    "#version 330 core\n"
    "layout(triangles) in;\n"
    "layout(triangle_strip, max_vertices = 18) out;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "in vec4 vertNormal[];\n"
    "const float NORMAL_LINE_LENGTH = 0.1;\n"
    "const float NORMAL_ARROW_LENGTH = 0.05;\n"
    "const float NORMAL_ARROW_WIDTH = 0.03;\n"
    "void main() {\n"
    "   int i;\n"
    "   for (i = 0; i < gl_in.length(); i++) {\n"
    "       vec4 P = projMatrix * mvMatrix * gl_in[i].gl_Position.xyzw;\n"
    "       vec4 N = projMatrix * mvMatrix * vertNormal[i];\n"
    "       gl_Position = P + N * (NORMAL_LINE_LENGTH + NORMAL_ARROW_LENGTH);\n"
    "       EmitVertex();\n"
    "       vec4 eye = P + N * NORMAL_LINE_LENGTH;\n"
    "       vec4 axis = normalize(vec4(cross(eye.xyz, N.xyz), 0.0));\n"
    "       axis.z = 0.f;\n"
    "       gl_Position = P + N * NORMAL_LINE_LENGTH + axis * NORMAL_ARROW_WIDTH;\n"
    "       EmitVertex();\n"
    "       gl_Position = P + N * NORMAL_LINE_LENGTH - axis * NORMAL_ARROW_WIDTH;\n"
    "       EmitVertex();\n"
    "       EndPrimitive();\n"
    "   }\n"
    "}\n";
static const char *normalsTipFragmentShaderSourceCore =
    "#version 330 core\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "    fragColor = vec4(vec3(0.0), 1.0);\n"
    "}\n";

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(103/255.f, 142/255.f, 166/255.f, 1); // set the background color

    // Create Shapes shader program
    m_program = new QOpenGLShaderProgram; // allow OpenGL shader programs to be linked and used
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSourceCore);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSourceCore);
    m_program->link();
    m_program->bind();
    m_default_projLoc = m_program->uniformLocation("projMatrix");
    m_default_mvLoc = m_program->uniformLocation("mvMatrix");
    m_default_normalLoc = m_program->uniformLocation("normalMatrix");
    m_default_lightPos = m_program->uniformLocation("lightPos");
    m_program->setUniformValue(m_default_lightPos, QVector3D(70, 70, 70)); // light stuff
    m_program->release();

    // Create Wireframe shader program
    m_wireframeProgram = new QOpenGLShaderProgram;
    m_wireframeProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, wireframeVertexShaderSourceCore);
    m_wireframeProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, wireframeFragmentShaderSourceCore);
    m_wireframeProgram->link();
    m_wireframeProgram->bind();
    m_wireframe_projLoc = m_wireframeProgram->uniformLocation("projMatrix");
    m_wireframe_mvLoc = m_wireframeProgram->uniformLocation("mvMatrix");
    m_wireframeProgram->release();

    // Create Normals shader program
    m_normalsProgram = new QOpenGLShaderProgram; // arrow body
    m_normalsProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, normalsVertexShaderSourceCore);
    m_normalsProgram->addShaderFromSourceCode(QOpenGLShader::Geometry, normalsGeometryShaderSourceCore);
    m_normalsProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, normalsFragmentShaderSourceCore);
    m_normalsProgram->link();
    m_normalsProgram->bind();
    m_normal_projLoc = m_normalsProgram->uniformLocation("projMatrix");
    m_normal_mvLoc = m_normalsProgram->uniformLocation("mvMatrix");
    m_normalsProgram->release();

    m_normalsTipsProgram = new QOpenGLShaderProgram; // arrow head
    m_normalsTipsProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, normalsTipVertexShaderSourceCore);
    m_normalsTipsProgram->addShaderFromSourceCode(QOpenGLShader::Geometry, normalsTipGeometryShaderSourceCore);
    m_normalsTipsProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, normalsTipFragmentShaderSourceCore);
    m_normalsTipsProgram->link();
    m_normalsTipsProgram->bind();
    m_normalTip_projLoc = m_normalsTipsProgram->uniformLocation("projMatrix");
    m_normalTip_mvLoc = m_normalsTipsProgram->uniformLocation("mvMatrix");
    m_normalsTipsProgram->release();

    // VAO/VBO stuff
    m_vao.create();
    m_vao.bind();
    m_vbo.create();
    bindVbo();
    m_vao.release();

    // m_camera is the model-view matrix. The projection matrix is separately tracked as m_proj
    m_camera = glm::translate(m_camera, glm::vec3(0, 0, -4 + m_zoomZ)); // Camera stuff (facing -z direction)
}

void GLWidget::bindVbo()
{
    // Create the OpenGLShape and get its vertices and normals
    if (m_currShape == SHAPE_TRIANGLE) {
        verts = m_triangle->generateShape();
    } else if (m_currShape == SHAPE_CUBE) {
        verts = m_cube->generateShape();
    } else if (m_currShape == SHAPE_SPHERE) {
        verts = m_sphere->generateShape();
    } else if (m_currShape == SHAPE_CYLINDER) {
        verts = m_cylinder->generateShape();
    } else if (m_currShape == SHAPE_CONE) {
        verts = m_cone->generateShape();
    }

    m_numTriangles = int(verts.size()) / 6;

    m_vbo.bind();
    m_vbo.allocate(verts.data(), verts.size()*sizeof(GLfloat));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                             nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                             reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    m_vbo.release();
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    // Draw 3D shape
    m_program->bind();
    m_program->setUniformValue(m_default_projLoc, glmMatToQMat(m_proj));
    m_program->setUniformValue(m_default_mvLoc, glmMatToQMat(m_camera * m_world));
    QMatrix3x3 normalMatrix = glmMatToQMat(m_camera * m_world).normalMatrix();
    m_program->setUniformValue(m_default_normalLoc, normalMatrix);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, m_numTriangles);

    if (settings.showWireframeNormals) {
        // Draw normals
        m_normalsTipsProgram->bind(); // arrow head
        m_normalsTipsProgram->setUniformValue(m_normalTip_projLoc, glmMatToQMat(m_proj));
        m_normalsTipsProgram->setUniformValue(m_normalTip_mvLoc, glmMatToQMat(m_camera * m_world));
        glDrawArrays(GL_TRIANGLES, 0, m_numTriangles);

        m_normalsProgram->bind(); // arrow body
        m_normalsProgram->setUniformValue(m_normal_projLoc, glmMatToQMat(m_proj));
        m_normalsProgram->setUniformValue(m_normal_mvLoc, glmMatToQMat(m_camera * m_world));
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES, 0, m_numTriangles);

        // Draw wireframe
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(-1, -1);
        m_wireframeProgram->bind();
        m_wireframeProgram->setUniformValue(m_wireframe_projLoc, glmMatToQMat(m_proj));
        m_wireframeProgram->setUniformValue(m_normal_mvLoc, glmMatToQMat(m_camera * m_world));
        glDrawArrays(GL_TRIANGLES, 0, m_numTriangles);
        glPolygonOffset(0, 0);
    }
}

void GLWidget::resizeGL(int w, int h)
{
    m_proj = glm::perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
}

void GLWidget::initializeShapesAndParameters() {
    m_currShape = SHAPE_TRIANGLE;
    m_currParam1 = 1;
    m_currParam2 = 1;
    m_triangle = new Triangle();
    m_triangle->updateParams();
    m_cube = new Cube();
    m_cube->updateParams(m_currParam1);
    m_sphere = new Sphere();
    m_sphere->updateParams(m_currParam1, m_currParam2);
    m_cylinder = new Cylinder();
    m_cylinder->updateParams(m_currParam1, m_currParam2);
    m_cone = new Cone();
    m_cone->updateParams(m_currParam1, m_currParam2);
}

QMatrix4x4 GLWidget::glmMatToQMat(glm::mat4x4 m) {
    // Note: glm::mat4x4 is column-major order
    //       QMatrix4x4 is row-major order

    QMatrix4x4 qMat(m[0][0], m[1][0], m[2][0], m[3][0],
                    m[0][1], m[1][1], m[2][1], m[3][1],
                    m[0][2], m[1][2], m[2][2], m[3][2],
                    m[0][3], m[1][3], m[2][3], m[3][3]);
    return qMat;
}

/* -----------------------------------------------
 *   Mouse Events for Orbital Camera stuff below
 * -----------------------------------------------
*/
void GLWidget::mousePressEvent(QMouseEvent *event) {
    m_oldXY = glm::vec2(event->position().y(), event->position().x());
    m_isDragging = true;
    update();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    if (m_isDragging) {
        glm::vec2 currPos(event->position().y(), event->position().x());
        m_angleXY.x += 10 * (currPos.x - m_oldXY.x) / (float) width();
        m_angleXY.y += 10 * (currPos.y - m_oldXY.y) / (float) height();
        m_oldXY = currPos;
        if (m_angleXY[0] < -90) m_angleXY[0] = -90;
        if (m_angleXY[0] > 90) m_angleXY[0] = 90;
        updateView();
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (m_isDragging) {
        m_isDragging = false;
        update();
    }
}

void GLWidget::wheelEvent(QWheelEvent *event) {
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->pixelDelta() / 8;
    if (!numPixels.isNull()) {
        m_zoomZ *= powf(0.999f, -numPixels.y());
    } else {
        m_zoomZ *= powf(0.99f, -numDegrees.y() / 15);
    }
    updateView();
}

void GLWidget::updateView() {
    m_camera =
            glm::translate(glm::vec3(0.0, 0.0, -4.0 + m_zoomZ)) *
            glm::rotate(m_angleXY[1], glm::vec3(0.0, 1.0, 0.0)) *
            glm::rotate(m_angleXY[0], glm::vec3(1.0, 0.0, 0.0));
    update();
}

/* -----------------------------------------------
 *   Settings Change
 * -----------------------------------------------
*/
void GLWidget::settingsChange()
{
    // show wireframe / normals
    if (settings.showWireframeNormals != m_currShowWireframeNormals) {
        m_currShowWireframeNormals = settings.showWireframeNormals;
        update();
        return;
    }

    // if shape settings change
    if (settings.shapeType != m_currShape) {
        if (settings.shapeType == SHAPE_TRIANGLE) {
            m_currShape = SHAPE_TRIANGLE;
        } else if (settings.shapeType == SHAPE_CUBE) {
            m_currShape = SHAPE_CUBE;
        } else if (settings.shapeType == SHAPE_SPHERE) {
            m_currShape = SHAPE_SPHERE;
        } else if (settings.shapeType == SHAPE_CYLINDER) {
            m_currShape = SHAPE_CYLINDER;
        } else if (settings.shapeType == SHAPE_CONE) {
            m_currShape = SHAPE_CONE;
        }
    }

    // parameter settings
    if (settings.shapeParameter1 != m_currParam1 || settings.shapeParameter2 != m_currParam2) {
        m_currParam1 = settings.shapeParameter1;
        m_currParam2 = settings.shapeParameter2;

        if (settings.shapeType == SHAPE_TRIANGLE) {
            m_triangle->updateParams();
        } else if (settings.shapeType == SHAPE_CUBE) {
            m_cube->updateParams(settings.shapeParameter1);
        } else if (settings.shapeType == SHAPE_SPHERE) {
            m_sphere->updateParams(settings.shapeParameter1, settings.shapeParameter2);
        } else if (settings.shapeType == SHAPE_CYLINDER) {
            m_cylinder->updateParams(settings.shapeParameter1, settings.shapeParameter2);
        } else if (settings.shapeType == SHAPE_CONE) {
            m_cone->updateParams(settings.shapeParameter1, settings.shapeParameter2);
        }
    }

    bindVbo();
    update();
}

GLWidget::~GLWidget()
{
    delete m_sphere;
    delete m_cone;
    delete m_cube;
    delete m_cylinder;
    delete m_triangle;
    if (m_program == nullptr) {
        return;
    } else {
        makeCurrent();
        m_vbo.destroy();
        delete m_program;
        m_program = nullptr;
        doneCurrent();
    }
    if (m_normalsProgram == nullptr) {
        return;
    } else {
        makeCurrent();
        delete m_normalsProgram;
        m_normalsProgram = nullptr;
        doneCurrent();
    }
    if (m_wireframeProgram == nullptr) {
        return;
    } else {
        makeCurrent();
        delete m_wireframeProgram;
        m_wireframeProgram = nullptr;
        doneCurrent();
    }
}
