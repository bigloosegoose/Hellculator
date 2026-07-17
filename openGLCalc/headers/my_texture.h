#pragma once

#include <glad/glad.h>
#include "stb_image.h"
#include <iostream>

GLenum globalTexType = GL_TEXTURE_2D;

//a bad but proud texture class made by battak for loading textures easily

class MTexture {
public:
	//texture id?
	unsigned int ID;


	MTexture(GLenum GLtexType, const char* imagePath, bool verticallyFlip, GLenum texUnit) {
		GLenum globalTexType = GLtexType;


		//create the texture
		glGenTextures(1, &ID);
		glActiveTexture(GL_TEXTURE0 + texUnit);
		glBindTexture(GLtexType, ID);

		//initializing
		glTexParameteri(GLtexType, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GLtexType, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GLtexType, GL_TEXTURE_WRAP_R, 0);
		glTexParameteri(GLtexType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GLtexType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_set_flip_vertically_on_load(verticallyFlip);

		int width, height, nrChannels;
		unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);

		//dynamically setting the image parameters so texture is loaded ideally
		if (data) {
			GLenum format = GL_RGB;

			if (nrChannels == 1) { format = GL_RED; glPixelStorei(GL_UNPACK_ALIGNMENT, 1); }
			else if (nrChannels == 3) { format = GL_RGB; glPixelStorei(GL_UNPACK_ALIGNMENT, 1); }
			else if (nrChannels == 4) { format = GL_RGBA; glPixelStorei(GL_UNPACK_ALIGNMENT, 4); }

			if (GLtexType == GL_TEXTURE_2D) {
				glTexImage2D(GLtexType, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GLtexType);
			}
			else {
				std::cout << "Texture Type(" << GLtexType << ") currently unspported in my_texture.h" << std::endl;
			}


		}
		else {
			std::cout << "ERROR::MTEXTURE::failed to load the texture image!" << std::endl;
		}
	}


};

void mtWrapMode(GLenum texWrapModeS, GLenum texWrapModeT, GLenum texWrapModeR) {
	glTexParameteri(globalTexType, GL_TEXTURE_WRAP_S, texWrapModeS);
	glTexParameteri(globalTexType, GL_TEXTURE_WRAP_T, texWrapModeT);
	glTexParameteri(globalTexType, GL_TEXTURE_WRAP_R, texWrapModeR);
}
void mtFilteringMode(GLenum texMinFilterMode, GLenum texMagFilterMode) {
	glTexParameteri(globalTexType, GL_TEXTURE_MIN_FILTER, texMinFilterMode);
	glTexParameteri(globalTexType, GL_TEXTURE_MAG_FILTER, texMagFilterMode);
}