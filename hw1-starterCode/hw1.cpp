/*
  CSCI 420 Computer Graphics, USC
  Assignment 2: Roller Coaster
  C++ starter code

  Student username: vivekras@usc.edu
  Student Name: Vivek Raj Saxena
  Student ID: 6834311974
*/

#include <iostream>
#include <cstring>
#include <cmath>
#include "openGLHeader.h"
#include "glutHeader.h"

#include "imageIO.h"
#include "openGLMatrix.h"
#include "basicPipelineProgram.h"


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double CRSpline(double p1, double p2, double p3, double p4, double t)
{
	return 0.5 * ((-p1 + 3 * p2 - 3 * p3 + p4)*t*t*t
		+ (2 * p1 - 5 * p2 + 4 * p3 - p4)*t*t
		+ (-p1 + p3)*t
		+ 2 * p2);
}

double dCRSpline(double p1, double p2, double p3, double p4, double t)
{
	return 0.5 * ((-p1 + 3 * p2 - 3 * p3 + p4) * 3 * t*t
		+ (2 * p1 - 5 * p2 + 4 * p3 - p4) * 2 * t
		+ (-p1 + p3)
		);
}

double ddCRSpline(double p1, double p2, double p3, double p4, double t)
{
	return 0.5 * ((-p1 + 3 * p2 - 3 * p3 + p4) * 6 *t
		+ (2 * p1 - 5 * p2 + 4 * p3 - p4) * 2 
		);
}


// represents one control point along the spline 
struct Point 
{
	double x;
	double y;
	double z;
};

struct Point cross(struct Point  a, struct Point b)
{
	struct Point r;
	r.x = a.y * b.z - a.z * b.y;
	r.y = a.z * b.x - a.x * b.z;
	r.z = a.x * b.y - a.y * b.x;
	return r;
}

double dot(struct Point  a, struct Point b)
{
	return a.x*b.x+a.y*b.y+a.z*b.z;
}

struct Point CRSpline(struct Point p1, struct Point p2, struct Point p3, struct Point p4, double t)
{
	struct Point c;
	c.x = CRSpline(p1.x, p2.x, p3.x, p4.x, t);
	c.y = CRSpline(p1.y, p2.y, p3.y, p4.y, t);
	c.z = CRSpline(p1.z, p2.z, p3.z, p4.z, t);

	return c;
}

struct Point dCRSpline(struct Point p1, struct Point p2, struct Point p3, struct Point p4, double t)
{
	struct Point c;
	c.x = dCRSpline(p1.x, p2.x, p3.x, p4.x, t);
	c.y = dCRSpline(p1.y, p2.y, p3.y, p4.y, t);
	c.z = dCRSpline(p1.z, p2.z, p3.z, p4.z, t);

	return c;
}
struct Point ddCRSpline(struct Point p1, struct Point p2, struct Point p3, struct Point p4, double t)
{
	struct Point c;
	c.x = ddCRSpline(p1.x, p2.x, p3.x, p4.x, t);
	c.y = ddCRSpline(p1.y, p2.y, p3.y, p4.y, t);
	c.z = ddCRSpline(p1.z, p2.z, p3.z, p4.z, t);

	return c;
}




struct Point add(struct Point a, struct Point b)
{
	struct Point c;
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
	return c;
};


struct Point sub(struct Point a, struct Point b)
{
	struct Point c;
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
	return c;
};


struct Point negate(struct Point b)
{
	struct Point c;
	c.x =  - b.x;
	c.y =  - b.y;
	c.z =  - b.z;
	return c;
};

struct Point mul(struct Point a, double s)
{
	struct Point c;
	c.x = a.x*s;
	c.y = a.y*s;
	c.z = a.z*s;
	return c;
};

struct Point interpolate(struct Point a, struct Point b, double s)
{
	return add(mul(a,1-s),mul(b,s));
};

double length(struct Point a)
{
	return sqrt(a.x*a.x+a.y*a.y+a.z*a.z);
};

