#pragma once
#include "camera.h"
#include <ctime>

GLfloat particleVertices[] = {
	// Positions          // Texture Coords
	-0.5f, -0.5f,  0.0f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.0f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.0f,  0.0f, 0.0f,

};

class Particle
{
public:
	typedef struct Transform {
		// object's transform
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
		// object space axis
		glm::vec3 forward = { 0.0f, 0.0f, 1.0f };
		// positive direction of y-axis
		glm::vec3 up = { 0.0f, 1.0f, 0.0f };
		// positive direction of x-axis
		glm::vec3 right = { -1.0f, 0.0f, 0.0f };
		// world space axis ident vector
		glm::vec3 x = { 1.0f, 0.0f, 0.0f };
		glm::vec3 y = { 0.0f, 1.0f, 0.0f };
		glm::vec3 z = { 0.0f, 0.0f, 1.0f };

		glm::mat4 model;
	}Transform;
	// position, rotation and scale of the object
	Transform transform;

	// toggle whether the particle is 
	bool isActive = true;
	// start life time
	clock_t startTime;
	// time for the particle to exist
	float lifetime;
	// speed and direction for the particle
	glm::vec3 velocity;
	// axis to rotate around
	glm::vec3 axis;
public:
	Particle(glm::vec3 start, glm::vec3 scale, glm::vec3 velocity, glm::vec3 axis, float lifetime) {
		
		this->isActive = true;
		this->transform.position = start;
		this->transform.scale = scale;
		this->velocity = velocity;
		this->axis = axis;
		this->lifetime = lifetime;
		this->startTime = clock();
	}
	Particle() {}
	~Particle() {}

	void update() {
		this->transform.position = this->transform.position + this->velocity;
		clock_t currentTime = clock();
		if ((float)(currentTime - startTime) >= lifetime) {
			this->isActive = false;
		}
	}

	/// render a particle with given texture
	void render(Camera camera, Shader shader, unsigned int VAO, unsigned int VBO) {

		glm::vec3 position = this->transform.position;
		glm::vec3 rotation = this->transform.rotation;
		glm::vec3 scale = this->transform.scale;
		// DRAW
		shader.use();
		// create Transform
		// identity matrix
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		// transformation
		// translate to position
		model = glm::translate(model, glm::vec3(position[0], position[1], position[2]));
		// rotate over lifetime
		model = glm::rotate(model, (float)glfwGetTime() * 2, this->axis);
		// scaling
		model = glm::scale(model, glm::vec3(scale[0], scale[1], scale[2]));
		// view
		view = camera.GetViewMatrix();
		// projection
		// projection = glm::perspective(glm::radians(100.0f), (float)WINDOW_WIDTH * 2.0f / (float)WINDOW_HEIGHT,  0.1f, 10.0f);
		projection = glm::perspective(glm::radians(camera.Zoom), 4.0f / 3.0f, 0.1f, 100.0f);
		// pass to shader
		// view
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		// light properties
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};

