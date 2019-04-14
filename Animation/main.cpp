// Based loosly on the first triangle OpenGL tutorial
// http://www.opengl.org/wiki/Tutorial:_OpenGL_3.1_The_First_Triangle_%28C%2B%2B/Win%29
// This program will render two triangles
// Most of the OpenGL code for dealing with buffer objects, etc has been moved to a 
// utility library, to make creation and display of mesh objects as simple as possible

// Windows specific: Uncomment the following line to open a console window for debug output
#if _DEBUG
#pragma comment(linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

#include "rt3d.h"
#include "rt3dObjLoader.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp> 
#include <stack>
#include "md2model.h"
#include "AABB.h"
#include <SDL.h>
#include "bass.h"
#include "Audio.h"
#include <stdlib.h>  

using namespace std;

#define DEG_TO_RADIAN 0.017453293

/// SKYBOX ///
glm::vec3 SkyBoxVertices[36] =
{
	glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, -1.0f,  1.0f), glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(1.0f, -1.0f, -1.0f),
	glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(-1.0f, -1.0f,  1.0f),
	glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(-1.0f,  1.0f, -1.0f),
	glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(1.0f, -1.0f,  1.0f), glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, -1.0f,  1.0f),
	glm::vec3(1.0f, -1.0f,  1.0f), glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(1.0f, -1.0f,  1.0f),
	glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, -1.0f)
};
unsigned int cubemapTexture;
GLuint shaderSkybox;

/// MINI MAP//
GLuint MiniMapShader;
GLuint FBO;
unsigned int SceneTexture;		
GLuint CubeVertCount = 36;

GLfloat CubeVerts[] = {
	0, 0, 0,        1, 1, 0,        1, 0, 0,        0, 0, 0,        0, 1, 0,
	1, 1, 0,        0, 0, 0,        0, 1, 1,        0, 1, 0,        0, 0, 0,
	0, 0, 1,        0, 1, 1,        0, 1, 0,        1, 1, 1,        1, 1, 0,
	0, 1, 0,        0, 1, 1,        1, 1, 1,        1, 0, 0,        1, 1, 0,
	1, 1, 1,        1, 0, 0,        1, 1, 1,        1, 0, 1,        0, 0, 0,
	1, 0, 0,        1, 0, 1,        0, 0, 0,        1, 0, 1,        0, 0, 1,
	0, 0, 1,        1, 0, 1,        1, 1, 1,        0, 0, 1,        1, 1, 1,
	0, 1, 1

};

GLfloat CubeNorms[] = {
	0, 0, -1,       0, 0, -1,       0, 0, -1,       0, 0, -1,       0, 0, -1,
	0, 0, -1,       -1, 0, 0,       -1, 0, 0,       -1, 0, 0,       -1, 0, 0,
	-1, 0, 0,       -1, 0, 0,       0, 1, 0,        0, 1, 0,        0, 1, 0,
	0, 1, 0,        0, 1, 0,        0, 1, 0,        1, 0, 0,        1, 0, 0,
	1, 0, 0,        1, 0, 0,        1, 0, 0,        1, 0, 0,        0, -1, 0,
	0, -1, 0,       0, -1, 0,       0, -1, 0,       0, -1, 0,       0, -1, 0,
	0, 0, 1,        0, 0, 1,        0, 0, 1,        0, 0, 1,        0, 0, 1,
	0, 0, 1

};

GLfloat CubeColours[] = { 0.0f, 0.0f, 0.0f,
0.0f, 1.0f, 0.0f,
1.0f, 1.0f, 0.0f,
1.0f, 0.0f, 0.0f,
0.0f, 0.0f, 1.0f,
0.0f, 1.0f, 1.0f,
1.0f, 1.0f, 1.0f,
1.0f, 0.0f, 1.0f
};