double dist(struct Point a, struct Point b)
{
	struct Point c=sub(a,b);

	return length(c);
};

struct Point normalize(struct Point a)
{
	struct Point c;
	double l = length(a);
	if (!l) l = 1;
	c.x = a.x / l;
	c.y = a.y / l;
	c.z = a.z / l;
	return c;
};

// spline struct 
// contains how many control points the spline has, and an array of control points 
struct Spline
{
	int numControlPoints;
	Point * points;
};

// the spline array 
Spline * splines;
// total number of splines 
int numSplines;


struct GPoint3f
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

struct GPoint2f
{
	GLfloat x;
	GLfloat y;
};

struct VertexData
{
	struct GPoint3f pos;
	struct GPoint3f normal;
	struct GPoint2f texCoord;
};

struct GPoint3f toGPoint(struct Point pt)
{
	struct GPoint3f res;
	res.x = pt.x;
	res.y = pt.y;
	res.z = pt.z;
	return res;
}

int loadSplines(char * argv)
{
	char * cName = (char *)malloc(128 * sizeof(char));
	FILE * fileList;
	FILE * fileSpline;
	int iType, i = 0, j, iLength;

	// load the track file 
	fileList = fopen(argv, "r");
	if (fileList == NULL)
	{
		printf("can't open file\n");
		exit(1);
	}

	// stores the number of splines in a global variable 
	fscanf(fileList, "%d", &numSplines);
	printf("N %d %s\n",numSplines,argv);
	splines = (Spline*)malloc(numSplines * sizeof(Spline));

	// reads through the spline files 
	for (j = 0; j < numSplines; j++)
	{
		i = 0;
		fscanf(fileList, "%s", cName);
		fileSpline = fopen(cName, "r");

		if (fileSpline == NULL)
		{
			printf("can't open file\n");
			exit(1);
		}

		// gets length for spline file
		fscanf(fileSpline, "%d %d", &iLength, &iType);

		// allocate memory for all the points
		splines[j].points = (Point *)malloc(iLength * sizeof(Point));
		splines[j].numControlPoints = iLength;

		// saves the data to the struct
		while (fscanf(fileSpline, "%lf %lf %lf",
			&splines[j].points[i].x,
			&splines[j].points[i].y,
			&splines[j].points[i].z) != EOF)
		{
			i++;
		}
	}

	free(cName);

	return 0;
}

int initTexture(const char * imageFilename, GLuint textureHandle,bool transparentBack=false)
{
	// read the texture image
	ImageIO img;
	ImageIO::fileFormatType imgFormat;
	ImageIO::errorType err = img.load(imageFilename, &imgFormat);

	if (err != ImageIO::OK)
	{
		printf("Loading texture from %s failed.\n", imageFilename);
		return -1;
	}

	// check that the number of bytes is a multiple of 4
	if (img.getWidth() * img.getBytesPerPixel() % 4)
	{
		printf("Error (%s): The width*numChannels in the loaded image must be a multiple of 4.\n", imageFilename);
		return -1;
	}

	// allocate space for an array of pixels
	int width = img.getWidth();
	int height = img.getHeight();
	unsigned char * pixelsRGBA = new unsigned char[4 * width * height]; // we will use 4 bytes per pixel, i.e., RGBA

	// fill the pixelsRGBA array with the image pixels
	memset(pixelsRGBA, 0, 4 * width * height); // set all bytes to 0
	for (int h = 0; h < height; h++)
		for (int w = 0; w < width; w++)
		{
			// assign some default byte values (for the case where img.getBytesPerPixel() < 4)
			pixelsRGBA[4 * (h * width + w) + 0] = 0; // red
			pixelsRGBA[4 * (h * width + w) + 1] = 0; // green
			pixelsRGBA[4 * (h * width + w) + 2] = 0; // blue
			pixelsRGBA[4 * (h * width + w) + 3] = 255; // alpha channel; fully opaque

			// set the RGBA channels, based on the loaded image
			int numChannels = img.getBytesPerPixel();
			for (int c = 0; c < numChannels; c++) // only set as many channels as are available in the loaded image; the rest get the default value
				pixelsRGBA[4 * (h * width + w) + c] = img.getPixel(w, h, c);


			if(transparentBack&&
			(!pixelsRGBA[4 * (h * width + w) + 0])&&
			(!pixelsRGBA[4 * (h * width + w) + 1])&&
			(!pixelsRGBA[4 * (h * width + w) + 2])
			)
			pixelsRGBA[4 * (h * width + w) + 3] = 0; // alpha channel; fully transparent
		}

	// bind the texture
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	// initialize the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelsRGBA);

	// generate the mipmaps for this texture
	glGenerateMipmap(GL_TEXTURE_2D);

	// set the texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// query support for anisotropic texture filtering
	GLfloat fLargest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	printf("Max available anisotropic samples: %f\n", fLargest);
	// set anisotropic texture filtering
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 0.5f * fLargest);
	delete[] pixelsRGBA;

	// query for any errors
	GLenum errCode = glGetError();
	if (errCode != 0)
	{
		printf("Texture initialization error. Error code: %d.\n", errCode);
		return -1;
	}

	// de-allocate the pixel array -- it is no longer needed

	return 0;
}



