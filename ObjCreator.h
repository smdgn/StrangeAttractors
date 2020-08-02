#pragma once
#ifndef OBJCREATOR_H
#define OBJCREATOR_H

#include<glm.hpp>
#include<GLTools.h>
#include <vector>
#include <ObjLoader/Model.h>
#include <ObjLoader/Triangle.h>
#include <ObjLoader/Vertex.h>
#include <gtc/type_ptr.hpp>
#include <numeric>

#define PI 3.14159265
#define EPSILON 1e-3f

enum MaterialType { BRONZE = 0, SILVER, GOLD, JADE, OBSIDIAN, PERL, RUBY, GREEN_RUBBER };  //Some Material Presets...

struct Material {
	glm::vec4 mat_emissive;
	glm::vec4 mat_ambient;
	glm::vec4 mat_diffuse;
	glm::vec4 mat_specular;
	float shine;

	void sendToShader(GLuint shader) {   //Sends material values to the named shader. uniform names equal material variable names
		glUniform4fv(glGetUniformLocation(shader, "mat_emissive"), 1, glm::value_ptr(mat_emissive));
		glUniform4fv(glGetUniformLocation(shader, "mat_ambient"), 1, glm::value_ptr(mat_ambient));
		glUniform4fv(glGetUniformLocation(shader, "mat_diffuse"), 1, glm::value_ptr(mat_diffuse));
		glUniform4fv(glGetUniformLocation(shader, "mat_specular"), 1, glm::value_ptr(mat_specular));
		glUniform1f(glGetUniformLocation(shader, "shine"), shine);
	}
};

struct Light {
	glm::vec4 light_position;
	glm::vec4 light_ambient;
	glm::vec4 light_diffuse;
	glm::vec4 light_specular;

	void sendToShader(GLuint shader) {
		glUniform4fv(glGetUniformLocation(shader, "light_position"), 1, glm::value_ptr(light_position));
		glUniform4fv(glGetUniformLocation(shader, "light_diffuse"), 1, glm::value_ptr(light_diffuse));
		glUniform4fv(glGetUniformLocation(shader, "light_specular"), 1, glm::value_ptr(light_specular));
		glUniform4fv(glGetUniformLocation(shader, "light_ambient"), 1, glm::value_ptr(light_ambient));
	}

};

struct modelObj { //Collection of normals, Vertices, texturecords and color describing one 3D Model
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texturecords;
	std::vector<glm::vec4> colors;
};

Material setMaterialType(MaterialType mat);
Light initLight();

class ObjCreator {
private:
	//VBOS/VAO
	GLuint vertexBuffer;
	GLuint normalBuffer;
	GLuint textureBuffer;
	GLuint colorBuffer;

	GLuint VaoObj = 0;
	GLuint vertCount;

	void init();
public:
	//Some Model presets
	modelObj createCylinder(float radius, float height, int tesselate);
	modelObj createCube(float size);

	//Binding Ops
	void bindBuffers(modelObj&);
	void bindVertexData(std::vector<glm::vec3>&);
	void bindColorData(std::vector<glm::vec4>&);
	void bindNormalData(std::vector<glm::vec3>&);
	void bindTextureData(std::vector<glm::vec2>&);

	void draw(GLenum mode = GL_TRIANGLES);

	~ObjCreator();

};
#endif //!OBJCREATOR_H
