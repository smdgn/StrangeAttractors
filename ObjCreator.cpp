#include "ObjCreator.h"


glm::vec3 ObjCreator::computeNormal(obj::Triangle * triangle)
{
	glm::vec3 vertices[3];
	for (int i = 0; i < 3; i++) {
		vertices[i] = glm::vec3(triangle->vertex[i].pos.x, triangle->vertex[i].pos.y, triangle->vertex[i].pos.z);
	}
	glm::vec3 A = vertices[1] - vertices[0];
	glm::vec3 B = vertices[2] - vertices[0];
	return glm::normalize(glm::cross(A, B));
}

void ObjCreator::init()
{
	if (VaoObj == 0) {
		glGenVertexArrays(1, &VaoObj);
		glBindVertexArray(VaoObj);
	}
	else glBindVertexArray(VaoObj);
}


modelObj ObjCreator::create(obj::Model* model, float angle)
{
	std::vector<glm::vec3> triangle;
	std::vector<glm::vec3> normals;

	this->angle = angle;
	this->model = model;

	for (unsigned int i = 0; i < model->GetTriangleCount(); ++i)
	{
		obj::Triangle* tr = model->GetTriangle(i);
		obj::Model::PrimitiveCollection AdjTri;
		glm::vec3 normal = computeNormal(tr);
		for (int i = 0; i < 3; i++) {
			triangle.push_back(glm::vec3(tr->vertex[i].pos.x, tr->vertex[i].pos.y, tr->vertex[i].pos.z));  //populate the Vertex Array
			model->GetAdjacentTriangles(AdjTri, &tr->vertex[i]); //Get every adjacent Triangle for every Vertex

			std::vector<glm::vec3> tempNormals; //temporal normals for each face

			tempNormals.push_back(glm::normalize(normal));
			for (auto& neighbourTriangle : AdjTri) {
				bool passed = true;
				glm::vec3 neighbourNormal = computeNormal(neighbourTriangle);
				if (std::acos(glm::dot(neighbourNormal, normal))*180.0f / PI < angle) { //angle
					for (auto cmp : tempNormals) {
						if (glm::length(cmp - neighbourNormal) < EPSILON) {
							passed = false;
						}
					}
				}
				else passed = false;
				if (passed == true) tempNormals.push_back(glm::normalize(neighbourNormal));
			}
			normals.push_back(glm::normalize(std::accumulate(tempNormals.begin(), tempNormals.end(), decltype(tempNormals)::value_type(0) / (float)tempNormals.size())));
			AdjTri.clear();
		}
	}
	return modelObj{ normals, triangle };
}

modelObj ObjCreator::createCylinder(float radius, float h, int tesselate)
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> textcords;
	float angle;
	float nextAngle;
	float height = h / 2.0f;

	float x, z;
	float xnext, znext;

	for (int i = 0; i <= tesselate; i++) {
		angle = i * 2.0*PI / (tesselate);
		nextAngle = (i + 1) * 2.0f*PI / (tesselate);
		float theta = float(i) / tesselate;
		float nextTheta = float(i + 1) / tesselate;

		x = radius * cos(angle);
		z = radius * sin(angle);

		xnext = radius * cos(nextAngle);
		znext = radius * sin(nextAngle);

		vertices.push_back(glm::vec3(xnext, -height, znext));  //Mantel
		vertices.push_back(glm::vec3(x, -height, z));
		vertices.push_back(glm::vec3(x, height, z));

		vertices.push_back(glm::vec3(x, height, z));  //Mantel
		vertices.push_back(glm::vec3(xnext, height, znext));
		vertices.push_back(glm::vec3(xnext, -height, znext));

		vertices.push_back(glm::vec3(xnext, -height, znext));
		vertices.push_back(glm::vec3(0.0f, -height, 0.0f));
		vertices.push_back(glm::vec3(x, -height, z)); //Top Triangles

		vertices.push_back(glm::vec3(x, height, z));
		vertices.push_back(glm::vec3(0.0f, height, 0.0f));
		vertices.push_back(glm::vec3(xnext, height, znext)); //Bottom Triangles

		glm::vec3 a = glm::vec3(x, -height, z) - glm::vec3(xnext, -height, znext);
		glm::vec3 b = glm::vec3(x, height, z) - glm::vec3(xnext, -height, znext);
		for (int i = 0; i < 3; i++) normals.push_back(glm::normalize(glm::cross(a, b)));

		a = glm::vec3(xnext, height, znext) - glm::vec3(x, height, z);
		b = glm::vec3(xnext, -height, znext) - glm::vec3(x, height, z);
		for (int i = 0; i < 3; i++) normals.push_back(glm::normalize(glm::cross(a, b)));

		a = glm::vec3(0.0f, -height, 0.0f) - glm::vec3(xnext, -height, znext);
		b = glm::vec3(x, -height, z) - glm::vec3(xnext, -height, znext);
		for (int i = 0; i < 3; i++) normals.push_back(glm::normalize(glm::cross(a, b)));

		a = glm::vec3(0.0f, height, 0.0f) - glm::vec3(x, height, z);
		b = glm::vec3(xnext, height, znext) - glm::vec3(x, height, z);
		for (int i = 0; i < 3; i++) normals.push_back(glm::normalize(glm::cross(a, b)));

		textcords.push_back(glm::vec2(nextTheta, 0));
		textcords.push_back(glm::vec2(theta, 0));
		textcords.push_back(glm::vec2(theta, 1));

		textcords.push_back(glm::vec2(theta, 1));
		textcords.push_back(glm::vec2(nextTheta, 1));
		textcords.push_back(glm::vec2(nextTheta, 0));

		for (int i = 0; i < 6; i++) {
			textcords.push_back(glm::vec2(0.0f, 0.0f));
		}

	}
	return modelObj{ normals, vertices, textcords };
}

