#include "Cylinder.h"
#include "glm/gtc/constants.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"

void Cylinder::updateParams(int param1, int param2)
{
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}


void Cylinder::setVertexData() {
    // [TODO] -- Realtime 1 Project

}

/**
 * Inserts a glm::vec3 into a vector of floats.
 * This will come in handy if you want to take advantage of vectors to build your shape!
 */
void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v){
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
