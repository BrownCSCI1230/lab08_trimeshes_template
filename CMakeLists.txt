cmake_minimum_required(VERSION 3.16)

# Sets project name
project(lab08 LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Sets C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specifies required Qt components
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS OpenGL)
find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
find_package(Qt6 REQUIRED COMPONENTS Gui)

# Allows you to include files from within those directories, without prefixing their filepaths
include_directories(src)

# Specifies .cpp and .h files to be passed to the compiler
add_executable(${PROJECT_NAME}
  src/main.cpp

  src/mainwindow.cpp
  src/Settings.cpp
  src/glwidget.cpp
  src/shapes/triangle.cpp
  src/shapes/cube.cpp
  src/shapes/sphere.cpp
  src/shapes/cylinder.cpp
  src/shapes/cone.cpp

  src/mainwindow.h
  src/settings.h
  src/glwidget.h
  src/shapes/triangle.h
  src/shapes/cube.h
  src/shapes/sphere.h
  src/shapes/cylinder.h
  src/shapes/cone.h
)

# Specifies libraries to be linked (Qt components, glew, etc)
target_link_libraries(${PROJECT_NAME} PRIVATE
  Qt::Core
  Qt::Widgets
  Qt::OpenGL
  Qt::OpenGLWidgets
  Qt::Gui
)

# Set this flag to silence warnings on Windows
if (MSVC OR MSYS OR MINGW)
  set(CMAKE_CXX_FLAGS "-Wno-volatile")
endif()

# Set this flag to silence warnings on MacOS
if (APPLE)
  set(CMAKE_CXX_FLAGS "-Wno-deprecated-volatile")
endif()
