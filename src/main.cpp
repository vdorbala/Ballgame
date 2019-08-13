#include "main.h"
#include "timer.h"
#include "ball.h"
#include "ground.h"
#include "objects.h"

using namespace std;

GLMatrices Matrices;
GLuint     programID;
GLFWwindow *window;

int main_score;
const int ENEMY_COUNT = 40;
int up_flag = 0;
float viscous = 1;
int dead_balls[ENEMY_COUNT];
int dead_enemy_count = 0;

Badball badball;
Ball balls[ENEMY_COUNT];
Ground ground1, ground2;
Plank enemy_plank[ENEMY_COUNT/2];
Trampoline trampoline;

bounding_box_t screen_box;

float screen_zoom = 1, screen_center_x = 0, screen_center_y = 1.0;
double mouse_x = 0.0, mouse_y=0.0;
int mouse_action = 0;

int level = 0;

Timer t60(1.0 / 60);

void draw() {

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram (programID);

	// Eye - Location of camera. 
	// glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
	// Target - Where is the camera looking at.
	// glm::vec3 target (0, 0, 0);
	// Up - Up vector defines tilt of camera.
	// glm::vec3 up (0, 1, 0);

	// Compute Camera matrix (view)
	// Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D

	Matrices.view = glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); // Fixed camera for 2D (ortho) in XY plane

	// Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)

	glm::mat4 VP = Matrices.projection * Matrices.view;

	// Send our transformation to the currently bound shader, in the "MVP" uniform
	// For each model you render, since the MVP will be different (at least the M part)
	glm::mat4 MVP;  // MVP = Projection * View * Model

	// Scene render the order is important for overwriting the objects

	for(int i=0;i<ENEMY_COUNT;i++)
		if(dead_balls[i]==0)	
			{
				balls[i].draw(VP);
				if(i%2 == 0)
					enemy_plank[i/2].draw(VP);
			}

	ground1.draw(VP);
	ground2.draw(VP);

	trampoline.draw(VP);

	badball.draw(VP);


}

void tick_input(GLFWwindow *window) {
	int left  = glfwGetKey(window, GLFW_KEY_LEFT);
	int right = glfwGetKey(window, GLFW_KEY_RIGHT);
	int up    = glfwGetKey(window, GLFW_KEY_UP);
	int down    = glfwGetKey(window, GLFW_KEY_DOWN);
	int a 	= glfwGetKey(window, GLFW_KEY_A);
	int s 	= glfwGetKey(window, GLFW_KEY_S);
	int d 	= glfwGetKey(window, GLFW_KEY_D);
	int w 	= glfwGetKey(window, GLFW_KEY_W);

	if(a){
		screen_center_x -= 0.01;
		reset_screen(2);
	}
	
	if(s){
		screen_center_y -= 0.01;
		reset_screen(2);
	}

	if(d){
		screen_center_x += 0.01;
		reset_screen(2);
	}

if(w){
		screen_center_y += 0.01;
		reset_screen(2);
	}


	if (left) {
		badball.speed += 0.002;
	}
	else if (right) {
		badball.speed -= 0.002;
	}
	else {
		if (badball.speed < 0) {
			badball.speed += 0.001;
		}
		else if (badball.speed > 0) {
			badball.speed -= 0.001;
		}
		else {
			badball.speed = 0;
		}
	}

	if (up == GLFW_PRESS and !up_flag) {
		up_flag = 1;
		badball.yspeed = 0.25f;
	}
}

bool tick_elements() {
	badball.tick(viscous);
	for(int i=0;i<ENEMY_COUNT;i++)
		{
			if(dead_balls[i]==0)
			{
				balls[i].tick();

				if(i%2 == 0) 
					{
						enemy_plank[i/2].tick();										
						if (enemy_plank[i/2].detect_collision(badball))
						{	
							main_score+=100;   
							float m  = tan((enemy_plank[i/2].rotation + 90)*M_PI/180.0);
							badball.speed = badball.speed - enemy_plank[i/2].speed;	
							badball.speed = (badball.speed*(1 - m*m) + 2*m*badball.yspeed)/(1 + m*m);
							badball.speed = badball.speed + enemy_plank[i/2].speed;
							badball.yspeed =(badball.yspeed*(m*m - 1) + 2*m*badball.speed)/(1 + m*m);             

						}
					}

				if (detect_collision(badball.bounding_box(), balls[i].bounding_box()))
					{
						dead_balls[i] = 1;
						dead_enemy_count++;
						main_score+=100;
						if(badball.position.y > balls[i].position.y)
							badball.yspeed += 0.15f;                
					}
	
				if(!detect_collision(balls[i].bounding_box(),screen_box) )
					{
						balls[i].position.x = -balls[i].position.x;
						if(i%2 == 0)
							{
								enemy_plank[i/2].position.x = balls[i].position.x + balls[i].radius*cos(enemy_plank[i/2].rotation*M_PI/180.0);						
								enemy_plank[i/2].position.y = balls[i].position.y + balls[i].radius*sin(enemy_plank[i/2].rotation*M_PI/180.0);						
								
							}

					}


			}

		}


	if (detect_collision(badball.bounding_box(),screen_box))


	if(trampoline.detect_collision(badball))
		{
			
			badball.yspeed  =0.27f;	
			badball.position.y =1.0f;
		}

	if(!detect_collision(badball.bounding_box(),screen_box))	
		badball.position.x = -badball.position.x;


	if(ground1.detect_collision(badball) || ground2.detect_collision(badball))
	{
		badball.yspeed = -0.5*badball.yspeed;
		badball.position.y = 0;
		
		up_flag = 0;

	}

	return false;    
}