#ifdef WIN32
  #ifdef _DEBUG
    #pragma comment(lib, "glew32d.lib")
  #else
    #pragma comment(lib, "glew32.lib")
  #endif
#endif

#ifdef WIN32
char shaderBasePath[1024] = SHADER_BASE_PATH;
#else
char shaderBasePath[1024] = "../openGLHelper-starterCode";
#endif



#ifdef WIN32
char texshaderBasePath[1024] = SHADER_BASE_PATH"/tex";
#else
char texshaderBasePath[1024] = "../openGLHelper-starterCode/tex";
#endif

using namespace std;

int mousePos[2]; // x,y coordinate of the mouse position

int leftMouseButton = 0; // 1 if pressed, 0 if not 
int middleMouseButton = 0; // 1 if pressed, 0 if not
int rightMouseButton = 0; // 1 if pressed, 0 if not

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROL_STATE;
CONTROL_STATE controlState = ROTATE;

// state of the world


bool ride = 0;
double rideprogress = 0;
Point ridePos;
Point rideDir;
Point rideNormal;

float landRotate[3] = { 0.0f, 0.0f, 0.0f };
float landTranslate[3] = { -3.0f, 0.0f, 0.2f };

int windowWidth = 1280;
int windowHeight = 720;
char windowTitle[512] = "CSCI 420 homework I";


///number of indices in index buffer
int indices = 0;

///number of vertices in vertex buffer
int vertices = 0;

int verticesSky = 0;

int indicesSky = 0;

///opengl texture
GLuint texture = 0;
GLuint textureSky = 0;
GLuint textureRail = 0;
/// GROUND

///opengl vertex array object id
GLuint vboVertices = 0;
GLuint vboVerticesSky = 0;

GLuint vboIndicesSky = 0;

///opengl index buffer, used for triangle and wireframe
GLuint vao = 0;
GLuint vaoSky = 0;




/// Spline

///number of indices in index buffer
int splineIndices = 0;

///number of vertices in vertex buffer
int splineVertices = 0;

///opengl vertex array object id
GLuint vboVerticesSpline = 0;
///opengl vertex buffer
GLuint vboIndicesSpline = 0;
///opengl index buffer, used for triangle and wireframe
GLuint vaoSpline = 0;




//shader gradient
BasicPipelineProgram shader;

//shader textured
BasicPipelineProgram texshader;

///matrix maniplation
OpenGLMatrix matrix;

///control the drawing mode - fill, wirefram or points

enum drwmode { Fill=0, Wireframe, Points};
int drawmode = Fill;


///control textured or gradient colors
bool drawtex=false;

