#include <memory>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>

#include "common.hxx"
#include "gl_init.hxx"

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std;

static auto console = getConsole("gl_init");


static glm::mat4 ViewMatrix;
static glm::mat4 ProjectionMatrix;

// Initial position : on +Z
glm::vec3 position = glm::vec3( 2, 2, 2 );

// view direction
glm::vec3 direction;

// up direction
glm::vec3 up;

// Initial horizontal angle : toward -Z
float horizontalAngle = 0.0f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;

/* FBO management */
#define TOTAL_COUNT_FBO 32
static FboIndex fboTable[TOTAL_COUNT_FBO];
static int currentFboIndex;
static int currentActiveFbo;

/* Shaders management */
GLuint defaultProgramID;
GLuint MatrixID;
GLuint TextureID;
GLuint MainClipPlaneID;
GLuint PortalClipPlaneID;

GLuint portalProgramID;
GLuint MatrixIDportal;
GLuint TextureIDportal;

GLuint fontProgramID;

enum { NONE, MAIN, PORTAL, FONT } usingProgram;

void updatePlayerInputs()
{
	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024/2, 768/2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(1024/2 - xpos );
	verticalAngle   += mouseSpeed * float( 768/2 - ypos );
}

GLuint LoadShaders(FileLibrary& library, std::string vertex_file_path, std::string fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
    std::string VertexShaderCode = library.getRoot().getSubPath("common/shaders/" + vertex_file_path).readStringContent();

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode = library.getRoot().getSubPath("common/shaders/" + fragment_file_path).readStringContent();

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	console->info("Compiling shader : {}", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		console->error("{}", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	console->info("Compiling shader : {}", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		console->error("{}", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	console->info("Linking program");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		console->error("{}", &ProgramErrorMessage[0]);
	}

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}


void glfw_callback(int error_code, const char* description)
{
    console->error("{}={}", error_code, description);
}

static glm::mat4 ModelMatrix = glm::mat4(1.0);

static void updateTransformMatrix()
{
    float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.0001f, 100.0f);

    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
    if (usingProgram == MAIN)
    	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    else if (usingProgram == PORTAL)
        glUniformMatrix4fv(MatrixIDportal, 1, GL_FALSE, &MVP[0][0]);
}

void setMeshMatrix(glm::mat4 mat)
{
    ModelMatrix = mat;
	updateTransformMatrix();
}

void setViewMatrix(glm::mat4x4 mat)
{
    ViewMatrix = mat;
    updateTransformMatrix();
}

void initFbo()
{
    for (auto i = 0; i < TOTAL_COUNT_FBO; i++)
    {
        unsigned int fbo;
        unsigned int texColorBuffer;
        unsigned int rbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        // generate texture
        glGenTextures(1, &texColorBuffer);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 768, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
        // rbo
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1024, 768);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        // sanity check
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::stringstream stream;
            stream << std::hex << glCheckFramebufferStatus(GL_FRAMEBUFFER);
            throw GlException(std::string("Unable to init FBO index: ") + to_string(i) + "  " + stream.str());
        }
        // reset
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // store information
        fboTable[i].fboIndex = fbo;
        fboTable[i].textureIndex = texColorBuffer;
        fboTable[i].rbo = rbo;
    };
    unlockAllFbo();
}

int getValidFbo(FboIndex* result)
{
    if (currentFboIndex < TOTAL_COUNT_FBO) {
        *result = fboTable[currentFboIndex];
        glBindFramebuffer(GL_FRAMEBUFFER, result->fboIndex);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, currentActiveFbo);
        currentFboIndex++;
        return 0;
    } else {
        return -1;
    }
}

void unlockAllFbo()
{
    currentFboIndex = 0;
    currentActiveFbo = 0;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void setActiveFbo(FboIndex* fbo)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fboIndex);
    currentActiveFbo = fbo->fboIndex;
}

void setClippingEquations(std::vector<glm::vec3> equs)
{
    float planeEquation[6][4] = {
         {-1.0, 0.0, 1.0, 0.0},
         {-1.0, 0.0, 1.0, 0.0},
         {-1.0, 0.0, 1.0, 0.0},
         {-1.0, 0.0, 1.0, 0.0},
         {-1.0, 0.0, 1.0, 0.0},
         {-1.0, 0.0, 1.0, 0.0},
    };
    int m = equs.size() > 6 ? 6 : equs.size();
    for (int i = 0; i < m; i++) {
        planeEquation[i][0] = equs[i].x;
        planeEquation[i][1] = equs[i].y;
        planeEquation[i][2] = equs[i].z;

        planeEquation[i][3] = 0.0;
    }
    switch (usingProgram) {
    case MAIN:
        glUniform4fv(MainClipPlaneID, 6, &planeEquation[0][0]);
        break;
    case PORTAL:
        glUniform4fv(PortalClipPlaneID, 6, &planeEquation[0][0]);
        break;
    default:
        break;
    }
}