GLfloat cubeTexCoords[] = { 0, 0,   1, 1,   1, 0,   0, 0,   0, 1,   1, 1,   0, 0,   1, 1,   1, 0,   0, 0,
0, 1,   1, 1,   0, 0,   1, 1,   1, 0,   0, 0,   0, 1,   1, 1,   0, 0,   0, 1,
1, 1,   0, 0,   1, 1,   1, 0,   0, 0,   1, 0,   1, 1,   0, 0,   1, 1,   0, 1,
0, 0,   1, 0,   1, 1,   0, 0,   1, 1,   0, 1

};

GLuint cubeIndexCount = 36;

GLuint cubeIndices[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22
, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35

};

/// MINI MAP END ///

/// GLOBALS ///
GLuint meshIndexCount = 0;
GLuint md2VertCount = 0;
GLuint md2VertCount1 = 0;
GLuint meshObjects[5];
GLuint shaderProgram;
bool freeCamera = false;

//Hobgoblin
glm::vec3 hobgoblinPos(-15, 1.2f, 0);
glm::vec3 Car(0.0, 0.2, 0.0);

//Starts Standing

//Player Camera
//glm::vec3 eye(hobgoblinPos.x-5.0f, hobgoblinPos.y+1.5f, hobgoblinPos.z);
//glm::vec3 at(0.0f, 0.0f, 0.0f);
//glm::vec3 up(0.0f, 1.0f, 0.0f);
//glm::vec3 eyeReturn(0.0f, 0.0f, 0.0f);
//GLfloat r = 90.0f;
//GLfloat rReturn = 0.0f;

glm::vec3 eye(Car.x - 30.0f, Car.y + 11.5f, Car.z);
glm::vec3 at(0.0f, 0.0f, 0.0f);
glm::vec3 up(0.0f, 1.0f, 0.0f);
glm::vec3 eyeReturn(0.0f, 0.0f, 0.0f);
GLfloat r = 90.0f;
GLfloat rReturn = 0.0f;

////Minimap
//glm::vec3 Minieye(hobgoblinPos.x, hobgoblinPos.y + 19.0f, hobgoblinPos.z);
//glm::vec3 Miniat(hobgoblinPos.x, hobgoblinPos.y, hobgoblinPos.z);
//glm::vec3 Miniup(0.0f, 1.0f, 0.0f);


stack<glm::mat4> mvStack; 

/// TEXTURE ///
GLuint textures[3];

rt3d::lightStruct light0 = {
	{0.1f, 0.1f, 0.1f, 1.0f}, // ambient
	{1.0f, 1.0f, 1.0f, 1.0f}, // diffuse
	{1.0f, 1.0f, 1.0f, 1.0f}, // specular
	{0.0f, 0.0f, 0.0f, 1.0f}  // position
};
glm::vec4 lightPos(0, 1.0f, 0, 1.0f); //light position

rt3d::materialStruct material0 = {
	{0.2f, 0.4f, 0.2f, 1.0f}, // ambient
	{0.5f, 1.0f, 0.5f, 1.0f}, // diffuse
	{0.0f, 0.1f, 0.0f, 1.0f}, // specular
	2.0f  // shininess
};
rt3d::materialStruct material1 = {
	{0.4f, 0.4f, 1.0f, 1.0f}, // ambient
	{0.8f, 0.8f, 1.0f, 1.0f}, // diffuse
	{0.8f, 0.8f, 0.8f, 1.0f}, // specular
	1.0f  // shininess
};

/// MD2 ///
md2model tmpModel;
md2model CarModel;
md2model BuildingModel;
md2model SmallBuildingModel;
int currentAnim = 0;

/// AUDIO ///

//HSAMPLE *samples = NULL;
//
//HSAMPLE loadSample(char * filename)
//{
//	HSAMPLE sample;
//	if (sample = BASS_SampleLoad(FALSE, filename, 0, 0, 3, BASS_SAMPLE_OVER_POS))
//		cout << "sample " << filename << " loaded!" << endl;
//	else
//	{
//		cout << "Can't load sample";
//		exit(0);
//	}
//	return sample;
//}


