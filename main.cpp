// Just a simple spring mass cloth demo with minimal code for easy reading/reference
#include "Camera.h"
#include "Face.h"
#include "Particle.h"
#include "Spring.h"
#include "vmath.h"
#include <GL\freeglut.h>
#include <GL\GLU.h>
#include <math.h>
#include <vector>

void init();
void generateClothPlane();
void generateSprings(float dist);
void update(float dt);
void updateCamera(float dt);
void reshapeFunc(int width, int height);

void onMouseMove(int mouseX, int mouseY);
void onPassiveMouseMove(int mouseX, int mouseY);
void onMouseFunc(int button, int state, int x, int y);
void onMouseScroll(float dx);
void onKeyPress(unsigned char key, int x, int y);

void displayFunc();
void idleFunc();
void exit();

Camera cam;
vmath::vec2 mousePos;
float phi = 0.0f;
float theta = 0.0f;
float rho = 30.0f;

int previousTime = 0;
int shadeMode = 0;

vmath::vec3 spherePos;
float radius = 2.0f;

vmath::vec4 sphereMatAmbient;
vmath::vec4 sphereMatDiffuse;
vmath::vec4 sphereMatSpecular;

std::vector<Particle> particles;
std::vector<Spring> springs;
std::vector<Face> faces;
unsigned int gridSize = 41;
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
	
	glutInitWindowSize(static_cast<int>(1920.0 * 0.75), 
		static_cast<int>(1080.0 * 0.75));
	glutCreateWindow("Animate");
	glutCreateMenu(NULL);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);
	glDepthFunc(GL_LEQUAL);
	//glutFullScreen();

	// Set GLUT event callbacks
	glutMouseFunc(onMouseFunc);
	glutMotionFunc(onMouseMove);
	glutPassiveMotionFunc(onPassiveMouseMove);
	glutKeyboardFunc(onKeyPress);

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
	// Generate the intial cloth particles
	generateClothPlane();

	// Set the cloths material
	matAmbient = vmath::vec4(0.5f, 0.3f, 0.3f, 1.0f);
	matDiffuse = vmath::vec4(0.5f, 0.3f, 0.3f, 1.0f);
	matSpecular = vmath::vec4(0.0f, 0.0f, 0.0f, 0.5f);

	// Set the ground planes material
	planeMatAmbient = vmath::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	planeMatDiffuse = vmath::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	planeMatSpecular = vmath::vec4(0.0f, 0.0f, 0.0f, 0.5f);

	// Set the spheres material
	sphereMatAmbient = vmath::vec4(0.3f, 0.3f, 0.6f, 1.0f);
	sphereMatDiffuse = vmath::vec4(0.1f, 0.1f, 0.8f, 1.0f);
	sphereMatSpecular = vmath::vec4(0.0f, 0.0f, 0.0f, 0.5f);

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