void activateDefaultDrawingProgram()
{
	glUseProgram(defaultProgramID);
    glActiveTexture(GL_TEXTURE0);
	glUniform1i(TextureID, 0);
    usingProgram = MAIN;
    updateTransformMatrix();
    for (auto i = 0; i < 6; i++)
        glEnable(GL_CLIP_DISTANCE0 + i);
}

void activatePortalDrawingProgram()
{
	glUseProgram(portalProgramID);
    glActiveTexture(GL_TEXTURE0);
	glUniform1i(TextureIDportal, 0);
    for (auto i = 0; i < 6; i++)
        glEnable(GL_CLIP_DISTANCE0 + i);
    usingProgram = PORTAL;
    updateTransformMatrix();
}

unsigned int activateFontDrawingProgram()
{
	glUseProgram(fontProgramID);
    for (auto i = 0; i < 6; i++)
        glDisable(GL_CLIP_DISTANCE0 + i);
    usingProgram = FONT;
    return fontProgramID;
}

int milendall_gl_init(FileLibrary& library)
{
	// Initialise GLFW
    glfwSetErrorCallback(glfw_callback);
	if( !glfwInit() )
	{
		console->error("Failed to initialize GLFW");
		return -1;
	}
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Milendall", NULL, NULL);
	if( window == NULL ){
		console->error("Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible");
		getchar();
		glfwTerminate();
		return -1;
	}
    glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		console->error("Failed to initialize GLEW");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	defaultProgramID = LoadShaders(library, "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader" );
    MatrixID = glGetUniformLocation(defaultProgramID, "MVP");
	TextureID  = glGetUniformLocation(defaultProgramID, "myTextureSampler");
    MainClipPlaneID = glGetUniformLocation(defaultProgramID, "ClipPlane");

    portalProgramID = LoadShaders(library, "Portal.vertexshader", "Portal.fragmentshader" );
    MatrixIDportal = glGetUniformLocation(portalProgramID, "MVP");
	TextureIDportal  = glGetUniformLocation(defaultProgramID, "myTextureSampler");
    PortalClipPlaneID = glGetUniformLocation(defaultProgramID, "ClipPlane");

    fontProgramID = LoadShaders(library, "font.vertexshader", "font.fragmentshader" );

    initFbo();

    return 0;
}

void milendall_gl_close()
{
	glDeleteProgram(defaultProgramID);
	glDeleteProgram(portalProgramID);
	glDeleteProgram(fontProgramID);
	glfwTerminate();
}

void TrianglesBufferInfo::draw()
{
    // 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

	// 2nd attribute buffer : UVs
    if (uvbuffer != 0) {
        glEnableVertexAttribArray(1);
	    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	    glVertexAttribPointer(
		    1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);
    }

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

		// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,      // mode
		indicesCount,    // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0           // element array buffer offset
	);

	glDisableVertexAttribArray(0);
	if (uvbuffer != 0)
        glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

TrianglesBufferInfo::TrianglesBufferInfo(
        std::span<glm::vec3> vertices,
        std::span<unsigned short> indices) : uvbuffer(0)
{
    glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), indices.data(), GL_STATIC_DRAW);

    indicesCount = indices.size();

    console->info("TrianglesBufferInfo p={} uv={} i={} - indices={}",
                vertexBuffer, uvbuffer, elementbuffer, indicesCount);
}

TrianglesBufferInfo::TrianglesBufferInfo(
        std::span<glm::vec3> vertices,
        std::span<glm::vec2> uv,
        std::span<unsigned short> indices)
{
    glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(glm::vec2), uv.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), indices.data(), GL_STATIC_DRAW);

    indicesCount = indices.size();

    console->info("TrianglesBufferInfo p={} uv={} i={} - indices={}",
                vertexBuffer, uvbuffer, elementbuffer, indicesCount);
}

TrianglesBufferInfo::~TrianglesBufferInfo()
{
    glDeleteBuffers(1, &vertexBuffer);
    if (uvbuffer)
        glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &elementbuffer);
}