/// Set up rendering context ///
SDL_Window * setupRC(SDL_GLContext &context) {
	SDL_Window * window;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) // Initialize video
        rt3d::exitFatalError("Unable to initialize SDL"); 
	  
    // Request an OpenGL 3.0 context.
	
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); 

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);  // double buffering on
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); // 8 bit alpha buffering
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // Turn on x4 multisampling anti-aliasing (MSAA)
 
    // Create 800x600 window
    window = SDL_CreateWindow("SDL/GLM/OpenGL Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
	if (!window) // Check window was created OK
        rt3d::exitFatalError("Unable to create window");
 
    context = SDL_GL_CreateContext(window); // Create opengl context and attach to window
    SDL_GL_SetSwapInterval(1); // set swap buffers to sync with monitor's vertical refresh rate
	return window;
}

/// Texture loading function ///
GLuint loadBitmap(char *fname) {
	GLuint texID;
	glGenTextures(1, &texID); // generate texture ID

	// load file - using core SDL library
 	SDL_Surface *tmpSurface;
	tmpSurface = SDL_LoadBMP(fname);
	if (!tmpSurface) {
		std::cout << "Error loading bitmap" << std::endl;
	}

	// bind texture and set parameters
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	SDL_PixelFormat *format = tmpSurface->format;
	
	GLuint externalFormat, internalFormat;
	if (format->Amask) {
		internalFormat = GL_RGBA;
		externalFormat = (format->Rmask < format-> Bmask) ? GL_RGBA : GL_BGRA;
	}
	else {
		internalFormat = GL_RGB;
		externalFormat = (format->Rmask < format-> Bmask) ? GL_RGB : GL_BGR;
	}

	glTexImage2D(GL_TEXTURE_2D,0,internalFormat,tmpSurface->w, tmpSurface->h, 0,
		externalFormat, GL_UNSIGNED_BYTE, tmpSurface->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	SDL_FreeSurface(tmpSurface); // texture loaded, free the temporary buffer
	return texID;	// return value of texture ID
}


unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		SDL_Surface *tmpSurface;
		tmpSurface = SDL_LoadBMP(faces[i].c_str());
		if (!tmpSurface) {
			std::cout << "Error loading bitmap" << std::endl;
		}

		SDL_PixelFormat *format = tmpSurface->format;

		GLuint externalFormat, internalFormat;
		if (format->Amask) {
			internalFormat = GL_RGBA;
			externalFormat = (format->Rmask < format->Bmask) ? GL_RGBA : GL_BGRA;
		}
		else {
			internalFormat = GL_RGB;
			externalFormat = (format->Rmask < format->Bmask) ? GL_RGB : GL_BGR;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, tmpSurface->w, tmpSurface->h, 0, externalFormat, GL_UNSIGNED_BYTE, tmpSurface->pixels);
		SDL_FreeSurface(tmpSurface); // texture loaded, free the temporary buffer

	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}