void initGL(GLFWwindow *window, int width, int height) {
	main_score = 0;

	badball       = Badball (5,5,0.2,0.0, COLOR_BLACK);

	for(int i=0;i<ENEMY_COUNT;i++){
		color_t BALL_COLOR = {rand()%265, rand()%265, rand()%265};
		balls[i] = Ball(16*(rand()%1000)/1000.0 -8, 3*(rand()%1000)/1000.0 + 1, 0.2 + (rand()%100)/500.0 ,0.01 + (rand()%1000)/100000.0,BALL_COLOR);
	}

	float plank_x,plank_y,angle;

	for(int i=0;i<ENEMY_COUNT/2;i++)
		{
			angle = rand()%90;
			plank_x = balls[i*2].position.x + balls[i*2].radius*cos(angle*M_PI/180.0);
			plank_y = balls[i*2].position.y + balls[i*2].radius*sin(angle*M_PI/180.0);

			enemy_plank[i] = Plank(plank_x,plank_y,0.1,2*balls[i*2].radius,angle,balls[i*2].speed,COLOR_BROWN);
		}


	ground1       = Ground(6,-1.0,8,1.4,0.0,0.0,COLOR_GREEN);
	ground2       = Ground(-6,-1.0,8,1.4,0.0,0.0,COLOR_GREEN);
	trampoline 	  = Trampoline(6,-0.25,COLOR_BLACK);
 

	programID = LoadShaders("Sample_GL.vert", "Sample_GL.frag");
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


	reshapeWindow (window, width, height);

	// Background color of the scene
	glClearColor (COLOR_BACKGROUND.r / 256.0, COLOR_BACKGROUND.g / 256.0, COLOR_BACKGROUND.b / 256.0, 0.0f); // R, G, B, A
	glClearDepth (1.0f);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

	cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
	cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
	cout << "VERSION: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}


int main(int argc, char **argv) {
	srand(time(0));
	int width  = 1600;
	int height = 800;

	window = initGLFW(width, height);

	initGL (window, width, height);

    screen_box = { screen_center_x, screen_center_y, 7.0f,10.0f};



	/* Draw in loop */
	for(int i=0;!glfwWindowShouldClose(window) && dead_enemy_count < ENEMY_COUNT;i++) {

		if (t60.processTick()) {

			draw();
			glfwSwapBuffers(window);

			tick_elements();

			tick_input(window);

			reset_screen(2);

			level = main_score/1000;
		}

		if( i%100000 == 0){

			std::string  str_score = "##FLYBALL##";
    		const char *dis_score = str_score.c_str();
			glfwSetWindowTitle(window,dis_score);
					
		}

	}



	quit(window);
}

bool detect_collision(bounding_box_t a, bounding_box_t b) {
	// checking that the center of the circle is less than their sum of radius
	// (width or height) = radius of the circle
	return ( ( ( a.x- b.x )*( a.x- b.x ) + ( a.y- b.y )*( a.y- b.y ) ) < ( a.width + b.width ) * ( a.height + b.height ) );    
}


// https://stackoverflow.com/questions/3267243/in-opengl-how-can-i-adjust-for-the-window-being-resized

void reset_screen(float ratio) {
	float top    = screen_center_y + 4 / screen_zoom;
	float bottom = screen_center_y - 4 / screen_zoom;
	float left   = screen_center_x - 4 / screen_zoom;
	float right  = screen_center_x + 4 / screen_zoom;
	Matrices.projection = glm::ortho(left*ratio, right*ratio, bottom, top, 0.1f, 500.0f);
}
