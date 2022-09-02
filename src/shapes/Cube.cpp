#include "Cube.h"
#include <iostream>

void Cube::updateParams(int param1)
{
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    setVertexData();
}

void Cube::makeTile(glm::vec3 topLeft, glm::vec3 bottomLeft,
                    glm::vec3 bottomRight, glm::vec3 topRight)
{
    // Task 2: create a tile (aka 2 triangles) out of the 4
    //         given points (once you have the tile, make sure you add
    //         the points to m_vertexData)
    // Note: pay attention to how you are calculating your normals! you will
    //       be using this function to create a cube


}

void Cube::makeFace(glm::vec3 topLeft, glm::vec3 bottomLeft,
                    glm::vec3 bottomRight, glm::vec3 topRight)
{
    // Task 3: create a single side of the cube out of the 4
    //         given points and makeTile()
    // Note: think about how param 1 affects the number of triangles on
    //       the face of the cube


}

void Cube::setVertexData()
{
    // Uncomment this for Task 2 and recomment out for Task 3:
//    makeTile(glm::vec3(-0.5f, 0.5f, 0.5f),
//             glm::vec3(-0.5f, -0.5f, 0.5f),
//             glm::vec3(0.5f, -0.5f, 0.5f),
//             glm::vec3(0.5f, 0.5f, 0.5f));

    // Uncomment this for Task 3:
//    // Positive z
//    makeFace(glm::vec3(-0.5f, 0.5f, 0.5f),
//             glm::vec3(-0.5f, -0.5f, 0.5f),
//             glm::vec3(0.5f, -0.5f, 0.5f),
//             glm::vec3(0.5f, 0.5f, 0.5f));



    // Task 4: Use the makeFace() function to make all 6 sides of the cube


}

/**
 * Inserts a glm::vec3 into a vector of floats.
 * This will come in handy if you want to take advantage of vectors to build your shape!
 */
void Cube::insertVec3(std::vector<float> &data, glm::vec3 v){
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
