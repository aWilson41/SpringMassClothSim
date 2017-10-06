#include <GL\freeglut.h>
#include <GL\GLU.h>
#include <math.h>
#include <vector>

#include "Spring.h"
#include "Particle.h"
#include "vmath.h"
#include "Camera.h"

void init();
void generateSprings(int gridSize, float dist);
void update(float dt);
void updateCamera(float dt);
void reshapeFunc(int width, int height);

void onMouseMove(int mouseX, int mouseY);
void onPassiveMouseMove(int mouseX, int mouseY);
void onMouseFunc(int button, int state, int x, int y);
void onMouseScroll(float dx);

void displayFunc();
void idleFunc();
void exit();

Camera cam;
vmath::vec2 mousePos;
float phi = 0.0f;
float theta = 0.0f;
float rho = 30.0f;

int previousTime = 0;

std::vector<Particle> particles;
std::vector<Spring> springs;
std::vector<unsigned int> indices;
vmath::vec4 matAmbient;
vmath::vec4 matDiffuse;
vmath::vec4 matSpecular;

vmath::vec4 planeMatAmbient;
vmath::vec4 planeMatDiffuse;
vmath::vec4 planeMatSpecular;

vmath::vec4 lightAmbient;
vmath::vec4 lightDiffuse;
vmath::vec4 lightSpecular;

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
	glutIdleFunc(idleFunc);
	previousTime = glutGet(GLUT_ELAPSED_TIME);

	init();

	glutMainLoop();
	atexit(exit);
	return 0;
}

// Initializaiton code
void init()
{
	int gridSize = 25;
	float halfSize = gridSize * 0.5f;
	particles.resize(gridSize * gridSize);
	int index = 0;
	float density = 0.5f;
	// Generate a triangle plane for drawing
	for (int i = 0; i < gridSize; i++)
	{
		for (int j = 0; j < gridSize; j++)
		{
			particles[index++] = Particle(vmath::vec3((static_cast<float>(i) - halfSize) * density, 2.0f, (static_cast<float>(j) - halfSize)* density));
		}
	}

	// Two corners are fixed
	particles[0].isFixed = true;
	particles[gridSize - 1].isFixed = true;
	// Fix the first row
	/*for (int i = 0; i < gridSize; i++)
	{
		particles[i].isForceApplied = false;
	}*/
	// Last two corners are fixed
	particles[gridSize * gridSize - 1].isFixed = true;
	particles[gridSize * gridSize - gridSize].isFixed = true;

	generateSprings(gridSize, density);

	// Setup indices
	// Every triangle is drawn independently (overlap)
	indices.resize(6 * gridSize * gridSize);
	index = 0;
	for (unsigned int i = 0; i < gridSize - 1; i++)
	{
		for (unsigned int j = 0; j < gridSize - 1; j++)
		{
			indices[index++] = i * gridSize + j;
			indices[index++] = i * gridSize + j + 1;
			indices[index++] = (i + 1) * gridSize + j;

			indices[index++] = (i + 1) * gridSize + j;
			indices[index++] = i * gridSize + j + 1;
			indices[index++] = (i + 1) * gridSize + j + 1;
		}
	}

	// Set the cloths material
	matAmbient = vmath::vec4(0.5f, 0.3f, 0.3f, 1.0f);
	matDiffuse = vmath::vec4(0.5f, 0.3f, 0.3f, 1.0f);
	matSpecular = vmath::vec4(0.0f, 0.0f, 0.0f, 0.5f);

	// Set the ground planes material
	planeMatAmbient = vmath::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	planeMatDiffuse = vmath::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	planeMatSpecular = vmath::vec4(0.0f, 0.0f, 0.0f, 0.5f);

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
	updateCamera(0.016f);
}

