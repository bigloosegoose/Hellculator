#pragma once

#include <iostream> //debug

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// I was confused so a self guide: ABC = Constants(defaults), Abc = Member variables(object vars), abc = input values

// also implement camera modes. Therefore, comeback

//list of possible movements
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

enum Camera_Mode {
	FPS,
	FLY,
	BTK
};


//default values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SENSITIVITY = 0.05f;
const float ZOOM = 45.0f;

class Camera {
public:


	//camera attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	//euler angles
	float Yaw;
	float Pitch;
	//camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;
	Camera_Mode Mode;

	//constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f,0.0f,-1.0f)),MovementSpeed(SPEED),MouseSensitivity(SENSITIVITY),Zoom(ZOOM)
	{
	
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	//constructor with scalars
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	glm::mat4 GetViewMatrix() {
		return glm::lookAt(Position, Position + Front, Up);
	}


	void SetCameraMode(Camera_Mode mode) {
		Mode = mode;
	}


	void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
		float velocity = MovementSpeed * deltaTime;

		if (Mode == FLY) 
		{
			if (direction == FORWARD)
				Position += Front * velocity;
			if (direction == BACKWARD)
				Position -= Front * velocity;
			if (direction == RIGHT)
				Position += Right * velocity;
			if (direction == LEFT)
				Position -= Right * velocity;
			if (direction == UP)
				Position += Up * velocity;
			if (direction == DOWN)
				Position -= Up * velocity;
		}

		if (Mode == FPS)
		{

			glm::vec3 fpsFront = glm::normalize(glm::vec3(Front.x, 0.0f, Front.z));

			if (direction == FORWARD)
				Position += fpsFront * velocity;
			if (direction == BACKWARD)
				Position -= fpsFront * velocity;
			if (direction == RIGHT)
				Position += Right * velocity;
			if (direction == LEFT)
				Position -= Right * velocity;
			//if (direction == UP)
				//Position += Up * velocity;
			//if (direction == DOWN)
				//Position -= Up * velocity;
		}

	}		

	void ProcessMouseMovement(float xoffset, float yoffset, float deltaTime ,GLboolean constrainPitch = true) {
		
		if (Mode != BTK) 
		{
			xoffset *= MouseSensitivity;
			yoffset *= MouseSensitivity;

			Yaw += xoffset;
			Pitch += yoffset;

			if (constrainPitch == true) {
				if (Pitch > 89.0f)  Pitch = 89.0f;
				if (Pitch < -89.0f) Pitch = -89.0f;
			}
			updateCameraVectors();
		}

		else if (Mode == BTK)
		{
			xoffset *= MouseSensitivity;
			yoffset *= MouseSensitivity;

			Yaw += xoffset;
			Pitch = 0.0f;
			updateCameraVectors();

			float velocity = MovementSpeed * deltaTime;
			Position += Front * velocity * yoffset;
		}
	}
	
	void ProcessMouseScroll(float yoffset) {
		Zoom -= (float)yoffset;
		if (Zoom > 45.0f) Zoom = 45.0f;
		if (Zoom < 1.0f)  Zoom =  1.0f;
	}


private:
	//calculates the front vector from the camera's updated euler angles

	void updateCameraVectors() {
		// calculate new front
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		//re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}

};