#include <glad/glad.h>		
#include <GLFW/glfw3.h>		
#include <iostream>

//#include "linmath.h"
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <stdlib.h>
#include <stdio.h>
#include <sstream>		// "String Stream", used for setting the windows title

#include <vector>
#include "cGameObject.h"

#include "cMeshTypeManager.h"	
#include "cSceneLoader.h"

std::vector< cGameObject* > g_vec_pGameObjects;

cMeshTypeManager* g_pMeshTypeManager = 0;
cSceneLoader* g_pSceneLoader = 0;

glm::vec3 g_offset = glm::vec3(-55.0f, 0.0f, 0.f);

glm::mat4 g_targetTransform;

static const char* vertex_shader_text =
"in vec3 vColour;\n"		// "attribute"
"in vec3 vPosition;\n"		// "attribute"

"uniform mat4 MVP;\n"
"uniform vec3 objectColour;\n"
"uniform int  isWireframe;		// 0 = false, 1 = true\n"

"out vec3 color;		// Used to be varying\n"

"void main()\n"
"{\n"
"    gl_Position = MVP * vec4( vPosition, 1.0f );\n"
"	color = objectColour;\n"
"}\n";

static const char* fragment_shader_text =
"in vec3 color;\n"

"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0f);\n"
"}\n";

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);


	if (key == GLFW_KEY_A)	g_targetTransform = glm::rotate(g_targetTransform, 0.1f, glm::vec3(0.f, 1.f, 0.f));		// "Left"

	if (key == GLFW_KEY_D)	g_targetTransform = glm::rotate(g_targetTransform, -0.1f, glm::vec3(0.f, 1.f, 0.f));		// "Right"

	if (key == GLFW_KEY_W)	g_targetTransform = glm::translate(g_targetTransform, glm::vec3(1.0f, 0.0, 0.f));		// "Forward"

	if (key == GLFW_KEY_S)	g_targetTransform = glm::translate(g_targetTransform, glm::vec3(-1.0f, 0.0, 0.f));			// "Back"

	if (key == GLFW_KEY_Q)	g_targetTransform = glm::translate(g_targetTransform, glm::vec3(0.0, -1.0, 0.f));		// "Down"

	if (key == GLFW_KEY_E)	g_targetTransform = glm::translate(g_targetTransform, glm::vec3(0.0, 1.0, 0.f));		// "Up"

	if (key == GLFW_KEY_V && action == GLFW_PRESS) {
		for (int index = 0; index < g_vec_pGameObjects.size(); index++)
		{
			g_vec_pGameObjects[index]->bIsWireframe = !g_vec_pGameObjects[index]->bIsWireframe;
		}
	}

}

