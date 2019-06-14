#pragma once
#include <stb_image.h>
#include "camera.h"

GLfloat planeVerticesTextureOnly[] = {
    // Positions          // Texture Coords
    -0.5f, -0.5f,  0.0f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.0f,  0.0f, 0.0f,

};


class Plane
{
public:
    // @dev define a structure of transform, which contains the position, rotation and scale
    // of the object, and also the three axis of the object's object space and world space.
    typedef struct Transform {
        // object's transform
        glm::vec3 position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
        // object space axis
        glm::vec3 forward = { 0.0f, 0.0f, 1.0f};
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
private:
    unsigned int VAO, VBO;
public:
    Plane() {}
    Plane(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
        this->transform.position = position;
        this->transform.rotation = rotation;
        this->transform.scale = scale;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVerticesTextureOnly), planeVerticesTextureOnly, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
    }
    ~Plane() {}

    void render(Camera cam, Shader shader) {
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
        // rotate with rotation
		model = glm::rotate(model, rotation[0], { 0.0f, 0.0f, 1.0f });
		glm::mat4 rotateY = {
			rotation[1], 0.0f, rotation[2], 0.0f,
			0.0f, 1.0f, 0.0f ,0.0f,
			-rotation[2], 0.0f, rotation[1], 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		model = rotateY * model;
		model = glm::rotate(model, glm::radians(90.0f), { 0.0f, 1.0f, 0.0f });

        // scaling
        model = glm::scale(model, glm::vec3(scale[0], scale[1], scale[2]));
        // view
        view = cam.GetViewMatrix();
        // projection
        // projection = glm::perspective(glm::radians(100.0f), (float)WINDOW_WIDTH * 2.0f / (float)WINDOW_HEIGHT,  0.1f, 10.0f);
        projection = glm::perspective(glm::radians(cam.Zoom), 4.0f/3.0f, 0.1f, 100.0f);
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

        glDeleteBuffers(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};

