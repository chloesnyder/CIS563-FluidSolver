#include "container.hpp"

container::container() {

}

void container::create() {
    glm::vec3 vert_col[8];
    glm::vec3 vert_pos[8];
    GLuint indices[24];

    vert_pos[0] = glm::vec3(minX, minY, minZ);
    vert_pos[1] = glm::vec3(minX, maxY, minZ);
    vert_pos[2] = glm::vec3(minX, maxY, maxZ);
    vert_pos[3] = glm::vec3(minX, minY, maxZ);
    vert_pos[4] = glm::vec3(maxX, minY, minZ);
    vert_pos[5] = glm::vec3(maxX, maxY, minZ);
    vert_pos[6] = glm::vec3(maxX, maxY, maxZ);
    vert_pos[7] = glm::vec3(maxX, minY, maxZ);
//    vert_pos[0] = glm::vec3(-2);
//    vert_pos[1] = glm::vec3(-2, 2,-2);
//    vert_pos[2] = glm::vec3(-2, 2, 2);
//    vert_pos[3] = glm::vec3(-2, -2, 2);
//    vert_pos[4] = glm::vec3(2, -2, -2);
//    vert_pos[5] = glm::vec3(2, 2, -2);
//    vert_pos[6] = glm::vec3(2, 2, 2);
//    vert_pos[7] = glm::vec3(2, -2, 2);

    for(int i = 0; i < 8; i++) {
        vert_col[i] = glm::vec3(1,1,1);
    }

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 1;
    indices[3] = 2;
    indices[4] = 2;
    indices[5] = 3;
    indices[6] = 3;
    indices[7] = 0;

    indices[8] = 4;
    indices[9] = 5;
    indices[10] = 5;
    indices[11] = 6;
    indices[12] = 6;
    indices[13] = 7;
    indices[14] = 7;
    indices[15] = 4;

    indices[16] = 0;
    indices[17] = 4;
    indices[18] = 1;
    indices[19] = 5;
    indices[20] = 2;
    indices[21] = 6;
    indices[22] = 3;
    indices[23] = 7;

    glGenBuffers(1, &(vertexbuffer));
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(vert_pos), vert_pos, GL_STATIC_DRAW);

    glGenBuffers(1, &(colorbuffer));
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(vert_col), vert_col, GL_STATIC_DRAW);

    glGenBuffers(1, &(indexbuffer));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(GLuint), indices, GL_STATIC_DRAW);
}

void container::destroy() {
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &colorbuffer);
    glDeleteBuffers(1, &indexbuffer);
}