///request to make screnshots each frame
int makescreenshots = 0;
/// sequential number of auto screenshot
int screenshotindex = 0;

// write a screenshot to the specified filename
void saveScreenshot(const char * filename)
{
  unsigned char * screenshotData = new unsigned char[windowWidth * windowHeight * 3];
  glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, screenshotData);

  ImageIO screenshotImg(windowWidth, windowHeight, 3, screenshotData);

  if (screenshotImg.save(filename, ImageIO::FORMAT_JPEG) == ImageIO::OK)
    cout << "File " << filename << " saved successfully." << endl;
  else cout << "Failed to save file " << filename << '.' << endl;

  delete [] screenshotData;
}

void displayFunc()
{

	{///fix the modelbview matrix according to teh current state of rotation translation and scale

		float sh = sin(landRotate[0] * M_PI / 180);
		float ch = cos(landRotate[0] * M_PI / 180);
		float sv = sin(landRotate[1] * M_PI / 180);
		float cv = cos(landRotate[1] * M_PI / 180);

		

		matrix.SetMatrixMode(OpenGLMatrix::ModelView);
		if (ride)
		{
			struct Point Up = { 0, 0, 1 };
			//struct Point Up = rideNormal;

			struct Point rideLeft = normalize( cross((rideDir), Up));
			struct Point rideUp = normalize (cross(rideLeft, rideDir));
			float m[16] =
			{
				rideLeft.x, rideUp.x, rideDir.x, 0,
				rideLeft.y, rideUp.y, rideDir.y, 0,
				rideLeft.z, rideUp.z, rideDir.z, 0,
				0, 0, 0, 1,
			};

			double h = 0.3;
			matrix.LoadMatrix(m);
			matrix.Translate(-rideUp.x*h - ridePos.x, -rideUp.y*h - ridePos.y, -rideUp.z*h - ridePos.z);

		}
		else
		{
			float m[16] =
			{
				sh, -sv*ch, cv*ch, 0,
				-ch, -sv*sh, cv*sh, 0,
				0, cv, sv, 0,
				0, 0, 0, 1,
			};
			
			matrix.LoadMatrix(m);
			matrix.Translate(-landTranslate[0], -landTranslate[1], -landTranslate[2]);
		}}
	
	// render some stuff...
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	texshader.Bind();

	{///set modelview matrix to GPU
		float m[16];
		matrix.SetMatrixMode(OpenGLMatrix::ModelView);
		matrix.GetMatrix(m);
		texshader.SetModelViewMatrix(m);
	}
	{///set projection matrix to GPU
		float m[16];
		matrix.SetMatrixMode(OpenGLMatrix::Projection);
		matrix.GetMatrix(m);
		texshader.SetProjectionMatrix(m);
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(vao);///draw a triangle fan
	glDrawArrays(GL_TRIANGLE_FAN, 0, vertices);
	
	glBindTexture(GL_TEXTURE_2D, textureSky);
	glBindVertexArray(vaoSky);///draw a triangle fan
	glDrawElements(GL_QUADS, indicesSky, GL_UNSIGNED_INT, (void*)0);
	// glDrawArrays(GL_QUADS, 0, verticesSky);
	
	/*
	shader.Bind();

	{///set modelview matrix to GPU
		float m[16];
		matrix.SetMatrixMode(OpenGLMatrix::ModelView);
		matrix.GetMatrix(m);
		shader.SetModelViewMatrix(m);
	}
	{///set projection matrix to GPU
		float m[16];
		matrix.SetMatrixMode(OpenGLMatrix::Projection);
		matrix.GetMatrix(m);
		shader.SetProjectionMatrix(m);
	}*/
	glBindTexture(GL_TEXTURE_2D, textureRail);
	glBindVertexArray(vaoSpline);
	glDrawElements(GL_QUAD_STRIP, splineIndices, GL_UNSIGNED_INT, (void*)0);

	glutSwapBuffers();
}

void idleFunc()
{
}


///make the animation in the timer func
void timerFunc(int value)
{
	
	if (ride && numSplines)
	{
		int n = splines[0].numControlPoints;

		double	rideSpeed = 0.075;
		
		if (n <= rideprogress)rideprogress = 0;

		int j = rideprogress;
		double t = rideprogress - j;

		
		double l = dist(splines[0].points[(j+1)%n], splines[0].points[(j + 2) %n]);

		rideprogress += rideSpeed/l;

		if (n <= rideprogress)rideprogress = 0;



		ridePos = CRSpline(
			splines[0].points[(j) % n], splines[0].points[(j + 1) % n],
			splines[0].points[(j + 2) % n], splines[0].points[(j + 3) % n], t);

		rideDir = normalize(dCRSpline(
			splines[0].points[(j) % n], splines[0].points[(j + 1) % n],
			splines[0].points[(j + 2) % n], splines[0].points[(j + 3) % n], t));

		rideNormal = normalize(ddCRSpline(
			splines[0].points[(j) % n], splines[0].points[(j + 1) % n],
			splines[0].points[(j + 2) % n], splines[0].points[(j + 3) % n], t));


	}

	// for example, here, you can save the screenshots to disk (to make the animation)
	if (makescreenshots)
	{
		makescreenshots--;
		screenshotindex++;
		// write a screenshot to the specified filename

		char filename[256];
		sprintf(filename, "screnshot_%03d.jpg", screenshotindex);
		saveScreenshot(filename);
	}
	// make the screen update 
	glutPostRedisplay();
	glutTimerFunc(33, timerFunc,0);//target 30fps
}
void reshapeFunc(int w, int h)
{
  glViewport(0, 0, w, h);

  // setup perspective matrix...

  {////fix proection matrix
	  matrix.SetMatrixMode(OpenGLMatrix::Projection);
	  matrix.LoadIdentity();
	  matrix.Perspective(45, float(w) / h, 0.5, 1000);
	  matrix.Scale(1,1,-1);
  }
}

void mouseMotionDragFunc(int x, int y)
{
  // mouse has moved and one of the mouse buttons is pressed (dragging)

  // the change in mouse position since the last invocation of this function
  int mousePosDelta[2] = { x - mousePos[0], y - mousePos[1] };

  switch (controlState)
  {
    // translate the landscape
    case TRANSLATE:
      if (leftMouseButton)
      {
        // control x,y translation via the left mouse button
      }
      if (middleMouseButton)
      {
        // control z translation via the middle mouse button
      }
      break;

    // rotate the landscape
    case ROTATE:
      if (leftMouseButton)
      {
        // control x,y rotation via the left mouse button
        landRotate[0] -= mousePosDelta[0];
        landRotate[1] -= mousePosDelta[1];
      }
      if (middleMouseButton)
      {
        // control z rotation via the middle mouse button
        landRotate[2] -= mousePosDelta[1];
      }
      break;

    // scale the landscape
    case SCALE:
      break;
  }

  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void mouseMotionFunc(int x, int y)
{
  // mouse has moved
  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void mouseButtonFunc(int button, int state, int x, int y)
{
  // a mouse button has has been pressed or depressed

  // keep track of the mouse button state, in leftMouseButton, middleMouseButton, rightMouseButton variables
  switch (button)
  {
    case GLUT_LEFT_BUTTON:
      leftMouseButton = (state == GLUT_DOWN);
    break;

    case GLUT_MIDDLE_BUTTON:
      middleMouseButton = (state == GLUT_DOWN);
    break;

    case GLUT_RIGHT_BUTTON:
      rightMouseButton = (state == GLUT_DOWN);
    break;
  }

  // keep track of whether CTRL and SHIFT keys are pressed
  switch (glutGetModifiers())
  {
    case GLUT_ACTIVE_CTRL:
      controlState = TRANSLATE;
    break;

    case GLUT_ACTIVE_SHIFT:
      controlState = SCALE;
    break;

    // if CTRL and SHIFT are not pressed, we are in rotate mode
    default:
      controlState = ROTATE;
    break;
  }

  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void keyboardFunc(unsigned char key, int x, int y)
{
	float sh = sin(landRotate[0] * M_PI / 180);
	float ch = cos(landRotate[0] * M_PI / 180);
	float sv = sin(landRotate[1] * M_PI / 180);
	float cv = cos(landRotate[1] * M_PI / 180);

	float speed = 0.1;

  switch (key)
  {
    case 27: // ESC key
      exit(0); // exit the program
    break;

	case ' ':
		///change the drawing mode
		ride = !ride;
		rideprogress = 0;
		break;

	case 'w':
		if (!ride)
		{
			landTranslate[0] += cv*ch*speed;
			landTranslate[1] += cv*sh*speed;
			landTranslate[2] += sv*speed;
		}
		break;

	case 's':
		if (!ride)
		{
			landTranslate[0] -= cv*ch*speed;
			landTranslate[1] -= cv*sh*speed;
			landTranslate[2] -= sv*speed;
		}
		break;

	case 'a':
		if (!ride)
		{
			landTranslate[0] -= sh*speed;
			landTranslate[1] += ch*speed;
		}
		break;

	case 'd':
		if (!ride)
		{
			landTranslate[0] += sh*speed;
			landTranslate[1] -= ch*speed;
		}
		break;







	case 'x':
		// take a screenshot
		saveScreenshot("screenshot.jpg");
		break;
	
	case 'z':
		// take screenshots
		makescreenshots = 200;
		screenshotindex = 0;
		break;
	
  }
}

void initScene(const char* heighfile, const char * texturefile, const char * textureSkyfile, const char * textureRailfile)
{

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &texture);
	initTexture(texturefile, texture);

	glGenTextures(1, &textureSky);
	initTexture(textureSkyfile, textureSky);

	glGenTextures(1, &textureRail);
	initTexture(textureRailfile, textureRail,true);


  // load the image from a jpeg disk file to main memory
	ImageIO * heightmapImage = new ImageIO();
	if (heightmapImage->loadJPEG(heighfile) != ImageIO::OK)
  {
	  cout << "Error reading image " << heighfile << "." << endl;
    exit(EXIT_FAILURE);
  }

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  // do additional initialization here...

	//enagle the depth test
  glEnable(GL_DEPTH_TEST);

  {//init vertex buffer and index buffer

	  glGenVertexArrays(1, &vao);
	  glBindVertexArray(vao);


	  glGenBuffers(1, &vboVertices);
	  glBindBuffer(GL_ARRAY_BUFFER, vboVertices);

	  ///one vertex for each pixel of image
	  int sectors = 32;
	  double r = 100, z = -2;
	  vertices = 1 + sectors + 1;

	  VertexData * vdata = new VertexData[vertices];

	  vdata[0].pos.x = 0;
	  vdata[0].pos.y = 0;
	  vdata[0].pos.z = z;

	  vdata[0].texCoord.x = 0;
	  vdata[0].texCoord.y = 0;


	  for (int s = 0; s <= sectors; s++)
	  {
		  vdata[s + 1].pos.x = cos(s * 2 * M_PI / sectors)*r;
		  vdata[s + 1].pos.y = sin(s * 2 * M_PI / sectors)*r;
		  vdata[s + 1].pos.z = z;

		  vdata[s + 1].texCoord.x = cos(s * 2 * M_PI / sectors)*r;
		  vdata[s + 1].texCoord.y = sin(s * 2 * M_PI / sectors)*r;
	  }

	  glBufferData(GL_ARRAY_BUFFER, vertices * sizeof(VertexData), vdata, GL_STATIC_DRAW);

	  glEnableVertexAttribArray(1);
	  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)((char*)&vdata->texCoord - (char*)vdata));


	  glEnableVertexAttribArray(0);
	  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 0);


	  delete[] vdata;
  }



	{//init vertex buffer and index buffer

		glGenVertexArrays(1, &vaoSky);
		glBindVertexArray(vaoSky);


		glGenBuffers(1, &vboVerticesSky);
		glBindBuffer(GL_ARRAY_BUFFER, vboVerticesSky);

		glGenBuffers(1, &vboIndicesSky);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndicesSky);

		///one vertex for each pixel of image
		double r = 100,z=-2;
		

		int sectors = 32;
		int segments = 8;
		verticesSky = (sectors + 1)*(segments+1);
		indicesSky = (sectors*segments)*4;

		VertexData * vdata = new VertexData[verticesSky];
		GLuint * idata = new GLuint[indicesSky];
		int ipos = 0;
		int pos = 0;
		for (int s = 0; s <= sectors; s++)
			for (int t = 0; t <=segments; t++)
			{
				vdata[pos].pos.x = cos(t *  M_PI/2 / segments)*cos(s * 2 * M_PI / sectors)*r;
				vdata[pos].pos.y = cos(t *  M_PI/2 / segments)*sin(s * 2 * M_PI / sectors)*r;
				vdata[pos].pos.z = z+sin(t * M_PI/2 / segments)*r;

				vdata[pos].texCoord.x = s*1.0 / sectors;
				vdata[pos].texCoord.y = t*1.0 / segments;


				if (s&&t)
				{
					idata[ipos] = pos - 1;
					ipos++;
					idata[ipos] = pos;
					ipos++;
					idata[ipos] = pos - segments-1;
					ipos++;
					idata[ipos] = pos - segments-1 - 1;
					ipos++;
				}

				pos++;
			}


		glBufferData(GL_ARRAY_BUFFER, verticesSky * sizeof(VertexData), vdata, GL_STATIC_DRAW);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSky * sizeof(GLuint), idata, GL_STATIC_DRAW);


		delete[] vdata;
		delete[] idata;


		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)((char*)&vdata->texCoord - (char*)vdata));

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 0);


	}


	

	{//init vertex buffer and index buffer

		glGenVertexArrays(1, &vaoSpline);
		glBindVertexArray(vaoSpline);


		glGenBuffers(1, &vboVerticesSpline);
		glBindBuffer(GL_ARRAY_BUFFER, vboVerticesSpline);

		glGenBuffers(1, &vboIndicesSpline);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndicesSpline);

		double maxpartlength = 0.2;

		for (int j = 0; j < numSplines; j++)
		{
			int points = 0;

			int n = splines[j].numControlPoints;

			for (int i = 0; i < splines[j].numControlPoints; i++)
			{
				double l = dist(splines[j].points[(i+1)%n], splines[j].points[(i+2)%n]);
				int subpoints = l / maxpartlength;
				points +=  1 + subpoints;
			}//next point and middle points

			splineVertices += points;
			splineIndices += points*2;
		}


		int verticesOffset = splineVertices;
		splineVertices *= 2;

		VertexData * vdata = new VertexData[splineVertices];
		GLuint * idata = new GLuint[splineIndices];
		int pos = 0;
		int ipos = 0;

		double d = 0.1;


		double sz = 0.1;
		double step = 0;

		for (int j = 0; j < numSplines; j++)
		{

			int n = splines[j].numControlPoints;

			for (int i = 0; i < splines[j].numControlPoints; i++)
			{
				struct Point prevpt = splines[j].points[(i + 1) % n];
				struct Point currpt = splines[j].points[(i + 2) % n];

				double l = dist(currpt,prevpt);
				int subpoints = l / maxpartlength;

				for (int p = 0; p <=subpoints; p++)
				{
					double t = (1.0+p) / (subpoints + 1);
					struct Point c = CRSpline(
						splines[j].points[(i) % n], splines[j].points[(i + 1) % n],
						splines[j].points[(i + 2) % n], splines[j].points[(i + 3) % n], t);


					
					idata[ipos] = pos;
					ipos++;

					idata[ipos] = pos+verticesOffset;
					ipos++;



					struct Point rideNormal = { 0, 0, 1 };
					struct Point f = dCRSpline(
						splines[j].points[(i) % n], splines[j].points[(i + 1) % n],
						splines[j].points[(i + 2) % n], splines[j].points[(i + 3) % n], t);


					struct Point rideLeft = normalize(cross(f, rideNormal));

					vdata[pos].pos = toGPoint(add(c, mul(rideLeft, sz)));
					vdata[pos].texCoord.x = 0;
					vdata[pos].texCoord.y = step;


					vdata[pos + verticesOffset].pos = toGPoint(sub(c, mul(rideLeft, sz)));
					vdata[pos + verticesOffset].texCoord.x = 1;
					vdata[pos + verticesOffset].texCoord.y = step;
			
					step += 0.2;

					pos++;
				}
			}
		}



		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)((char*)&vdata->texCoord - (char*)vdata));

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 0);

		glBufferData(GL_ARRAY_BUFFER, splineVertices * sizeof(VertexData), vdata, GL_STATIC_DRAW);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, splineIndices * sizeof(GLuint), idata, GL_STATIC_DRAW);



		delete[] vdata;

		delete[] idata;
	}


	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5);

  {//initialise and use the shader
	  shader.Init(shaderBasePath);

	  glBindAttribLocation(shader.GetProgramHandle(), 0, "vpos");
	  glLinkProgram(shader.GetProgramHandle());
  }

  {//initialise and use the shader
	  texshader.Init(texshaderBasePath);
	  glBindAttribLocation(texshader.GetProgramHandle(), 0, "vpos");
	  glBindAttribLocation(texshader.GetProgramHandle(), 1, "vtex");
	  glLinkProgram(texshader.GetProgramHandle());
  }

  delete heightmapImage;
}

