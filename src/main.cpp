#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"
#include "model.h"

#include <stb_image.h>

#include "Plane.hpp"
#include "ParticleSystem.hpp"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
//���ڼ�����պеĺ���
unsigned int loadCubemap(vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// ��ʼ���ɴ��������λ��
Camera camera(glm::vec3(0.0f, 0.0f, 50.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "FinalProject", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	glEnable(GL_DEPTH_TEST);

	// ��ɫ���Ķ�ȡ�ͱ���
	// -------------------------
	Shader planetShader("model.vs", "model.fs");
	Shader shipShader("shipModel.vs", "shipModel.fs");
	Shader skyboxShader("skybox.vs", "skybox.fs");
	Shader planeShader("plane.vs", "plane.fs");
	Shader shadowShader("shadow_depth.vs", "shadow_depth.fs","shadow_depth.gs");

	planetShader.use();
	planetShader.setInt("texture_diffuse1", 0);
	planetShader.setInt("depthCubeMap", 1);

	shipShader.use();
	shipShader.setInt("texture_diffuse1", 0);
	shipShader.setInt("depthCubeMap", 1);

	// load models
	// -----------
	//��������ģ��
	Model planetModel("planet/planet.obj");

	//���طɴ�ģ��
	Model shipModel("spaceShip/spaceShip.obj");
	float shipSize = 0.125;

	//���ش�����ģ��
	Model portalModel("portal/portal.obj");

	//��պеĶ�������
	float skyboxVertices[] = {
		// positions          
		-60.0f,  60.0f, -60.0f,
		-60.0f, -60.0f, -60.0f,
		60.0f, -60.0f, -60.0f,
		60.0f, -60.0f, -60.0f,
		60.0f,  60.0f, -60.0f,
		-60.0f,  60.0f, -60.0f,

		-60.0f, -60.0f,  60.0f,
		-60.0f, -60.0f, -60.0f,
		-60.0f,  60.0f, -60.0f,
		-60.0f,  60.0f, -60.0f,
		-60.0f,  60.0f,  60.0f,
		-60.0f, -60.0f,  60.0f,

		60.0f, -60.0f, -60.0f,
		60.0f, -60.0f,  60.0f,
		60.0f,  60.0f,  60.0f,
		60.0f,  60.0f,  60.0f,
		60.0f,  60.0f, -60.0f,
		60.0f, -60.0f, -60.0f,

		-60.0f, -60.0f,  60.0f,
		-60.0f,  60.0f,  60.0f,
		60.0f,  60.0f,  60.0f,
		60.0f,  60.0f,  60.0f,
		60.0f, -60.0f,  60.0f,
		-60.0f, -60.0f,  60.0f,

		-60.0f,  60.0f, -60.0f,
		60.0f,  60.0f, -60.0f,
		60.0f,  60.0f,  60.0f,
		60.0f,  60.0f,  60.0f,
		-60.0f,  60.0f,  60.0f,
		-60.0f,  60.0f, -60.0f,

		-60.0f, -60.0f, -60.0f,
		-60.0f, -60.0f,  60.0f,
		60.0f, -60.0f, -60.0f,
		60.0f, -60.0f, -60.0f,
		-60.0f, -60.0f,  60.0f,
		60.0f, -60.0f,  60.0f
	};
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//ָ����պ���������ͼ������·��
	vector<std::string> faces
	{
		"sor_cwd/cwd_bk.jpg",
		"sor_cwd/cwd_dn.jpg",
		"sor_cwd/cwd_ft.jpg",
		"sor_cwd/cwd_lf.jpg",
		"sor_cwd/cwd_rt.jpg",
		"sor_cwd/cwd_up.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	//�������ǵ�ƽ��
	Plane sun(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f));

	//��ȡ���ǵ���ͼ
	GLuint sunTexture;
	// create texture
	glGenTextures(1, &sunTexture);
	glBindTexture(GL_TEXTURE_2D, sunTexture);

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
	unsigned char* data = stbi_load("sun/sun.png", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture!" << std::endl;
	}
	stbi_image_free(data);

	//��������ϵͳ
	ParticleSystem pars(glm::vec3(30.2f, 3.0f, 0.3f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), 0.05f, 400.0f, 20);
	pars.load("sun/snowflower.png");
	pars.start();

	//��������Ӱ�������������ͼ
	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// - Create depth texture
	GLuint depthCubeMap;
	glGenTextures(1, &depthCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
	for (GLuint i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		// render
		// ------
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

		//���������ͼ����Ⱦ
		glm::mat4 lightProjection;
		GLfloat near_plane = 1.0f, far_plane = 100.0f;
		lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);

		float radius = 0.1f;
		float planetX = cos(glfwGetTime() * radius) * 30.0;
		float planetZ = -sin(glfwGetTime() * radius) * 30.0;
		float portalOutX = cos(glfwGetTime() * radius - 0.2f) * 30.0;
		float portalOutZ = -sin(glfwGetTime() * radius - 0.2f) * 30.0;
		//float planetX = 30.0f;
		//float planetZ = 30.0f;

		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(lightProjection * glm::lookAt(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(lightProjection *
			glm::lookAt(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(lightProjection *
			glm::lookAt(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(lightProjection *
			glm::lookAt(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(lightProjection *
			glm::lookAt(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(lightProjection *
			glm::lookAt(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
	
		// �ӹ�Դ���ӽǽ��������ͼ����Ⱦ
		//glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		//shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		shadowShader.use();
		for (GLuint i = 0; i < 6; ++i)
			shadowShader.setMat4(("shadowMatrices[" + std::to_string(i) + "]").c_str(), shadowTransforms[i]);
		shadowShader.setFloat("far_plane", far_plane);
		shadowShader.setVec3("lightPos", glm::vec3(1.0f, 0.0f, 0.0f));

		//��Ⱦ����
		glm::mat4 model = glm::mat4(1.0f);
		//��ת
		model = glm::translate(model, glm::vec3(planetX, 0.0f, planetZ));
		//��ת
		model = glm::rotate(model, glm::radians((float)glfwGetTime() * 40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		shadowShader.setMat4("model", model);

		planetModel.Draw(shadowShader);

		//��Ⱦ�ɴ�
		model = glm::mat4(1.0f);
		model = glm::translate(model, camera.getPosition() + camera.getFront());
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		shadowShader.setMat4("model", model);

		shipModel.Draw(shadowShader);

		//��Ⱦ������
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 20.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		shadowShader.setMat4("model", model);

		portalModel.Draw(shadowShader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		/*-------------------------------------------------------------------------------------------------*/
		//������Ⱦ
		//���Ƚ�����պе���Ⱦ
		//glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glDisable(GL_DEPTH_TEST);
		skyboxShader.use();
		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
		//glDepthFunc(GL_LESS);

		// ʹ��������Ⱦ�õ�shader
		planetShader.use();

		//�����Դ��λ��
		planetShader.setVec3("lightPos", glm::vec3(1.0f, 0.0f, 0.0f));
		
		//�����������λ��
		planetShader.setVec3("viewPos", camera.Position);

		//����ͶӰ������ӽǾ���
		view = camera.GetViewMatrix();
		planetShader.setMat4("projection", projection);
		planetShader.setMat4("view", view);

		//����������Ӱ�õ�ͶӰԶƽ��ֵ
		planetShader.setFloat("far_plane", far_plane);

		//��Ⱦ����
		//��ת
		
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(planetX, 0.0f, planetZ)); 
		//��ת
		model = glm::rotate(model, glm::radians((float)glfwGetTime() * 40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	
		planetShader.setMat4("model", model);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);

		planetModel.Draw(planetShader);

		//��Ⱦ�ɴ�
		shipShader.use();

		//�����Դ���������λ��ֵ
		shipShader.setVec3("lightPos", glm::vec3(1.0f, 0.0f, 0.0f));

		shipShader.setVec3("viewPos", camera.Position);

		//����ͶӰ�͹۲����
		shipShader.setMat4("projection", projection);
		shipShader.setMat4("view", view);

		//����������Ӱ�õ�ͶӰԶƽ��ֵ
		shipShader.setFloat("far_plane", far_plane);

		model = glm::mat4(1.0f);
		model = glm::translate(model, camera.getPosition() + camera.getFront());
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		shipShader.setMat4("model", model);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);

		shipModel.Draw(shipShader);

		//��Ⱦ������
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 20.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		shipShader.setMat4("model", model);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);

		portalModel.Draw(shipShader);

		//��Ⱦ����
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		planeShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sunTexture);
		{
			glm::vec3 v = camera.Position - sun.transform.position;
			float lengthV = glm::length(v);
			float xRotation = glm::asin(v[1] / lengthV);
			sun.transform.rotation = { xRotation, v[0] / lengthV, v[2] / lengthV };
		}

		sun.render(camera, planeShader);

		//��Ⱦ����Ч��
		pars.position = glm::vec3(planetX + 0.2f, 3.0f, planetZ + 0.2f);

		pars.update();
		pars.render(camera, planeShader);

		glDisable(GL_BLEND);

		//������ײ���
		bool collisionX = (camera.getPosition() + camera.getFront()).x + shipSize >= -1.0 && 1.0 >= (camera.getPosition() + camera.getFront()).x - shipSize;
		bool collisionY = (camera.getPosition() + camera.getFront()).y + shipSize >= -1.0 && 1.0 >= (camera.getPosition() + camera.getFront()).y - shipSize;
		bool collisionZ = (camera.getPosition() + camera.getFront()).z + shipSize >= 19.0 && 21.0 >= (camera.getPosition() + camera.getFront()).z - shipSize;

		if (collisionX && collisionY && collisionZ) {
			camera.transport(glm::vec3(portalOutX, 0.0f, portalOutZ));
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

// �����������ĺ���
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	//��ס��shift����ʹ�ɴ����ƶ�����
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.ProcessKeyboard(SHIFT_PRESS, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		camera.ProcessKeyboard(SHIFT_RELEASE, deltaTime);

	//��F����ʹ�ɴ������Ǳ�����Ծ�ֹ
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		camera.ProcessKeyboard(LANDING, deltaTime);
}

// �����ڴ�С�����ĺ���
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// ��������ƶ��¼��ĺ���
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// ���������ֹ����¼��ĺ���
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

//������պ�����ĺ���
unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