void generateClothPlane()
{
	float halfSize = gridSize * 0.5f;
	particles.resize(gridSize * gridSize);
	int index = 0;
	float density = 0.4f;
	// Generate a bunch of particles in a plane
	for (unsigned int i = 0; i < gridSize; i++)
	{
		for (unsigned int j = 0; j < gridSize; j++)
		{
			particles[index++] = Particle(vmath::vec3((static_cast<float>(i) - halfSize) * density, 5.0f, (static_cast<float>(j) - halfSize)* density));
		}
	}

	// Fix particles
	// Two corners are fixed
	particles[0].isFixed = true;
	particles[gridSize - 1].isFixed = true;
	// Fix the first row
	/*for (int i = 0; i < gridSize; i++)
	{
		particles[i].isFixed = true;
	}*/
	// Last two corners are fixed
	particles[gridSize * gridSize - 1].isFixed = true;
	particles[gridSize * (gridSize - 1)].isFixed = true;

	generateSprings(density);

	// We use a an alternating [/][\] triangle setup
	faces.reserve(2 * gridSize * gridSize);
	index = 0;
	for (unsigned int i = 0; i < gridSize - 1; i++)
	{
		for (unsigned int j = 0; j < gridSize - 1; j++)
		{
			unsigned int index1 = i * gridSize + j;
			unsigned int index2 = index1 + 1;
			unsigned int index3 = index1 + gridSize;
			unsigned int index4 = index1 + gridSize + 1;

			// Exlusive or for alternating pattern
			if ((i % 2) ^ (j % 2))
			{
				faces.push_back(Face(&particles[index1], &particles[index2], &particles[index3]));
				particles[index1].faces.push_back(faces.back());
				particles[index2].faces.push_back(faces.back());
				particles[index3].faces.push_back(faces.back());

				faces.push_back(Face(&particles[index3], &particles[index2], &particles[index4]));
				particles[index3].faces.push_back(faces.back());
				particles[index2].faces.push_back(faces.back());
				particles[index4].faces.push_back(faces.back());
			}
			// Generate [\] triangle indices
			else
			{
				faces.push_back(Face(&particles[index2], &particles[index4], &particles[index1]));
				particles[index2].faces.push_back(faces.back());
				particles[index4].faces.push_back(faces.back());
				particles[index1].faces.push_back(faces.back());

				faces.push_back(Face(&particles[index1], &particles[index4], &particles[index3]));
				particles[index1].faces.push_back(faces.back());
				particles[index4].faces.push_back(faces.back());
				particles[index3].faces.push_back(faces.back());
			}
		}
	}
}

void generateSprings(float dist)
{
	// The diagonal distance is longer than the edge distance (for squares it's sqrt(dist^2 + dist^2))
	// Which is just sqrt2 * dist
	float diagDist = sqrt(2.0f) * dist;

	// Add the diagonal springs
	for (unsigned int x = 0; x < gridSize - 1; x++)
	{
		for (unsigned int y = 0; y < gridSize - 1; y++)
		{
			int topLeftIndex = x * gridSize + y;
			int topRightIndex = topLeftIndex + gridSize;
			int botLeftIndex = topLeftIndex + 1;
			int botRightIndex = topRightIndex + 1;
			springs.push_back(Spring(&particles[topLeftIndex], &particles[botRightIndex], diagDist));
			springs.push_back(Spring(&particles[topRightIndex], &particles[botLeftIndex], diagDist));
		}
	}
	// Add the vertical springs
	for (unsigned int x = 0; x < gridSize; x++)
	{
		// For every row - 1
		for (unsigned int y = 0; y < gridSize - 1; y++)
		{
			int topLeftIndex = y * gridSize + x;
			int topRightIndex = topLeftIndex + gridSize;
			springs.push_back(Spring(&particles[topLeftIndex], &particles[topRightIndex], dist));
		}
	}
	// Add the horizontal springs
	for (unsigned int y = 0; y < gridSize; y++)
	{
		// For every column -1 
		for (unsigned int x = 0; x < gridSize - 1; x++)
		{
			int botLeftIndex = y * gridSize + x;
			int botRightIndex = botLeftIndex + 1;
			springs.push_back(Spring(&particles[botLeftIndex], &particles[botRightIndex], dist));
		}
	}
}

