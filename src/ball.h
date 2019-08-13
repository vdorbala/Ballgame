#include "main.h"

#ifndef BALL_H
#define BALL_H


class Ball {
public:
    Ball() {}
    Ball(float x, float y,float radius,float speed, color_t color);
    glm::vec3 position;
    float rotation;
    void draw(glm::mat4 VP);
    void set_position(float x, float y);
    void tick();
    float radius;
    double speed;
    double yspeed;
    bounding_box_t bounding_box();
private:
    VAO *object;    // How an object looks 
};

class Badball :public Ball{
public:
    Badball(): Ball() {}
    Badball(float x, float y, float radius, float speed, color_t color) : Ball( x,  y, radius,speed, color){};
    void tick(float viscous); 
};
#endif