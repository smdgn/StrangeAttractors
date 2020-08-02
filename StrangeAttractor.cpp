// OpenGL Software zur Visualisierung eines Attraktors
// Programm umgesetzt mit der GLTools Library und ObjCreator
// Autor: Samed Dogan

#include <iostream>
#ifdef WIN32
#include <windows.h>
#endif
#define GLM_FORCE_RADIANS
#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/string_cast.hpp>
#include <GLTools.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>
#include <GLFrustum.h>
#include <math.h>
#include <math3d.h>
#include <GL/freeglut.h>
#include <AntTweakBar.h>
#include "ObjCreator.h"
#include <random>
#include "Attractors.h"

using namespace Attr;

//Camera Stacks
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLGeometryTransform transformPipeline;
GLFrustum viewFrustum;

//Shader, stock shaders are used
GLShaderManager sm;

//Rendered Object
ObjCreator attractor;
Attractor strangeAttr(LORENZ, 0.01);
const int NUM_POINTS = 100000;
std::vector<glm::vec3> vertexdata;
std::vector<glm::vec4> colordata;

//Counter for Timer function
double counter;

// Rotation Matrix
glm::quat rotation = glm::quat(0, 0, 0, 1);

// GUI
TwBar *bar;

//Define Constants

TwEnumVal A[] = { {LORENZ, "Lorenz"}, {THOMAS, "Thomas"}, {AIZAWA, "Aizawa"}, {DADRAS, "Dadras"}, {CHEN, "Chen"}, {HALVORSEN, "Halvorsen"}, {RABINOVICH, "Rabinovich"}, {THREESCROLL, "Three-Scroll Chaotic System"}, {CUSTOM, "Custom"} };
AttrType AtType = LORENZ;

//Current attraktor vector and startingpoint
glm::vec3 datapoint = glm::vec3(1.1f, 1.1f, -0.1);


void Timerfunc(int u) {
	counter += 0.1;
	glutPostRedisplay();
	glutTimerFunc(200, Timerfunc, 0);
}

void CreateGeometry()
{
	//Fill the container with random starting Points
	std::default_random_engine generator;
	std::normal_distribution<float> distribution(0.0, 1.0);
	for (int i = 0; i < NUM_POINTS; i++) {
		vertexdata.push_back(strangeAttr.getPoint() + distribution(generator));
		colordata.push_back(glm::vec4(0.2, 0.1*(i%10), 0.3*(i%5), 1.0));
	}
}

void TW_CALL SetAttractorCB(const void * value, void * clientData) { //Callback
	AtType = *(const AttrType *)value;
	vertexdata.clear();
	colordata.clear();
	strangeAttr.setType(AtType);
	CreateGeometry();
}

void TW_CALL GetAttractorCB(void * value, void * clientData) {
	(void)clientData;
	*(AttrType*)value = AtType;
}


void InitGUI()
{
	TwType MeshAttr = TwDefineEnum("Attractor", A, _LAST_ENTRY);
	bar = TwNewBar("TweakBar");
	TwDefine(" TweakBar size='200 400'");
	TwAddVarRW(bar, "Model Rotation", TW_TYPE_QUAT4F, &rotation, "");
	TwAddVarCB(bar, "Attractor", MeshAttr, SetAttractorCB, GetAttractorCB, NULL, "");

}

// Call Draw Scene
void RenderScene(void)
{
	// Clear depth and color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);


	// Obj Rotation
	modelViewMatrix.PushMatrix();
	glm::mat4 rot = glm::mat4_cast(rotation);
	modelViewMatrix.MultMatrix(glm::value_ptr(rot));

	//Update container
	for (auto it = vertexdata.begin(); it != vertexdata.end(); ++it) {
		*it = strangeAttr.compute(*it);
		//vertexdata.push_back(datapoint*0.1f);//gltCheckErrors(shaders);
		//colordata.push_back(glm::vec4(int(counter) % 1, 0.5f, counter, 1.0f));
	}
	modelViewMatrix.Scale(0.1, 0.1, 0.1);
	sm.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES, transformPipeline.GetModelViewProjectionMatrix());
	//Draw the model
	attractor.bindColorData(colordata);
	attractor.bindVertexData(vertexdata);
	
	glPointSize(1.0f);
	attractor.draw(GL_POINTS);

	// Pop Stack, Draw Gui
	modelViewMatrix.PopMatrix();
	//gltCheckErrors(shaders);
	TwDraw();

	//Swap Buffers
	glutSwapBuffers();
	glutPostRedisplay();
}

// Rendering Context
void SetupRC()
{
	// Black Background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glFrontFace(GL_CW);

	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
	sm.InitializeStockShaders();
	
	CreateGeometry();

	InitGUI();
}

void ShutDownRC()
{
	TwTerminate();
}


void ChangeSize(int w, int h)
{
	//Model ist normalisiert im Ursprung
	GLfloat nRange = 1.0f;
	// Verhindere eine Division durch Null
	if (h == 0)
		h = 1;
	// Setze den Viewport gemaess der Window-Groesse
	glViewport(0, 0, w, h);
	// Ruecksetzung des Projection matrix stack
	projectionMatrix.LoadIdentity();
	viewFrustum.SetPerspective(90.0f, w / (float)h, 0.1f, 100.0f);
	// Definiere das viewing volume (left, right, bottom, top, near, far)
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	// Ruecksetzung des Model view matrix stack
	modelViewMatrix.LoadIdentity();
	modelViewMatrix.Translate(0, 0, -1.5f);
	// Send the new window size to AntTweakBar
	TwWindowSize(w, h);
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Attractors");
	glutCloseFunc(ShutDownRC);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		// Veralteter Treiber etc.
		std::cerr << "Error: " << glewGetErrorString(err) << "\n";
		return 1;
	}

	glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
	glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT); // same as MouseMotion
	glutKeyboardFunc((GLUTkeyboardfun)TwEventKeyboardGLUT);
	glutSpecialFunc((GLUTspecialfun)TwEventKeyboardGLUT);

	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);

	TwInit(TW_OPENGL_CORE, NULL);
	SetupRC();

	Timerfunc(0);

	glutMainLoop();

	return 0;
}
