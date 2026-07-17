#pragma once

#include <glad/glad.h> //GET THOSE OPENGL HEADERS

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>



#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader
{
public:
	//the program ID
	unsigned int ID;
	unsigned int Vertex, Fragment, Geometry;


	//constructor
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr) {
		//get source code from file path
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		//make sure ifstream can throw errors
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			//open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// read file's buffer content into the streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			//close file handlers(as you do in other languages)
			vShaderFile.close();
			fShaderFile.close();
			//convert stream(byte stream?) into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch(std::ifstream::failure e) {
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();


		//Debugging shader compilation

		unsigned int vertex, fragment;
		int success;
		char infoLog[512];

		//vShader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);

		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		};
		Vertex = vertex;

		//fShader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);

		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		};
		Fragment = fragment;

		//#########################################################################################################
		//geometry shader wow

		if (geometryPath) {
			//get source code from file path
			std::string geometryCode;
			std::ifstream gShaderFile;
			//make sure ifstream can throw errors
			gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

			try {
				//open files
				gShaderFile.open(geometryPath);
				std::stringstream gShaderStream;
				// read file's buffer content into the streams
				gShaderStream << gShaderFile.rdbuf();
				//close file handlers(as you do in other languages)
				gShaderFile.close();
				//convert stream(byte stream?) into string
				geometryCode = gShaderStream.str();
			}
			catch (std::ifstream::failure e) {
				std::cout << "ERROR::SHADER::GEOMETRY::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
			}
			const char* gShaderCode = geometryCode.c_str();

			//Debugging shader compilation

			unsigned int geometry;
			int success;
			char infoLog[512];

			//gShader
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);

			glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(geometry, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
			}
			Geometry = geometry;

		}

		//#########################################################################################################

		//shader program
		ID = glCreateProgram();
		glAttachShader(ID, Vertex);
		glAttachShader(ID, Fragment);

		if (geometryPath) glAttachShader(ID, Geometry);

		glLinkProgram(ID);

		//Debugging shader linking

		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		};

		glDeleteShader(vertex);
		glDeleteShader(fragment);


	}

	void attachGeometryShader(const char* geometryPath) 
	{
		//get source code from file path
		std::string geometryCode;
		std::ifstream gShaderFile;
		//make sure ifstream can throw errors
		gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			//open files
			gShaderFile.open(geometryPath);
			std::stringstream gShaderStream;
			// read file's buffer content into the streams
			gShaderStream << gShaderFile.rdbuf();
			//close file handlers(as you do in other languages)
			gShaderFile.close();
			//convert stream(byte stream?) into string
			geometryCode = gShaderStream.str();
		}
		catch (std::ifstream::failure e) {
			std::cout << "ERROR::SHADER::GEOMETRY::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
		}
		const char* gShaderCode = geometryCode.c_str();

		//Debugging shader compilation

		unsigned int geometry;
		int success;
		char infoLog[512];

		//gShader
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);

		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(geometry, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		glAttachShader(ID, geometry);
		glLinkProgram(ID);

		//Debugging shader linking

		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::GEOMETRY::LINKING_FAILED\n" << infoLog << std::endl;
		};

		glDeleteShader(geometry);

	}

	// use or activate the shader
	void use() {
		glUseProgram(ID);
	}
	void glfwSetPointer(GLFWwindow* window) {
		glfwSetWindowUserPointer(window, &ID);
	}

	//utility uniform functions
	void setBool(const std::string& name, bool value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}

	void setInt(const std::string& name, int value)const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setFloat(const std::string& name, float value)const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setMat4(const std::string& name, glm::mat4 value)const {
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));  //uniform loc, no of matrices, no transpose, OpenGl friendly -> value_ptr(mat data)
	}

	void setVec3(const std::string& name, glm::vec3 vector)const {
		glUniform3f(glGetUniformLocation(ID, name.c_str()), vector.x, vector.y, vector.z);
	}
	void setVec3(const std::string& name, float xvalue, float yvalue, float zvalue)const {
		glUniform3f(glGetUniformLocation(ID, name.c_str()), xvalue, yvalue, zvalue);
	}
	void setVec3(const std::string& name, float value)const {
		glUniform3f(glGetUniformLocation(ID, name.c_str()), value, value, value);
	}
};