int main(int argc, char *argv[])
{
	if (argc<2)
	{
		printf("usage: %s <trackfile>\n", argv[0]);
		exit(0);
	}

	// load the splines from the provided filename
	loadSplines(argv[1]);

	printf("Loaded %d spline(s).\n", numSplines);
	for (int i = 0; i<numSplines; i++)
		printf("Num control points in spline %d: %d.\n", i, splines[i].numControlPoints);

  cout << "Initializing GLUT..." << endl;
  glutInit(&argc,argv);

  cout << "Initializing OpenGL..." << endl;

  #ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  #else
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  #endif

  glutInitWindowSize(windowWidth, windowHeight);
  glutInitWindowPosition(0, 0);  
  glutCreateWindow(windowTitle);

  cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;
  cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << endl;
  cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

  // tells glut to use a particular display function to redraw 
  glutDisplayFunc(displayFunc);
  // perform animation inside idleFunc
  glutIdleFunc(idleFunc);
  // callback for mouse drags
  glutMotionFunc(mouseMotionDragFunc);
  // callback for idle mouse movement
  glutPassiveMotionFunc(mouseMotionFunc);
  // callback for mouse button changes
  glutMouseFunc(mouseButtonFunc);
  // callback for resizing the window
  glutReshapeFunc(reshapeFunc);
  // callback for pressing the keys on the keyboard
  glutKeyboardFunc(keyboardFunc);
  // callback to request frames on timer
  glutTimerFunc(33, timerFunc,0);//target 30fps


  // init glew
  #ifdef __APPLE__
    // nothing is needed on Apple
  #else
    // Windows, Linux
    GLint result = glewInit();
    if (result != GLEW_OK)
    {
      cout << "error: " << glewGetErrorString(result) << endl;
      exit(EXIT_FAILURE);
    }
  #endif

  // do initialization
	initScene("./heightmap/Heightmap.jpg", "./grass.jpg", "./sky.jpg", "./rail.jpg");

  // sink forever into the glut loop
  glutMainLoop();
}


