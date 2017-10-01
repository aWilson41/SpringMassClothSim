#include <GL\freeglut.h>
#include <GL\GLU.h>
#include <math.h>
#include <vector>

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

std::vector<vmath::vec3> vertices;
std::vector<unsigned int> indices;
vmath::vec4 matAmbient;
vmath::vec4 matDiffuse;
vmath::vec4 matSpecular;

vmath::vec4 lightAmbient;
vmath::vec4 lightDiffuse;
vmath::vec4 lightSpecular;

//GLvoid Timer(int value);
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
	//glEnable(GL_CULL_FACE);
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
	int gridSize = 10;
	// Generate a triangle plane for drawing
	for (int i = 0; i < gridSize; i++)
	{
		for (int j = 0; j < gridSize; j++)
		{
			vertices.push_back(vmath::vec3(static_cast<float>(i - 5), static_cast<float>(j - 5), 0.0f));
		}
	}

	// Setup indices
	for (int r = 0; r < gridSize - 1; r++)
	{
		indices.push_back(r * gridSize);
		for (int c = 0; c < gridSize; c++)
		{
			indices.push_back(r * gridSize + c);
			indices.push_back((r + 1) * gridSize + c);
		}
		indices.push_back((r + 1) * gridSize + (gridSize - 1));
	}

	// Set the planes material
	matAmbient = vmath::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	matDiffuse = vmath::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	matSpecular = vmath::vec4(0.0f, 0.0f, 0.0f, 0.5f);

	// Setup the directional light
	lightAmbient = vmath::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	lightDiffuse = vmath::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	lightSpecular = vmath::vec4(0.0f, 0.0f, 0.0f, 0.5f);

	// Setup camera
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	cam.setPerspective(60, static_cast<float>(width) / static_cast<float>(height), 0.001f, 500.0f);
	// Init view
	theta = 1.57f;
	phi = 1.4f;
	updateCamera();
}


//GLvoid Timer(int value)
//{
//	currentTime++;
//	currentTime %= maxTime;
//	//app.update();
//	if (value)
//		glutPostRedisplay();
//	glutTimerFunc(static_cast<int>(1200 / fps), Timer, value);
//}

// Update the camera position
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

// Called when the window is resized
void reshapeFunc(int width, int height)
{
	glViewport(0, 0, width, height);
	cam.setPerspective(60, static_cast<float>(width) / static_cast<float>(height), 0.001f, 500.0f);
}

// Called when the mouse is moved and pressed
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

// Called when the mouse is moved and nothing is pressed
void onPassiveMouseMove(int mouseX, int mouseY)
{
	mousePos = vmath::vec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
}

// Called for every mouse function (I use it to capture scrolling)
void onMouseFunc(int button, int state, int x, int y)
{
	if (button == 3)
		onMouseScroll(-1.0f);
	else if (button == 4)
		onMouseScroll(1.0f);
}

// Called from onMouseFunc to handle scrolling
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

	// Set the draw information and lightss
	/*glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	glPointSize(6.0f);*/ // Just the triangle points
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(2.0f); // For wireframe triangles
	/*glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);*/ // For filled triangles
	glFrontFace(GL_CCW);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, &lightAmbient[0]);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, &lightDiffuse[0]);
	glLightfv(GL_LIGHT0, GL_SPECULAR, &lightSpecular[0]);
	glLightfv(GL_LIGHT0, GL_POSITION, vmath::normalize(vmath::vec4(0.0f, 1.0f, 1.0f, 0.0f)));
	glShadeModel(GL_FLAT);

	// Begin drawing
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(cam.fov, cam.aspectRatio, cam.nearZ, cam.farZ);

	// Draw the plane
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixf(cam.view[0]);
	// Set the material to use
	glMaterialfv(GL_FRONT, GL_AMBIENT, &matAmbient[0]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, &matDiffuse[0]);
	glMaterialfv(GL_FRONT, GL_SPECULAR, &matSpecular[0]);
	glMaterialf(GL_FRONT, GL_SHININESS, matSpecular[3]);
	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i < indices.size(); i++)
	{
		vmath::vec3 vertex = vertices[indices[i]];
		glVertex3f(vertex[0], vertex[1], vertex[2]);
	}
	glEnd();

	glutSwapBuffers();
	glutPostRedisplay();
}

void exit()
{
	
}