void generateSprings(int gridSize, float dist)
{
	//We use this index to iterate through the list of springs
	int index = 0;
	float diagDist = sqrt(pow(dist, 2) + pow(dist, 2));

	// There are 4 springs per voxel
	int numQuads = gridSize - 1;
	//springs.resize(numQuads * (numQuads * 4 + 2));

	for (unsigned int i = 0; i < numQuads; i++)
	{
		for (unsigned int j = 0; j < numQuads; j++)
		{
			int topLeftIndex = i * gridSize + j;
			int topRightIndex = (i + 1) * gridSize + j;
			int bottomLeftIndex = i * gridSize + j + 1;
			int bottomRightIndex = (i + 1) * gridSize + j + 1;
			springs.push_back(Spring(&particles[topLeftIndex], &particles[topRightIndex], dist));
			springs.push_back(Spring(&particles[topLeftIndex], &particles[bottomLeftIndex], dist));
			springs.push_back(Spring(&particles[topRightIndex], &particles[bottomLeftIndex], diagDist));
			springs.push_back(Spring(&particles[topLeftIndex], &particles[bottomRightIndex], diagDist));
		}
	}
	// Then fill in the missing few triangles
	for (unsigned int i = numQuads; i < gridSize * numQuads; i += gridSize)
	{
		springs.push_back(Spring(&particles[i], &particles[i + gridSize], dist));
		/*springs.back().p1->isForceApplied = false;
		springs.back().p2->isForceApplied = false;*/
	}
	for (unsigned int j = numQuads * gridSize; j < numQuads * gridSize + numQuads; j++)
	{
		springs.push_back(Spring(&particles[j], &particles[j + 1], dist));
	}

	//int numQuads = gridSize - 1;
	//springs.resize(6 * numQuads * numQuads);

	//// Iterate through each Particle to create our springs
	//for (int i = 0; i < gridSize; i++)
	//{
	//	for (int j = 0; j < gridSize; j++)
	//	{
	//		// Right border and not top
	//		if (i == gridSize - 1 && j < gridSize - 1)
	//			springs[index++] = Spring(&particles[i * gridSize + j], &particles[i * gridSize + j + 1], dist);
	//		// At the top of the cloth
	//		else if (j == 0)
	//		{
	//			springs[index++] = Spring(&particles[i * gridSize + j], &particles[(i + 1) * gridSize + j], dist);
	//			springs[index++] = Spring(&particles[i * gridSize + j], &particles[i * gridSize + j + 1], dist);
	//		}
	//		// Between the top and down broders of the cloth
	//		else if (j < gridSize - 1)
	//		{
	//			springs[index++] = Spring(&particles[i * gridSize + j], &particles[(i + 1) * gridSize + j - 1], diagDist);
	//			springs[index++] = Spring(&particles[i * gridSize + j - 1], &particles[(i + 1) * gridSize + j], diagDist);
	//			springs[index++] = Spring(&particles[i * gridSize + j], &particles[(i + 1) * gridSize + j], dist);
	//			springs[index++] = Spring(&particles[i * gridSize + j], &particles[i * gridSize + j + 1], dist);
	//		}
	//		else if (i == gridSize - 1 && j == gridSize - 1)
	//			continue;
	//		// At the bottom of the cloth
	//		else
	//		{
	//			springs[index++] = Spring(&particles[i * gridSize + j], &particles[(i + 1) * gridSize + j - 1], diagDist);
	//			springs[index++] = Spring(&particles[i * gridSize + j], &particles[(i + 1) * gridSize + j], dist);
	//			springs[index++] = Spring(&particles[i * gridSize + j - 1], &particles[(i + 1) * gridSize + j], dist);
	//		}
	//	}
	//}
}


void update(float dt)
{
	updateCamera(dt);

	// Apply forces from springs
	for (unsigned int i = 0; i < springs.size(); i++)
	{
		springs[i].applySpringForce();
	}

	// Integrate the particles
	for (unsigned int i = 0; i < particles.size(); i++)
	{
		// Apply gravity
		particles[i].applyForce(vmath::vec3(0.0f, -0.25f, 0.0f));

		particles[i].integrate(dt * 5.0f);
	}
}