void update(float dt)
{
	updateCamera(dt);

	// Apply forces from springs
	for (unsigned int i = 0; i < springs.size(); i++)
	{
		springs[i].applySpringForce();
	}

	// Particle collision with sphere (Triangle collision would be better)
	for (unsigned int i = 0; i < particles.size(); i++)
	{
		Particle& p = particles[i];
		// Distance between particle and sphere
		vmath::vec3 dist = p.pos - spherePos;
		// If the distance is less than the radius of the sphere then they're touching
		if (vmath::length(dist) < (radius + 0.2f))
		{
			// Collision resolution
			vmath::vec3 dir = vmath::normalize(dist);
			vmath::vec3 pos = dir * (radius + 0.2f) + spherePos;
			p.pos = pos;

			// Handle velocity (completely inelastic)
			// Remove velocity pointing into the sphere
			float l = vmath::dot(dir, p.velocity);
			if (l < 0.01)
				p.velocity = p.velocity - (l * dir);
		}
	}

	// Integrate the particles
	for (unsigned int i = 0; i < particles.size(); i++)
	{
		// Apply gravity
		particles[i].applyForce(vmath::vec3(0.0f, -0.25f, 0.0f));
		// Then integrate
		particles[i].integrate(0.05f);
	}

	// Update the normals
	for (unsigned int i = 0; i < particles.size(); i++)
	{
		particles[i].updateNormal();
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

void onKeyPress(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		shadeMode++;
		if (shadeMode > 3)
			shadeMode = 0;
		break;
	case'l':
		spherePos[0] += 0.1f;
		break;
	case'j':
		spherePos[0] -= 0.1f;
		break;
	case 27:
		exit(0);
		break;

	default:
		break;
	}
}


// Forward the callbacks to the class
void displayFunc()
{
	glClearColor(0.4f, 0.6f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set the draw information and lightss
	if (shadeMode == 3)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		glPointSize(6.0f); // Just the triangle points
	}
	else if (shadeMode == 2)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(2.0f); // For wireframe triangles
	}
	else // 0, 1 are flat and smooth shading modes
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // For filled triangles
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, &lightAmbient[0]);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, &lightDiffuse[0]);
	glLightfv(GL_LIGHT0, GL_SPECULAR, &lightSpecular[0]);
	glLightfv(GL_LIGHT0, GL_POSITION, vmath::normalize(vmath::vec4(0.0f, 15.0f, 0.0f, 1.0f)));
	if (shadeMode == 0)
		glShadeModel(GL_SMOOTH);
	else
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
	if (shadeMode == 0)
	{
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			Particle* p1 = faces[i].p1;
			Particle* p2 = faces[i].p2;
			Particle* p3 = faces[i].p3;
			// Smooth normals
			glNormal3f(p1->normal[0], p1->normal[1], p1->normal[2]);
			glVertex3f(p1->pos[0], p1->pos[1], p1->pos[2]);
			glNormal3f(p2->normal[0], p2->normal[1], p2->normal[2]);
			glVertex3f(p2->pos[0], p2->pos[1], p2->pos[2]);
			glNormal3f(p3->normal[0], p3->normal[1], p3->normal[2]);
			glVertex3f(p3->pos[0], p3->pos[1], p3->pos[2]);
		}
	}
	else
	{
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			Particle* p1 = faces[i].p1;
			Particle* p2 = faces[i].p2;
			Particle* p3 = faces[i].p3;
			// Flat normals
			vmath::vec3 normal = vmath::normalize(vmath::cross(p2->pos - p1->pos, p3->pos - p1->pos));
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(p1->pos[0], p1->pos[1], p1->pos[2]);
			glVertex3f(p2->pos[0], p2->pos[1], p2->pos[2]);
			glVertex3f(p3->pos[0], p3->pos[1], p3->pos[2]);
		}
	}
	glEnd();

	//glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_AMBIENT, &sphereMatAmbient[0]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, &sphereMatDiffuse[0]);
	glMaterialfv(GL_FRONT, GL_SPECULAR, &sphereMatSpecular[0]);
	glMaterialf(GL_FRONT, GL_SHININESS, sphereMatSpecular[3]);
	glTranslatef(spherePos[0], spherePos[1], spherePos[2]);
	glutSolidSphere(radius, 20, 20);

	// Draw the springs
	/*glDisable(GL_LIGHTING);
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	for (unsigned int i = 0; i < springs.size(); i++)
	{
		vmath::vec3 v1 = springs[i].p1->pos;
		vmath::vec3 v2 = springs[i].p2->pos;
		glVertex3f(v1[0], v1[1], v1[2]);
		glVertex3f(v2[0], v2[1], v2[2]);
	}
	glEnd();
	glEnable(GL_LIGHTING);*/

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
	//glutDestroyWindow(0);
}