int main(void)
{
	GLFWwindow* window;
	GLuint vertex_shader, fragment_shader, program;
	GLint mvp_location;
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	window = glfwCreateWindow(1200, 800, "Graphics Project1 : By Ryan Hammond", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	// Shader loading happening before vertex buffer array
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
	glCompileShader(vertex_shader);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
	glCompileShader(fragment_shader);

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);


	/*g_targetTransform[3].x = -300.0f;
	g_targetTransform[3].y = 2.5f;
	g_targetTransform[3].z = 5.0f;*/
	// Entire loading the model and placing ito the VAOs 
	// is now in the cMeshTypeManager
	::g_pMeshTypeManager = new cMeshTypeManager();
	::g_pSceneLoader = new cSceneLoader();

	::g_pSceneLoader->loadScene("scene.txt", *g_pMeshTypeManager, program, g_vec_pGameObjects, g_targetTransform);

	mvp_location = glGetUniformLocation(program, "MVP");

	GLuint UniformLoc_ID_objectColour = 0;
	GLuint UniformLoc_ID_isWireframe = 0;

	UniformLoc_ID_objectColour = glGetUniformLocation(program, "objectColour");
	UniformLoc_ID_isWireframe = glGetUniformLocation(program, "isWireframe");


	while (!glfwWindowShouldClose(window))
	{
		float ratio;
		int width, height;

		glm::mat4x4 p;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		glViewport(0, 0, width, height);
		glClearColor(0.f, 0.74f, 1.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// The "view" matrix is (usually) only updated once per "scene"
		// The "projection" matrix only needs to be updated when the window size 
		//    changes, but here we are updating it every "scene"
		// (a "scene" is when ALL the game objects are drawn)

		glm::mat4x4 v(1.0f);	// "View" (or camera) matrix

		p = glm::perspective(0.6f, ratio, 0.01f, 10000.0f);

		//set up the view matrix
		glm::vec4 eye4(g_targetTransform[3].x, g_targetTransform[3].y, g_targetTransform[3].z, 1.0f);
		glm::vec3 target(g_targetTransform[3].x, g_targetTransform[3].y, g_targetTransform[3].z);
		glm::vec4 offset(g_offset.x, g_offset.y, g_offset.z, 0.f);
		offset = g_targetTransform * offset;
		offset = glm::normalize(offset) * 20.f;
		glm::vec3 eye(target.x + offset.x, target.y + offset.y, target.z + offset.z);


		v = glm::lookAtRH(eye, target, glm::vec3(0.f, 1.f, 0.f));

		// Start of Draw Scene
		glDepthFunc(GL_ALWAYS);
		glEnable(GL_DEPTH_TEST);

		for (int index = 0; index != ::g_vec_pGameObjects.size(); index++)
		{
			cGameObject* pCurGameObject = ::g_vec_pGameObjects[index];

			std::string meshModelName = pCurGameObject->meshName;

			GLuint VAO_ID = 0;
			int numberOfIndices = 0;
			float unitScale = 1.0f;
			if (!::g_pMeshTypeManager->LookUpMeshInfo(meshModelName,
				VAO_ID,
				numberOfIndices,
				unitScale))
			{	// Skip the rest of the for loop, but continue
				continue;
			}

			glm::mat4x4 mvp(1.0f);		// Model, View, Projection matrix

										//			mat4x4_identity(m);
			glm::mat4x4 m = glm::mat4x4(1.0f);


			// Pre-rotation
			// Translation
			// Post-rotation
			// Scale

			// Pre in that it's BEFORE the translation
			m = glm::rotate(m, pCurGameObject->pre_Rot_X, glm::vec3(1.0f, 0.0f, 0.0f));
			m = glm::rotate(m, pCurGameObject->pre_Rot_Y, glm::vec3(0.0f, 1.0f, 0.0f));
			m = glm::rotate(m, pCurGameObject->pre_Rot_Z, glm::vec3(0.0f, 0.0f, 1.0f));

			m = glm::translate(m, glm::vec3(pCurGameObject->x, pCurGameObject->y, pCurGameObject->z));

			// Pre in that it's AFTER the translation
			m = glm::rotate(m, pCurGameObject->post_Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
			m = glm::rotate(m, pCurGameObject->post_Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
			m = glm::rotate(m, pCurGameObject->post_Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

			//
			float actualScale = pCurGameObject->scale * unitScale;

			m = glm::scale(m, glm::vec3(actualScale, actualScale, actualScale));


			if (pCurGameObject->bIsWireframe)
			{	// Turn off backface culling
				// Enable "wireframe" polygon mode
				glPolygonMode(GL_FRONT_AND_BACK,	// GL_FRONT_AND_BACK is the only thing you can pass here
					GL_LINE);			// GL_POINT, GL_LINE, or GL_FILL
				glDisable(GL_CULL_FACE);
			}
			else
			{	// "Regular" rendering: 
				// Turn on backface culling
				// Turn polygon mode to solid (Fill)
				//glCullFace( GL_BACK );		// GL_FRONT, GL_BACK, or GL_FRONT_AND_BACK
				//glEnable( GL_CULL_FACE );
				glPolygonMode(GL_FRONT_AND_BACK,	// GL_FRONT_AND_BACK is the only thing you can pass here
					GL_FILL);			// GL_POINT, GL_LINE, or GL_FILL
			}
			mvp = p * v * m;

			glUseProgram(program);

			glUniformMatrix4fv(mvp_location, 1, GL_FALSE,
				(const GLfloat*)glm::value_ptr(mvp));

			// Setting the uniform colours
			glUniform3f(UniformLoc_ID_objectColour,
				pCurGameObject->solid_R, pCurGameObject->solid_G, pCurGameObject->solid_B);
			if (pCurGameObject->bIsWireframe)
			{
				glUniform1i(UniformLoc_ID_isWireframe, TRUE);		// 1
			}
			else
			{
				glUniform1i(UniformLoc_ID_isWireframe, FALSE);	// 0
			}

			// Drawing indirectly from the index buffer

			glBindVertexArray(VAO_ID);
			glDrawElements(GL_TRIANGLES,
				numberOfIndices,
				GL_UNSIGNED_INT,	// Each index is how big?? 
				(GLvoid*)0);		// Starting point in buffer
			glBindVertexArray(0);

		}// for ( int index = 0;.... (bottom of "render scene" loop)

		 // End of Draw Scene

		 // Show or "present" what we drew...
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	// Bye bye...
	delete ::g_pMeshTypeManager;
	delete ::g_pSceneLoader;

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}