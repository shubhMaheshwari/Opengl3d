#include "main.h"
#include "timer.h"
#include "sea.h"
#include "boat.h"
#include "objects.h"
#include "monster.h"

using namespace std;

GLMatrices Matrices;
GLuint     programID;
GLFWwindow *window;

/**************************
* Customizable functions *
**************************/
#define num_barrels 10
#define num_rocks 40

Monster monsters;
Boat boat;
Sea sea;
vector<Sphere> sphere;
vector<Cube> barrels;
Cube rocks[num_rocks];


int boat_health = 100,score = 0;
float screen_zoom = 2.0, screen_center_x = 0, screen_center_y = 0, screen_center_z;
float eye_x,eye_y,eye_z;
float target_x,target_y,target_z;
float camera_rotation_angle = 95.0;
// Check which camera view is present
bool camera_follower = true,camera_top_view=false;
bool sphere_hold = false, boost_use = false;

// Wind Oscillations
float wind = 0;

Timer t60(1.0 / 60);

/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw() {


    // All Camera Positions defined here.
    if (camera_follower == true){
        target_x = boat.position.x;
        target_y = boat.position.y;
        target_z = boat.position.z;
        camera_rotation_angle = -boat.rotation + 95;


        eye_x = target_x + 10*cos(camera_rotation_angle*M_PI/180.0f);
        eye_y = target_y + 10;
        eye_z = target_z + 10*sin(camera_rotation_angle*M_PI/180.0f);

    }


	// clear the color and depth in the frame buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use the loaded shader program
    // Don't change unless you know what you are doing
	glUseProgram (programID);

    // Eye - Location of camera. Don't change unless you are sure!!
    glm::vec3 eye (eye_x,eye_y,eye_z);
    // Target - Where is the camera looking at.  Don't change unless you are sure!!
    glm::vec3 target (target_x, target_y, target_z);
    // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
    glm::vec3 up (0, 1, 0);

	// Compute Camera matrix (view)
	Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
	glm::mat4 VP = Matrices.projection * Matrices.view;

    // Don't change unless you are sure!!
	glm::mat4 MVP;  // MVP = Projection * View * Model

	// Scene render
	sea.draw(VP);
	boat.draw(VP);
    monsters.draw(VP);

	vector<Sphere> :: iterator s;
	for (s = sphere.begin(); s < sphere.end(); ++s)
		s->draw(VP);

	vector<Cube> :: iterator b;
	for (b = barrels.begin(); b < barrels.end(); ++b)
		b->draw(VP);

	for (int i = 0; i < num_rocks; ++i)
		rocks[i].draw(VP);

	
}

void tick_input(GLFWwindow *window) {


	// Boat movement 
	int a   = glfwGetKey(window, GLFW_KEY_A);
	int s   = glfwGetKey(window, GLFW_KEY_S);
	int d   = glfwGetKey(window, GLFW_KEY_D);
    int w   = glfwGetKey(window, GLFW_KEY_W);

	if(w){
		boat.speed -= 0.005;
	}

	else if(s){
		boat.speed += 0.005;
	}

	else {
		if (boat.speed < 0) {
			boat.speed += 0.002;
		}
		else if (boat.speed > 0) {
			boat.speed -= 0.002;
		}
		else {
			boat.speed = 0;
		}
	}




	if(a){
		boat.rotation +=1;
	}

	if(d){
		boat.rotation -=1;
	}




}

void tick_elements() {


    monsters.tick(boat.position.x, boat.position.z);

	boat.tick(wind);
	sea.tick();

	// Move each ball and erase them if they have reached the bottom
	vector<Sphere> :: iterator s;
	for (s = sphere.begin(); s < sphere.end(); ++s)
		if(s->tick())
			sphere.erase(s);

	sea.set_position(boat.position.x,boat.position.z);


}


void collision_function(){
    for (int i = 0; i < num_rocks; ++i)
    if(detect_collision(boat.bounding_box(),rocks[i].bounding_box()))
        {
            printf("Hit Rock Bottom\n");
            boat.speed = -0.5*boat.speed;
            boat_health -= 5;
        }


}
void initGL(GLFWwindow *window, int width, int height) {
	boat       = Boat(0, 0, COLOR_ORANGE);
	
    monsters    = Monster(COLOR_RED);

	sea        = Sea( 0, 0, COLOR_BLUE);

	// Barrels
	for(int i=0;i<num_barrels;i++)
		barrels.push_back(Cube(rand()%500 -250,0,rand()%500 -250,10,5,5,COLOR_BROWN));

	// Rocks
	for (int i = 0; i < num_rocks; ++i)
		rocks[i] = Cube(5*(rand()%200 -100),0,5*(rand()%200 -100),rand()%30 + 10,rand()%30 ,rand()%30+ 10,COLOR_GREY);



	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("Sample_GL.vert", "Sample_GL.frag");
	// Get a handle for our "MVP" uniform
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
	int height = 1600;

	window = initGLFW(width, height);

	initGL (window, width, height);

	/* Draw in loop */
	for (int t=0;!glfwWindowShouldClose(window);) {
		// Process timers

		if (t60.processTick()) {
			// 60 fps
			// OpenGL Draw commands
			if(boat_health <= 0)
				break;

			printf("%d\n",(t%180 -90) );
			wind = (t%30 -15);
			
			// Time for things to recharge
			if(t%50 == 0)
			{	
				sphere_hold = false;
				boost_use = true;
			}



			draw();
			// Swap Frame Buffer in double buffering
			glfwSwapBuffers(window);

			// All elements update 
			tick_elements();

			// Collision Engine
			collision_function();

			// Take input from user
			tick_input(window);
			
			reshapeWindow (window, width, height);

		// For stable time flow we update time here
		t++;
		}

		// Poll for Keyboard and mouse events
		glfwPollEvents();
	}

	quit(window);
}

bool detect_collision(bounding_box_t a, bounding_box_t b) {
	return (abs(a.x - b.x) * 2 <= (a.width + b.width)) &&
		   (abs(a.y - b.y) * 2 <= (a.height + b.height)) &&
		   (abs(a.z - b.z) * 2 <= (a.depth + b.depth));


}