// Update the camera position
void updateCamera(float dt)
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
	cam.setEyePos(vmath::lerp(cam.eyePos, vmath::vec3(x, y, z), dt * 12.0f));
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
	idleFunc();
}

// Called from onMouseFunc to handle scrolling
void onMouseScroll(float dx)
{
	rho += dx;
}


// Forward the callbacks to the class
void displayFunc()
{
	glClearColor(0.4f, 0.6f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set the draw information and lightss
	//glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	//glPointSize(6.0f); // Just the triangle points
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glLineWidth(2.0f); // For wireframe triangles
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // For filled triangles
	glFrontFace(GL_CCW);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, &lightAmbient[0]);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, &lightDiffuse[0]);
	glLightfv(GL_LIGHT0, GL_SPECULAR, &lightSpecular[0]);
	glLightfv(GL_LIGHT0, GL_POSITION, vmath::normalize(vmath::vec4(1.0f, 1.0f, 0.0f, 0.0f)));
	glShadeModel(GL_FLAT);

	// Begin drawing
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(cam.fov, cam.aspectRatio, cam.nearZ, cam.farZ);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixf(cam.view[0]);

	// Draw the cloth
	// Set the material to use
	glMaterialfv(GL_FRONT, GL_AMBIENT, &matAmbient[0]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, &matDiffuse[0]);
	glMaterialfv(GL_FRONT, GL_SPECULAR, &matSpecular[0]);
	glMaterialf(GL_FRONT, GL_SHININESS, matSpecular[3]);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < indices.size(); i += 3)
	{
		vmath::vec3 vertex1 = particles[indices[i]].pos;
		vmath::vec3 vertex2 = particles[indices[i + 1]].pos;
		vmath::vec3 vertex3 = particles[indices[i + 2]].pos;
		vmath::vec3 normal = vmath::cross(vertex2 - vertex1, vertex3 - vertex1);

		glVertex3f(vertex1[0], vertex1[1], vertex1[2]);
		glNormal3f(normal[0], normal[1], normal[2]);
		glVertex3f(vertex2[0], vertex2[1], vertex2[2]);
		glNormal3f(normal[0], normal[1], normal[2]);
		glVertex3f(vertex3[0], vertex3[1], vertex3[2]);
		glNormal3f(normal[0], normal[1], normal[2]);
	}
	glEnd();

	//glDisable(GL_LIGHTING);
	//glColor3f(1.0f, 0.0f, 0.0f);
	//// Draw the particles
	//glBegin(GL_LINES);
	//for (unsigned int i = 0; i < springs.size(); i++)
	//{
	//	vmath::vec3 v1 = springs[i].p1->pos;
	//	vmath::vec3 v2 = springs[i].p2->pos;
	//	glVertex3f(v1[0], v1[1], v1[2]);
	//	glVertex3f(v2[0], v2[1], v2[2]);
	//}
	//glEnd();

	//glEnable(GL_LIGHTING);
	// Draw ground plane
	/*glMaterialfv(GL_FRONT, GL_AMBIENT, &planeMatAmbient[0]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, &planeMatDiffuse[0]);
	glMaterialfv(GL_FRONT, GL_SPECULAR, &planeMatSpecular[0]);
	glMaterialf(GL_FRONT, GL_SHININESS, planeMatSpecular[3]);
	glBegin(GL_QUADS);
	glVertex3f(15.0f, -10.0f, -15.0f);
	glVertex3f(15.0f, -10.0f, 15.0f);
	glVertex3f(-15.0f, -10.0f, 15.0f);
	glVertex3f(-15.0f, -10.0f, -15.0f);
	glEnd();*/

	glutSwapBuffers();
	glutPostRedisplay();
}

void idleFunc()
{
	int t = glutGet(GLUT_ELAPSED_TIME);
	float dt = static_cast<float>(t - previousTime) / 1000.0f;
	previousTime = t;
	//printf("Dt: %f\n", dt);
	update(dt);
	glutPostRedisplay();
}

void exit()
{
	
}