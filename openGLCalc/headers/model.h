#pragma once

#include <iostream>
#include <glad/glad.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>

#include "shader.h"
#include "stb_image.h"

using std::cout, std::endl, std::string, std::vector;

//COMMENT IT OUT UNTIL YOU DONT GET IT

//functions
unsigned int TextureFromFile(const char* path, const string& directory);

//structs
struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};




struct Texture { // OH GOD
	unsigned int id;
	string type;
	string path;
};




//class? REVISE THE WHOLE OF MODEL LOADING
class Mesh {
public:
	//render data
	unsigned int VAO, VBO, EBO;

	//mesh data
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture>textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		setUpMesh();
	}

	void Draw(Shader& shader)
	{
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		//shader.use(); //controversial lol
		for (unsigned int i = 0; i < textures.size(); i++)
		{

			glActiveTexture(GL_TEXTURE0 + i);

			string number;
			string name = textures[i].type;

			if (name == "texture_diffuse")  number = std::to_string(diffuseNr++);
			else if (name == "texture_specular") number = std::to_string(specularNr++);

			shader.setInt(("material." + name + number).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);

		}
		glActiveTexture(GL_TEXTURE0);

		//Draw mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	void DrawInstanced(Shader& shader, unsigned int instancesNr)
	{
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		//shader.use(); //controversial lol
		for (unsigned int i = 0; i < textures.size(); i++)
		{

			glActiveTexture(GL_TEXTURE0 + i);

			string number;
			string name = textures[i].type;

			if (name == "texture_diffuse")  number = std::to_string(diffuseNr++);
			else if (name == "texture_specular") number = std::to_string(specularNr++);

			shader.setInt(("material." + name + number).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);

		}
		glActiveTexture(GL_TEXTURE0);

		//Draw mesh
		glBindVertexArray(VAO);
		glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instancesNr);
		glBindVertexArray(0);
	}

private:


	void setUpMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);//bytes of one vertice * nr vertices, start point, draw type

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

		//vertex positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		//normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		glEnableVertexAttribArray(1);
		//texture attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}
};

class Model {
public:

	//model data
	vector<Mesh> meshes;
	vector<Texture> textures_loaded;
	string directory;
	unsigned int InstancesNr;

	Model(const char* path)
	{
		loadModel(path);
	}
	void Draw(Shader& shader) {

		for (unsigned int i = 0; i < meshes.size(); i++) {

			meshes[i].Draw(shader);
		}
	}

	void setupInstanceBuffer(unsigned int instancesNr, glm::mat4* modelMatrices) {

		InstancesNr = instancesNr;

		//vertex buffer object boo
		unsigned int buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, instancesNr * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW); //try to simplify

		delete[] modelMatrices;

		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			unsigned int VAO = meshes[i].VAO;
			glBindVertexArray(VAO);
			//vertex attributes
			std::size_t v4s = sizeof(glm::vec4);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * v4s, (void*)0);
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * v4s, (void*)(1 * v4s));
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * v4s, (void*)(2 * v4s));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * v4s, (void*)(3 * v4s));
			glEnableVertexAttribArray(6);

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);

			glBindVertexArray(0);

		}
	}

	void DrawInstanced(Shader& shader) {


		for (unsigned int i = 0; i < meshes.size(); i++) {

			meshes[i].DrawInstanced(shader, InstancesNr);
		}
	}


private:





	void loadModel(string path) {
		Assimp::Importer importer; //make an importer object from AI namespace
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_OptimizeMeshes); //read the content

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
			return;
		}

		directory = path.substr(0, path.find_last_of('/'));
		processNode(scene->mRootNode, scene);
	}


	void processNode(aiNode* node, const aiScene* scene) {
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			processNode(node->mChildren[i], scene);
		}

	}


	Mesh processMesh(aiMesh* mesh, const aiScene* scene) {

		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture> textures;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			//Process vertex positions, normal and texture coordinates
			glm::vec3 vector;
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;

			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;

			if (mesh->mTextureCoords[0]) {//if the mesh contains texcoords
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else {
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			}

			vertices.push_back(vertex);
		}


		//process indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];

			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}

		//process material
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		return Mesh(vertices, indices, textures);
	}

	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName) {

		stbi_set_flip_vertically_on_load(true);
		vector<Texture> textures;

		unsigned int texCount = mat->GetTextureCount(type);

		if (texCount == 0)  cout << "WARNING::MODEL(" << directory << ")::TEXTURES::no textures of type " << typeName << " were found for this model" << endl;

		if (type == 1 and texCount == 0) {
			Texture texture;
			texture.id = TextureFromFile("error.png", "textures");
			texture.type = "texture_diffuse";
			texture.path = "textures/error.png";
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}

		for (unsigned int i = 0; i < texCount; i++) {

			aiString str;
			mat->GetTexture(type, i, &str);
			bool skip = false;

			for (unsigned int j = 0; j < textures_loaded.size(); j++) {
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip) {
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture);
			}
		}

		return textures;

	}

};



unsigned int TextureFromFile(const char* path, const string& directory) {
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format = GL_RGB;

		if (nrComponents == 1) { format = GL_RED; glPixelStorei(GL_UNPACK_ALIGNMENT, 1); }
		else if (nrComponents == 3) { format = GL_RGB; glPixelStorei(GL_UNPACK_ALIGNMENT, 1); }
		else if (nrComponents == 4) { format = GL_RGBA; glPixelStorei(GL_UNPACK_ALIGNMENT, 4); }


		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		cout << "Texture failed to load at path: " << path << endl;
		stbi_image_free(data);
	}

	return textureID;
}