/// INITIALISE ///
void init(void) {

	shaderSkybox = rt3d::initShaders("skybox.vert", "skybox.frag"); 
	shaderProgram = rt3d::initShaders("phong-tex.vert","phong-tex.frag");
	
	glUseProgram(shaderProgram);
	rt3d::setLight(shaderProgram, light0);
	rt3d::setMaterial(shaderProgram, material0);

	vector<GLfloat> verts;
	vector<GLfloat> norms;
	vector<GLfloat> tex_coords;
	vector<GLuint> indices;
	rt3d::loadObj("cube.obj", verts, norms, tex_coords, indices);
	GLuint size = indices.size();
	meshIndexCount = size;
	textures[0] = loadBitmap("fabric.bmp");
	meshObjects[0] = rt3d::createMesh(verts.size()/3, verts.data(), nullptr, norms.data(), tex_coords.data(), size, indices.data());


	//rt3d::loadObj("BuildingObjTest.obj", verts, tex_coords, norms, indices);
	//textures[4] = loadBitmap("Building2Texture1.bmp");
	//meshObjects[4] = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, tex_coords.data(), norms.data(), size, indices.data());


	//////////skybox
	glUseProgram(shaderSkybox);
	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, 3 * 3 * 2 * 6 * sizeof(GLfloat), &SkyBoxVertices, GL_STATIC_DRAW); // 3 coords, 3 verts, 2 tris per face, 6 faces
	glVertexAttribPointer((GLuint)RT3D_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0); // make sure that the position in the shade is called in_Position
	glEnableVertexAttribArray(0);
	meshObjects[3] = skyboxVAO;

	// skybox textures
	vector<std::string> faces
	{
		std::string("./resources/textures/skybox/right.bmp"),
		std::string("./resources/textures/skybox/left.bmp"),
		std::string("./resources/textures/skybox/top.bmp"),
		std::string("./resources/textures/skybox/bottom.bmp"),
		std::string("./resources/textures/skybox/front.bmp"),
		std::string("./resources/textures/skybox/back.bmp")
	};
	cubemapTexture = loadCubemap(faces);


	textures[1] = loadBitmap("hobgoblin2.bmp");
	meshObjects[1] = tmpModel.ReadMD2Model("tris.MD2");
	md2VertCount = tmpModel.getVertDataCount();

	textures[2] = loadBitmap("Car.bmp");
	meshObjects[2] = CarModel.ReadMD2Model("CarModel.MD2");
	md2VertCount1 = CarModel.getVertDataCount();


	meshObjects[4] = BuildingModel.ReadMD2Model("./resources/md2models/Buildings/Building.MD2");
	meshObjects[5] = SmallBuildingModel.ReadMD2Model("./resources/md2models/Buildings/SmallBuilding.MD2");
	md2VertCount1 = BuildingModel.getVertDataCount();
	md2VertCount1 = SmallBuildingModel.getVertDataCount();
	textures[4] = loadBitmap("./resources/textures/Building/Building1Texture1.bmp");
	textures[5] = loadBitmap("./resources/textures/Building/BuildingCatTexture.bmp");
	textures[6] = loadBitmap("./resources/textures/Building/Building3Texture.bmp");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d) {
	return glm::vec3(pos.x + d*std::sin(r*DEG_TO_RADIAN), pos.y, pos.z - d*std::cos(r*DEG_TO_RADIAN));
}

glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d) {
	return glm::vec3(pos.x + d*std::cos(r*DEG_TO_RADIAN), pos.y, pos.z + d*std::sin(r*DEG_TO_RADIAN));
}


void rotateCameraAroundPlayer(float angle) {
	r += angle;
	eye.x -= Car.x;
	eye.z -= Car.z;

	float newX = eye.x*std::cos(-angle*DEG_TO_RADIAN) + eye.z*std::sin(-angle*DEG_TO_RADIAN);
	float newZ = -eye.x*std::sin(-angle*DEG_TO_RADIAN) + eye.z*std::cos(-angle*DEG_TO_RADIAN);

	eye.x = (newX + Car.x);
	eye.z = (newZ + Car.z);
}

void NPCMovement()
{
	glm::vec3(hobgoblinPos.x += 0.2, hobgoblinPos.y, hobgoblinPos.z);	
	
	glm::vec3(hobgoblinPos.x -= 0.2, hobgoblinPos.y, hobgoblinPos.z);
}