modelObj ObjCreator::createCube(float size)
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;

	vertices = { glm::vec3(-1.0f, 1.0f, -1.0f),
		glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f, 1.0f, -1.0f),
		glm::vec3(-1.0f, 1.0f, -1.0f),

		glm::vec3(-1.0f, -1.0f, 1.0f),
		glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(-1.0f, 1.0f, -1.0f),
		glm::vec3(-1.0f, 1.0f, -1.0f),
		glm::vec3(-1.0f, 1.0f, 1.0f),
		glm::vec3(-1.0f, -1.0f, 1.0f),

		glm::vec3(1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f, -1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, -1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f),

		glm::vec3(-1.0f, -1.0f, 1.0f),
		glm::vec3(-1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, -1.0f, 1.0f),
		glm::vec3(-1.0f, -1.0f, 1.0f),

		glm::vec3(-1.0f, 1.0f, -1.0f),
		glm::vec3(1.0f, 1.0f, -1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(-1.0f, 1.0f, 1.0f),
		glm::vec3(-1.0f, 1.0f, -1.0f),

		glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(-1.0f, -1.0f, 1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f),
		glm::vec3(-1.0f, -1.0f, 1.0f),
		glm::vec3(1.0f, -1.0f, 1.0f) };

	for (int i = 0; i < 31; i += 6) {
		glm::vec3 a = vertices[i + 1] - vertices[i];
		glm::vec3 b = vertices[i + 2] - vertices[i];
		glm::vec3 normal = glm::normalize(glm::cross(a, b));
		for (int i = 0; i < 6; i++) normals.push_back(normal);
	}
	for (auto& vertex : vertices) vertex = vertex * size;
	return modelObj{ normals, vertices };
}

void ObjCreator::bindBuffers(modelObj & objM)
{
	vertCount = objM.vertices.size();

	/*glGenVertexArrays(1, &VaoObj);
	glBindVertexArray(VaoObj);

	// Erzeuge das VBO
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	//Kopiere die normals ins VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) *objM.normals.size(), objM.normals.data(), GL_DYNAMIC_DRAW);

	glGenBuffers(1, &geometryObj);
	glBindBuffer(GL_ARRAY_BUFFER, geometryObj);
	//Kopiere die vertices ins VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) *objM.vertices.size() , objM.vertices.data(), GL_DYNAMIC_DRAW);


	// Shader Attribute
	glEnableVertexAttribArray(GLT_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, geometryObj);
	glVertexAttribPointer(GLT_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(GLT_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glVertexAttribPointer(GLT_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);*/

	if (objM.vertices.size() != 0) bindVertexData(objM.vertices);
	if (objM.colors.size() != 0) bindColorData(objM.colors);
	if (objM.normals.size() != 0) bindNormalData(objM.normals);
	if (objM.texturecords.size() != 0) bindTextureData(objM.texturecords);

	//VAO wieder unbinden
	glBindVertexArray(0);

}

