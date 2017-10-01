#include <GL\freeglut.h>
#include <GL\GLU.h>
#include <math.h>

#include "vmath.h"
#include "Camera.h"

Camera cam;
vmath::vec2 mousePos;
float phi = 0.0f;
float theta = 0.0f;
float rho = 20.0f;

int currentTime = 1;
float fps = 30.0;
int maxTime = 1200;

GLvoid Timer(int value);
void displayFunc();
void init();
void updateCamera();
void reshapeFunc(int width, int height);
void onMouseMove(int mouseX, int mouseY);
void onMouseScroll(float dx);
void onPassiveMouseMove(int mouseX, int mouseY);
void onMouseFunc(int button, int state, int x, int y);
void exit();

int main(int argc, char** argv)
{
	// Setup glut and opengl
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	
	glutInitWindowSize(1600, 900);
	glutCreateWindow("Animate");
	glutCreateMenu(NULL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);
	glDepthFunc(GL_LEQUAL);

	// Set GLUT event callbacks
	// - Directly redirect GLUT mouse button events to AntTweakBar
	glutMouseFunc(onMouseFunc);
	// - Directly redirect GLUT mouse motion events to AntTweakBar
	glutMotionFunc(onMouseMove);
	// - Directly redirect GLUT mouse "passive" motion events to AntTweakBar (same as MouseMotion)
	glutPassiveMotionFunc(onPassiveMouseMove);

	glutDisplayFunc(displayFunc);
	glutReshapeFunc(reshapeFunc);

	init();

	glutMainLoop();
	atexit(exit);
	return 0;
}

// Initializaiton code
void init()
{
	// Setup camera
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	cam.setPerspective(60, static_cast<float>(width) / static_cast<float>(height), 0.001f, 500.0f);
	// Init view
	theta = 1.57f;
	phi = 1.4f;
	updateCamera();
}


GLvoid Timer(int value)
{
	currentTime++;
	currentTime %= maxTime;
	//app.update();
	if (value)
		glutPostRedisplay();
	glutTimerFunc(static_cast<int>(1200 / fps), Timer, value);
}

void updateCamera()
{
	// Clamp
	if (phi > 3.14f)
		phi = 3.14f;
	else if (phi < 0.01f)
		phi = 0.01f;

	// Convert spherical coords
	float x = rho * sin(phi) * cos(theta);
	float y = rho * cos(phi);
	float z = rho * sin(phi) * sin(theta);

	// Should prob use a slerp here but ohwell
	cam.setEyePos(vmath::lerp(cam.eyePos, vmath::vec3(x, y, z), 0.3f));
	cam.setFocalPt(0.0f, 0.0f, 0.0f);
}

void reshapeFunc(int width, int height)
{
	glViewport(0, 0, width, height);
	cam.setPerspective(60, static_cast<float>(width) / static_cast<float>(height), 0.1f, 500.0f);
}

void onMouseMove(int mouseX, int mouseY)
{
	// Calculate the change in mouse pos
	float dx = mouseX - mousePos[0];
	float dy = mouseY - mousePos[1];
	mousePos = vmath::vec2(static_cast<float>(mouseX), static_cast<float>(mouseY));

	// Update phi and theta
	theta += dx * 0.008f;
	phi -= dy * 0.008f;
}

void onPassiveMouseMove(int mouseX, int mouseY)
{
	mousePos = vmath::vec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
}

void onMouseFunc(int button, int state, int x, int y)
{
	if (button == 3)
		onMouseScroll(-1.0f);
	else if (button == 4)
		onMouseScroll(1.0f);
}

void onMouseScroll(float dx)
{
	rho += dx;
}


// Forward the callbacks to the class
void displayFunc()
{
	updateCamera();

	glClearColor(0.4f, 0.6f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}

void exit()
{
	
}