void update(void) {

	NPCMovement();
	

const	Uint8 *keys = SDL_GetKeyboardState(NULL);

if (keys[SDL_SCANCODE_9])
{
	Audio::LoadSample("Audio/GTARadio.wav");
}

if (keys[SDL_SCANCODE_8])
{
	Audio::LoadSample("Audio/CarAudio.wav");
}
if (keys[SDL_SCANCODE_P]) {
	BASS_Pause();
}

if (keys[SDL_SCANCODE_R]) {
	BASS_Start();
}

if (keys[SDL_SCANCODE_W])
{
	Audio::LoadSample("Audio/CarAudio.wav");
	eye = moveForward(eye, r, 0.1f);
}

if (keys[SDL_SCANCODE_S])
{
	eye = moveForward(eye, r, -0.1f);
	Audio::LoadSample("Audio/CarAudio.wav");
}

if (freeCamera)
{
	if (keys[SDL_SCANCODE_W]) { eye = moveForward(eye, r, 0.1f); ; }
	if (keys[SDL_SCANCODE_S]) { eye = moveForward(eye, r, -0.1f); }



	if (keys[SDL_SCANCODE_A]) { eye = moveRight(eye, r, 0.1f); r -= 1.0f;  }
	if (keys[SDL_SCANCODE_D]) { eye = moveRight(eye, r, -0.1f); r += 1.0f;}


	if (keys[SDL_SCANCODE_Q]) { eye.y += 0.1f; }
	if (keys[SDL_SCANCODE_E]) { eye.y -= 0.1f; }
	if (keys[SDL_SCANCODE_R]) { freeCamera = false; eye = eyeReturn; r = rReturn; }
	if (keys[SDL_SCANCODE_COMMA]) { r -= 1.0f; }
	if (keys[SDL_SCANCODE_PERIOD]) { r += 1.0f; }
}
else
{

	if (keys[SDL_SCANCODE_W]) { Car = moveForward(Car, r, 0.1f); eye = moveForward(eye, r, 0.1f); currentAnim = 1; }
	if (!keys[SDL_SCANCODE_W]) { if (currentAnim  = 1) { currentAnim = 0; } }
	if (keys[SDL_SCANCODE_S]) { Car = moveForward(Car, r, -0.1f); eye = moveForward(eye, r, -0.1f); }
	if (keys[SDL_SCANCODE_A]) { rotateCameraAroundPlayer(-1.5f); }
	if (keys[SDL_SCANCODE_D]) { rotateCameraAroundPlayer(1.5f); }
	if (keys[SDL_SCANCODE_R]) { eyeReturn = eye; rReturn = r; freeCamera = true; }
}


	if ( keys[SDL_SCANCODE_1] ) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
	}
	if ( keys[SDL_SCANCODE_2] ) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
	}
	if ( keys[SDL_SCANCODE_Z] ) {
		if (--currentAnim < 0) currentAnim = 19;
		cout << "Current animation: " << currentAnim << endl;
	}
	if ( keys[SDL_SCANCODE_X] ) {
		if (++currentAnim >= 20) currentAnim = 0;
		cout << "Current animation: " << currentAnim << endl;
	}
}


