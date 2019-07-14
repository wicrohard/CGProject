#pragma once
#include "Particle.hpp"
#include <iostream>
#include <ctime>
#include <map>

#define MAX_PARTICLE_NUM 20

class ParticleSystem
{
public:
	// emission's position
	glm::vec3 position;
	// emission's direction
	glm::vec3 direction;
	// particle's lifetime
	float lifetime;
	// particle's size
	glm::vec3 scale;
	// emission's area's size
	GLint size;
	// emission's field
	GLfloat foe = 10;
	// toogle whether the particle system is pauseed or not
	bool isPause = true;
	// rate for particle generation
	float emissionRate = 1.0f;
	// speed of the particle
	float speed = 0.01f;
	// texture's id
	unsigned int texture;
	unsigned int VAO;
	unsigned int VBO;
	// particles
	Particle particles[MAX_PARTICLE_NUM];
	// number of particles
	int numberOfParticles;


	// clock for spawn
	clock_t last, currentTime;
public:

	ParticleSystem(glm::vec3 position, glm::vec3 direction, glm::vec3 scale, float speed, float lifetime, int size) {
		this->position = position;
		this->direction = direction;
		this->scale = scale;
		this->speed = speed;
		this->lifetime = lifetime;
		this->texture = texture;
		this->size = size;
		this->numberOfParticles = 0;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particleVertices), particleVertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
	}
	ParticleSystem() {}
	~ParticleSystem() {}

	void load(const char* path) {
		// create texture
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		// set texture wrapping parameters
		// for S direction (x direction)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		// for T direction (y direction)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// for minify
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		// set color
		float border_color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

		// load image
		int width, height, nrChannels;
		// tell stb_image.h to flip the loaded texture's on the y-axis
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Failed to load texture!" << std::endl;
		}
		stbi_image_free(data);


	}

	void update() {
		if (!isPause) {
			currentTime = clock();
			if (this->numberOfParticles < MAX_PARTICLE_NUM && ((float)(currentTime - last) > 200.0f)) {
				spawn();
				this->last = currentTime;
			}
			for (int i = 0; i < numberOfParticles; i++) {
				particles[i].update();
				if (!particles[i].isActive) {
					respawn(i);
				}
			}
		}
	}
	
	void render(Camera camera, Shader shader) {
		shader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		// sort with distance from camera
		std::map<float, int> sorted_indices;
		for (int i = 0; i < numberOfParticles; i++) {
			float distance = glm::length(camera.Position - particles[i].transform.position);
			sorted_indices[distance] = i;
		}
		// render those of farther distance
		for (std::map<float, int>::reverse_iterator it = sorted_indices.rbegin(); it != sorted_indices.rend(); ++it) {
			particles[it->second].render(camera, shader, VAO, VBO);
		}
	}

	// @dev Create a particle and spawn it
	void spawn() {
		
		// a random axis to rotate around
		GLfloat randomX = ((rand() % 100) - 50) / 50.0f;
		GLfloat  randomY = ((rand() % 100) - 50) / 50.0f;
		glm::vec3 axis = { randomX, randomY, 0.0f };
		// a random start position inside specified spawn area
		randomX = (float)((rand() % size) - size/2)/10.0f;
		randomY = (float)((rand() % size) - size/2)/10.0f;
		glm::vec3 start_pos = this->position + glm::vec3(randomX, 0.0f, randomY);
		// a random velocity
		glm::vec3 velocity = this->direction * this->speed;
		particles[this->numberOfParticles] = Particle(start_pos, scale, velocity, axis, lifetime);
		this->numberOfParticles++;
	}

	// @dev respawn the particle that has reached end of lifetime, giving it a new start position and velocity and refresh lifetime
	void respawn(int index) {
		// a random axis to rotate around
		GLfloat randomX = ((rand() % 100) - 50) / 50.0f;
		GLfloat  randomY = ((rand() % 100) - 50) / 50.0f;
		glm::vec3 axis = { randomX, randomY, 0.0f };
		// a random start position inside specified spawn area
		randomX = (float)((rand() % size) - size/2)/10.0f;
		randomY = (float)((rand() % size) - size/2)/10.0f;
		glm::vec3 start_pos = this->position + glm::vec3(randomX, 0.0f, randomY);
		// a random velocity
		glm::vec3 velocity = this->direction * this->speed;

		// update variables
		particles[index].transform.position = start_pos;
		particles[index].axis = axis;
		particles[index].velocity = velocity;
		particles[index].startTime = clock();
		particles[index].isActive = true;
	}

	void pause() {
		this->isPause = true;
	}

	void start() {
		this->isPause = false;
		this->last = clock();
	}
};

