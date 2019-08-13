#include "main.h"
#include "ball.h"

#ifndef OBJECT_H
#define OBJECT_H

class Trampoline {
public:
    Trampoline() {}
    Trampoline(float x, float y, color_t color);
    glm::vec3 position;
    float rotation;
    void draw(glm::mat4 VP);
    void set_position(float x, float y);
    bool detect_collision(Ball ball);
    bounding_box_t bounding_box();
private:
    VAO *object;
};

#endif