void draw(SDL_Window * window) {
	// clear the screen
	glEnable(GL_CULL_FACE);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);
	glm::mat4 projection(1.0);
	projection = glm::perspective(float(60.0f*DEG_TO_RADIAN), 800.0f / 600.0f, 1.0f, 50000.0f);
	rt3d::setUniformMatrix4fv(shaderProgram, "projection", glm::value_ptr(projection));


	GLfloat scale(1.0f); // just to allow easy scaling of complete scene

	glm::mat4 modelview(1.0); // set base position for scene
	mvStack.push(modelview);

	at = moveForward(eye, r, 1.0f);
	mvStack.top() = glm::lookAt(eye, at, up);


	///////////Skybox
	glDisable(GL_CULL_FACE); // disable culling and depth test
	glDisable(GL_DEPTH_TEST);
	glUseProgram(shaderSkybox);

	// ... set view and projection matrix
	glm::mat4 projection_skybox = glm::perspective(float(45.0f*DEG_TO_RADIAN), (float)800.0f / (float)600.0f, 0.1f, 100.0f);
	rt3d::setUniformMatrix4fv(shaderSkybox, "projection", glm::value_ptr(projection_skybox)); // JR note this is a 45 degree projection because we are looking at the face of a cube
	glm::mat3 mvRotOnlyMat3 = glm::mat3(mvStack.top());
	mvStack.push(glm::mat4(mvRotOnlyMat3)); // only use the rotational part, i.e. upper 3x3
	rt3d::setUniformMatrix4fv(shaderSkybox, "modelview", glm::value_ptr(mvStack.top()));
	mvStack.pop();

	glBindVertexArray(meshObjects[3]); //bind the VAO
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture); // bind the cubemap
	glDrawArrays(GL_TRIANGLES, 0, 36); // draw
	glBindVertexArray(0); // unbind the VAO (by binding to zero)

	glEnable(GL_CULL_FACE); // enable culling and depth test to draw rest of scene
	glEnable(GL_DEPTH_TEST);

	/////////////////////////////////////////

	glUseProgram(shaderProgram);
	glm::vec4 tmp = mvStack.top()*lightPos;
	rt3d::setLightPos(shaderProgram, glm::value_ptr(tmp));
	rt3d::setUniformMatrix4fv(shaderProgram, "projection", glm::value_ptr(projection));
	glDepthMask(GL_TRUE); // make sure depth test is on

						  // draw a cube for ground plane
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(0.0f, -0.1f, 0.0f));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(20.0f, 0.1f, 20.0f));
	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::setMaterial(shaderProgram, material0);
	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
	mvStack.pop();


	// Animate the md2 model, and update the mesh with new vertex data
	tmpModel.Animate(currentAnim, 1.0f);
	cout << "Current animaiton" << currentAnim << endl;
	rt3d::updateMesh(meshObjects[1], RT3D_VERTEX, tmpModel.getAnimVerts(), tmpModel.getVertDataSize());

	// draw the hobgoblin
	//glm::vec4 hobgoblinLightPos(hobgoblinPos.x, hobgoblinPos.y + 1.0, hobgoblinPos.z, 1.0f);
	//hobgoblinLightPos = hobgoblinLightPos*mvStack.top();
	////light0.position[0] = hobgoblinPos.x;.
	////light0.position[1] = hobgoblinPos.y;
	////light0.position[3] = hobgoblinPos.z;
	//rt3d::setLightPos(shaderProgram, glm::value_ptr(hobgoblinLightPos));
	//glCullFace(GL_FRONT);
	//glBindTexture(GL_TEXTURE_2D, textures[1]);
	//rt3d::materialStruct tmpMaterial = material1;
	//rt3d::setMaterial(shaderProgram, tmpMaterial);
	//mvStack.push(mvStack.top());
	//mvStack.top() = glm::translate(mvStack.top(), glm::vec3(hobgoblinPos.x, hobgoblinPos.y, hobgoblinPos.z));
	//mvStack.top() = glm::rotate(mvStack.top(), float(90.0f*DEG_TO_RADIAN), glm::vec3(-1.0f, 0.0f, 0.0f));
	//mvStack.top() = glm::rotate(mvStack.top(), float((r-90.0f)*DEG_TO_RADIAN), glm::vec3(0.0f, 0.0f, -1.0f));
	//mvStack.top() = glm::scale(mvStack.top(),glm::vec3(scale*0.05, scale*0.05, scale*0.05));
	//rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
	//rt3d::drawMesh(meshObjects[1], md2VertCount, GL_TRIANGLES);
	//mvStack.pop();
	//glCullFace(GL_BACK);

	//Draw npc

	glm::vec4 hobgoblinLightPos(hobgoblinPos.x, hobgoblinPos.y + 1.0, hobgoblinPos.z, 1.0f);
	glCullFace(GL_FRONT);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	rt3d::materialStruct tmpMaterial = material1;
	rt3d::setMaterial(shaderProgram, tmpMaterial);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(hobgoblinPos.x, hobgoblinPos.y, hobgoblinPos.z));
	mvStack.top() = glm::rotate(mvStack.top(), float(90.0f*DEG_TO_RADIAN), glm::vec3(-1.0f, 0.0f, 0.0f));
	mvStack.top() = glm::rotate(mvStack.top(), float((r-90.0f)*DEG_TO_RADIAN), glm::vec3(0.0f, 0.0f, -1.0f));
	mvStack.top() = glm::scale(mvStack.top(),glm::vec3(scale*0.05, scale*0.05, scale*0.05));
	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::drawMesh(meshObjects[1], md2VertCount, GL_TRIANGLES);
	mvStack.pop();
	glCullFace(GL_BACK);

	//Draw the car
	glCullFace(GL_FRONT);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	rt3d::materialStruct tmpMaterial1 = material1;
	rt3d::setMaterial(shaderProgram, tmpMaterial1);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(Car.x, Car.y, Car.z));
	mvStack.top() = glm::rotate(mvStack.top(), float(180.0f*DEG_TO_RADIAN), glm::vec3(-1.0f, 0.0f, 0.0f));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(scale*-1.5f, scale*-1.5f, scale*-1.5f));
	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::drawMesh(meshObjects[2], md2VertCount, GL_TRIANGLES);
	mvStack.pop();
	glCullFace(GL_BACK);

	//Draw the building
	glCullFace(GL_FRONT);
	glBindTexture(GL_TEXTURE_2D, textures[4]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(10.f, 0.0, 1.0f));
	mvStack.top() = glm::rotate(mvStack.top(), float(0.0f*DEG_TO_RADIAN), glm::vec3(-1.0f, 0.0f, 0.0f));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(scale*2.0f, scale*2.0f, scale*2.0f));
	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::drawMesh(meshObjects[4], md2VertCount, GL_TRIANGLES);
	mvStack.pop();
	glCullFace(GL_BACK);

	//Draw the building
	glCullFace(GL_FRONT);
	glBindTexture(GL_TEXTURE_2D, textures[5]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(50.f, 0.0, 1.0f));
	mvStack.top() = glm::rotate(mvStack.top(), float(0.0f*DEG_TO_RADIAN), glm::vec3(-1.0f, 0.0f, 0.0f));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(scale*2.0f, scale*2.0f, scale*2.0f));
	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::drawMesh(meshObjects[4], md2VertCount, GL_TRIANGLES);
	mvStack.pop();
	glCullFace(GL_BACK);


	//Draw the building
	glCullFace(GL_FRONT);
	glBindTexture(GL_TEXTURE_2D, textures[6]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(30.f, 0.0, 30.0f));
	mvStack.top() = glm::rotate(mvStack.top(), float(0.0f*DEG_TO_RADIAN), glm::vec3(-1.0f, 0.0f, 0.0f));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(scale*2.0f, scale*2.0f, scale*2.0f));
	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::drawMesh(meshObjects[4], md2VertCount, GL_TRIANGLES);
	mvStack.pop();
	glCullFace(GL_BACK);

	//Draw the building
	glCullFace(GL_FRONT);
	glBindTexture(GL_TEXTURE_2D, textures[6]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-10.0, 0.0, 30.0f));
	mvStack.top() = glm::rotate(mvStack.top(), float(0.0f*DEG_TO_RADIAN), glm::vec3(-1.0f, 0.0f, 0.0f));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(scale*2.0f, scale*2.0f, scale*2.0f));
	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::drawMesh(meshObjects[5], md2VertCount, GL_TRIANGLES);
	mvStack.pop();
	glCullFace(GL_BACK);

	mvStack.pop();
	SDL_GL_SwapWindow(window); // swap buffers
}


// Program entry point - SDL manages the actual WinMain entry point for us
int main(int argc, char *argv[]) {	
    SDL_Window * hWindow; // window handle
    SDL_GLContext glContext; // OpenGL context handle
    hWindow = setupRC(glContext); // Create window and render context 
	
	// Required on Windows *only* init GLEW to access OpenGL beyond 1.1
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) { // glewInit failed, something is seriously wrong
		std::cout << "glewInit failed, aborting." << endl;
		exit (1);
	}
	cout << glGetString(GL_VERSION) << endl;

	init();
	Audio::LoadSample("Audio/GTARadio.wav");
	bool running = true; // set running to true
	SDL_Event sdlEvent;  // variable to detect SDL events
	while (running)	{	// the event loop
		while (SDL_PollEvent(&sdlEvent)) {
			if (sdlEvent.type == SDL_QUIT)
				running = false;
		}
		update();
		draw(hWindow); // call the draw function
	}
	//BASS_Free();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(hWindow);
    SDL_Quit();
    return 0;
}