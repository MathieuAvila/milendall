
#define GLM_FORCE_MESSAGES
#include <glm/glm.hpp>

#include "level.hxx"

#include <memory>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>

using namespace std;


// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "gl_init.hxx"

using namespace glm;

#include "common.hxx"

int main( void )
{
    set_level(level::debug);

	// Initialise GLFW

    milendall_gl_init();


    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/room1/"));
    auto ref = fl.getRoot().getSubPath("room.gltf");
    GltfMaterialLibraryIfacePtr materialLibrary = GltfMaterialLibraryIface::getMaterialLibray();
    auto model = make_unique<GltfModel>(materialLibrary, ref);
    auto instance = make_unique<GltfInstance>(model->getInstanceParameters());
    glm::mat4 mat_id;
    model->applyDefaultTransform(instance.get(), mat_id);



	do{

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		setMeshMatrix(glm::mat4(1.0));

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureID, 0);


        model->draw(instance.get());


		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

    milendall_gl_close();

	return 0;
}

