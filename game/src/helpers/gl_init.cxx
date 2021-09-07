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

static auto console = spdlog::stdout_color_mt("gl_init");


glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

// Initial position : on +Z
glm::vec3 position = glm::vec3( 2, 2, 2 );

// view direction
glm::vec3 direction;

// up direction
glm::vec3 up;

// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
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

GLuint portalProgramID;
GLuint MatrixIDportal;
GLuint TextureIDportal;

bool usingMainProgram = true;

void computeMatricesFromInputs(){

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024/2, 768/2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(1024/2 - xpos );
	verticalAngle   += mouseSpeed * float( 768/2 - ypos );

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	direction = glm::vec3(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f),
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);

	// Up vector
	up = glm::cross( right, direction );

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
		position -= right * deltaTime * speed;
	}

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								position+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
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
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
    if (usingMainProgram)
    	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    else
        glUniformMatrix4fv(MatrixIDportal, 1, GL_FALSE, &MVP[0][0]);
}

void setMeshMatrix(glm::mat4 mat)
{
    ModelMatrix = mat;
	updateTransformMatrix();
}

void setViewComponents(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
{
    float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.0001f, 100.0f);
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								position+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );
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

void activateDefaultDrawingProgram()
{
	glUseProgram(defaultProgramID);
    glActiveTexture(GL_TEXTURE0);
	glUniform1i(TextureID, 0);
    usingMainProgram = true;
    updateTransformMatrix();
}

void activatePortalDrawingProgram()
{
	glUseProgram(portalProgramID);
    glActiveTexture(GL_TEXTURE0);
	glUniform1i(TextureIDportal, 0);
    usingMainProgram = false;
    updateTransformMatrix();
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

    portalProgramID = LoadShaders(library, "Portal.vertexshader", "Portal.fragmentshader" );
    MatrixIDportal = glGetUniformLocation(portalProgramID, "MVP");
	TextureIDportal  = glGetUniformLocation(defaultProgramID, "myTextureSampler");

    initFbo();

    return 0;
}

void milendall_gl_close()
{
	glDeleteProgram(defaultProgramID);
	glDeleteProgram(portalProgramID);
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