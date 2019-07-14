#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// ����������ƶ���ö����
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
	SHIFT_PRESS,
	SHIFT_RELEASE,
	LANDING
};

//ŷ����
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
//�ƶ��ٶ�
const float SPEED       =  10.0f;
//����ƶ��ԽǶȵ�Ӱ���С
const float SENSITIVITY =  0.1f;
//���ű���
const float ZOOM        =  45.0f;


class Camera
{
public:
    // �������������
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // ŷ����
    float Yaw;
    float Pitch;
    // ���ڵ�����������Ƶı���
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // ��glm vec����ʼ��
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // �ñ�������ֵ����ʼ��
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // ����view����
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // ����������룬���ڿ�����������ƶ�
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
		//wasd���Ʒ����ƶ�
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
		//��ס��shift��ʹ�ɴ��ƶ�����
		if (direction == SHIFT_PRESS) {
			MovementSpeed = 20.0f;
		}
		if (direction == SHIFT_RELEASE) {
			MovementSpeed = 10.0f;
		}
    }

    // ��������ƶ����룬���ڿ�����������ӽ�
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

    // �������������룬�����ӽ�����
    void ProcessMouseScroll(float yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;
    }

	//���������������������λ�����̶����������ͷ�м�
	glm::vec3 getPosition() {
		return Position;
	}

	glm::vec3 getFront() {
		return Front;
	}

	//�����������ʹ��������ɴ������д���
	void transport(glm::vec3 newPosition) {
		Position = newPosition;
	}

private:
    // �����������Front
    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		//��¼��ǰ�ɴ���λ�ã����������������λ��
		glm::vec3 shipPos = Position + Front;
        Front = glm::normalize(front);

        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up    = glm::normalize(glm::cross(Right, Front));

		//�����µ�Front��֮ǰ��¼�ķɴ�λ�������������λ�ã���ʵ�ֵ����˳��ӽ�
		Position = shipPos - Front;
    }
};
#endif