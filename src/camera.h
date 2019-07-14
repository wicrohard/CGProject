#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// 用于摄像机移动的枚举量
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
	SHIFT_PRESS,
	SHIFT_RELEASE,
	LANDING
};

//欧拉角
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
//移动速度
const float SPEED       =  10.0f;
//鼠标移动对角度的影响大小
const float SENSITIVITY =  0.1f;
//缩放变量
const float ZOOM        =  45.0f;


class Camera
{
public:
    // 摄像机基本属性
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // 欧拉角
    float Yaw;
    float Pitch;
    // 用于调整摄像机控制的变量
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // 用glm vec来初始化
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // 用标量属性值来初始化
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // 返回view矩阵
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // 处理键盘输入，用于控制摄像机的移动
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
		//wasd控制方向移动
		if (direction == FORWARD) {
			Position += Front * velocity;
			if (Position.x > 58.0 || Position.x < -58.0 || Position.y > 58.0 || Position.y < -58.0 || Position.z > 58.0 || Position.z < -58.0) {
				Position -= Front * velocity;
			}
		}
		if (direction == BACKWARD) {
			Position -= Front * velocity;
			if (Position.x > 58.0 || Position.x < -58.0 || Position.y > 58.0 || Position.y < -58.0 || Position.z > 58.0 || Position.z < -58.0) {
				Position += Front * velocity;
			}
		}
		if (direction == LEFT) {
			Position -= Right * velocity;
			if (Position.x > 58.0 || Position.x < -58.0 || Position.y > 58.0 || Position.y < -58.0 || Position.z > 58.0 || Position.z < -58.0) {
				Position += Right * velocity;
			}
		}
		if (direction == RIGHT) {
			Position += Right * velocity;
			if (Position.x > 58.0 || Position.x < -58.0 || Position.y > 58.0 || Position.y < -58.0 || Position.z > 58.0 || Position.z < -58.0) {
				Position -= Right * velocity;
			}
		}
		//按住左shift来使飞船移动加速
		if (direction == SHIFT_PRESS) {
			MovementSpeed = 20.0f;
		}
		if (direction == SHIFT_RELEASE) {
			MovementSpeed = 10.0f;
		}
    }

    // 处理鼠标移动输入，用于控制摄像机的视角
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        updateCameraVectors();
    }

    // 处理鼠标滚轮输入，用于视角缩放
    void ProcessMouseScroll(float yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;
    }

	//这两个方法用于物体更新位置来固定在摄像机镜头中间
	glm::vec3 getPosition() {
		return Position;
	}

	glm::vec3 getFront() {
		return Front;
	}

	//这个方法用来使摄像机（飞船）进行传送
	void transport(glm::vec3 newPosition) {
		Position = newPosition;
	}

private:
    // 更新摄像机的Front
    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		//记录当前飞船的位置，用来更新摄像机的位置
		glm::vec3 shipPos = Position + Front;
        Front = glm::normalize(front);

        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up    = glm::normalize(glm::cross(Right, Front));

		//根据新的Front和之前记录的飞船位置来更新摄像机位置，以实现第三人称视角
		Position = shipPos - Front;
    }
};
#endif