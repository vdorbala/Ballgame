#include "objects.h"
#include "main.h"

Trampoline::Trampoline(float x, float y, color_t color) {
    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;
    
    GLfloat vertex_buffer_data[45] ={ 
        0.9f, 0.0f, 0.0f,
        0.9f, 1.1f, 0.0f,
        0.7f, 1.0f, 0.0f,

        0.9f, 0.0f, 0.0f,
        0.7f, 1.1f, 0.0f,
        0.7f, 0.0f, 0.0f,

        0.7f, 1.0f, 0.0f,
       -0.7f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,

       -0.7f, 0.0f, 0.0f,
       -0.9f, 1.1f, 0.0f,
       -0.7f, 1.0f, 0.0f,

       -0.7f, 0.0f, 0.0f,
       -0.9f, 1.1f, 0.0f,
       -0.9f, 0.0f, 0.0f

    }; 

    this->object = create3DObject(GL_TRIANGLES, 15, vertex_buffer_data, color, GL_FILL);
}

void Trampoline::draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);
    glm::mat4 rotate    = glm::rotate((float) (this->rotation * M_PI / 180.0f), glm::vec3(0, 0, 1));
    rotate          = glm::translate(glm::vec3(0, 0, 0)) * rotate * glm::translate(glm::vec3(0, 0, 0));
    Matrices.model *= (translate * rotate);
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);
}

void Trampoline::set_position(float x, float y) {
    this->position = glm::vec3(x, y, 0);
}

bool Trampoline::detect_collision(Ball ball) {
    return std::fabs(this->position.x - ball.position.x) < (ball.radius + 0.7f) && std::fabs(this->position.y - ball.position.y) < (ball.radius);
}

bounding_box_t Trampoline::bounding_box() {
    float x = this->position.x, y = this->position.y;
    bounding_box_t bbox = { x, y, 1.7f, 1.1f };
    return bbox;
}