void ObjCreator::bindBuffers(std::vector<glm::vec3>& container) {
	vertCount = container.size();

	glGenVertexArrays(1, &VaoObj);
	glBindVertexArray(VaoObj);

	glGenBuffers(1, &geometryObj);
	glBindBuffer(GL_ARRAY_BUFFER, geometryObj);
	//Kopiere die vertices ins VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* container.size(), container.data(), GL_DYNAMIC_DRAW);


	// Shader Attribute
	glEnableVertexAttribArray(GLT_ATTRIBUTE_VERTEX);
	//glBindBuffer(GL_ARRAY_BUFFER, geometryObj);
	glVertexAttribPointer(GLT_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	//VAO wieder unbinden
	glBindVertexArray(0);

}
void ObjCreator::bindVertexData(std::vector<glm::vec3>& vertices)
{
	vertCount = vertices.size();

	init();

	glGenBuffers(1, &geometryObj);
	glBindBuffer(GL_ARRAY_BUFFER, geometryObj);
	//Kopiere die vertices ins VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);

	// Shader Attribute
	glEnableVertexAttribArray(GLT_ATTRIBUTE_VERTEX);
	//glBindBuffer(GL_ARRAY_BUFFER, geometryObj);
	glVertexAttribPointer(GLT_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	//VAO wieder unbinden
	glBindVertexArray(0);

}
void ObjCreator::bindColorData(std::vector<glm::vec4>& color)
{
	init();
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4)* color.size(), color.data(), GL_DYNAMIC_DRAW);

	// Shader Attribute
	glEnableVertexAttribArray(GLT_ATTRIBUTE_COLOR);
	//glBindBuffer(GL_ARRAY_BUFFER, geometryObj);
	glVertexAttribPointer(GLT_ATTRIBUTE_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), 0);

	//VAO wieder unbinden
	glBindVertexArray(0);

}
void ObjCreator::bindNormalData(std::vector<glm::vec3>& normals)
{
	init();
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	//Kopiere die vertices ins VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* normals.size(), normals.data(), GL_DYNAMIC_DRAW);

	// Shader Attribute
	glEnableVertexAttribArray(GLT_ATTRIBUTE_NORMAL);
	//glBindBuffer(GL_ARRAY_BUFFER, geometryObj);
	glVertexAttribPointer(GLT_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	//VAO wieder unbinden
	glBindVertexArray(0);
}
void ObjCreator::bindTextureData(std::vector<glm::vec2>& texture)
{
	init();
	glGenBuffers(1, &textureBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
	//Kopiere die vertices ins VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)* texture.size(), texture.data(), GL_DYNAMIC_DRAW);

	// Shader Attribute
	glEnableVertexAttribArray(GLT_ATTRIBUTE_TEXTURE0);
	//glBindBuffer(GL_ARRAY_BUFFER, geometryObj);
	glVertexAttribPointer(GLT_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);

	//VAO wieder unbinden
	glBindVertexArray(0);
}

modelObj ObjCreator::update(float angle) //updates normals, for vertice updates, create and load new obj
{
	modelObj retmodel;
	if (geometryObj != 0 && normalBuffer != 0) {
		retmodel = create(model, angle);
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3)*vertCount, retmodel.normals.data());
	}
	this->angle = angle;

	return retmodel;
}

void ObjCreator::draw(GLenum mode)
{
	glBindVertexArray(VaoObj);
	//glBindBuffer(GL_ARRAY_BUFFER, geometryObj);
	//glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glDrawArrays(mode, 0, vertCount);
	glBindVertexArray(0);
}

ObjCreator::~ObjCreator() {
	model = NULL;
	glDeleteVertexArrays(1, &VaoObj);
	glDeleteBuffers(1, &normalBuffer);
	glDeleteBuffers(1, &geometryObj);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteBuffers(1, &textureBuffer);
}

Material setMaterialType(MaterialType mat) {
	switch (mat) {
	case(BRONZE):
		return Material{ glm::vec4(0), glm::vec4(0.2125f, 0.1275f, 0.054f, 1.0f), glm::vec4(0.714f, 0.4284f, 0.18144f, 1.0f), glm::vec4(0.393548f, 0.271906f, 0.166721f, 1.0f), 25.6f };
	case(SILVER):
		return Material{ glm::vec4(0), glm::vec4(0.19225f, 0.19225f, 0.19225f, 1.0f), glm::vec4(0.50754f, 0.50754f, 0.50754f, 1.0f), glm::vec4(0.508273f, 0.508273f, 0.508273f, 1.0f), 51.2f };
	case(GOLD):
		return Material{ glm::vec4(0), glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f), glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f), glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f), 51.2f };
	case(JADE):
		return Material{ glm::vec4(0), glm::vec4(0.135f, 0.2225f, 0.1575f, 0.95f), glm::vec4(0.54f, 0.89f, 0.63f, 0.95f), glm::vec4(0.316228f, 0.316228f, 0.316228f, 0.95f), 12.8f };
	case(OBSIDIAN):
		return Material{ glm::vec4(0), glm::vec4(0.05375f, 0.05f, 0.06625f, 0.82f), glm::vec4(0.18275f, 0.17f, 0.22525f, 0.82f), glm::vec4(0.332741f, 0.328634f, 0.346435f, 0.82f), 38.4f };
	case(PERL):
		return Material{ glm::vec4(0), glm::vec4(0.25f, 0.20725f, 0.20725f, 0.922f), glm::vec4(1.0f, 0.829f, 0.829f, 0.922f), glm::vec4(0.296648f, 0.296648f, 0.296648f, 0.922f), 11.264f };
	case(RUBY):
		return Material{ glm::vec4(0), glm::vec4(0.1745f, 0.01175f, 0.01175f, 0.55f), glm::vec4(0.61424f, 0.04136f, 0.04136f, 0.55f), glm::vec4(0.727811f, 0.626959f, 0.626959f, 0.55f), 76.8f };
	case(GREEN_RUBBER):
		return Material{ glm::vec4(0), glm::vec4(0.0f,0.05f,0.0f,1.0f), glm::vec4(0.4f,0.5f,0.4f,1.0f), glm::vec4(0.04f,0.7f,0.04f,1.0f), 10.0f };
	default:
		return  Material{ glm::vec4(0.0f), glm::vec4(0.0f), glm::vec4(0.0f), glm::vec4(0.0f), 0.0f };  //empty mat
	}

}

Light initLight()
{
	return Light{ glm::vec4(5.0f,5.0f,0.0f,1.0f), glm::vec4(1), glm::vec4(1), glm::vec4(1) };
}
