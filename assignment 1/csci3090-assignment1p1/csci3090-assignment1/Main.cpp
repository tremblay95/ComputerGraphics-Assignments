
#include "ply/readply.h"
#include "graphics/Window.h"
#include "Shaders.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <vector>

ply_model *bunny;

unsigned int triangles;

GLuint program;
GLuint bunnyVAO;
GLuint vBuffer;
GLuint iBuffer;

glm::mat4 projection;
glm::mat4 view;
glm::mat4 viewPerspective;
glm::mat3 normal;

float eyex, eyey, eyez;
float angle = 0.0;


// compute normal of a face given 3 vertices
glm::vec3 compute_face_normal(const ply_vertex &v1, const ply_vertex &v2, const ply_vertex &v3)
{
	glm::vec3 vecA = glm::vec3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
	glm::vec3 vecB = glm::vec3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);

	return glm::normalize(glm::cross(vecA, vecB));
}

// compute normal of a vertex given the normals of all faces it belongs to
glm::vec3 compute_vertex_normal(const std::vector<glm::vec3> &faceNorms)
{
	glm::vec3 avg = glm::vec3(0);
	float numFace = faceNorms.size();
	for (int i = 0; i < numFace; i++)
	{
		avg += faceNorms[i];
	}
	return glm::normalize(avg / numFace);
}

void init()
{
	GLint vPosition;
	GLint vNormal;

	glm::vec3 *vertices;
	glm::vec3 *normals;
	unsigned int *indices;

	std::vector<glm::vec3> *faceNorms;

	int vs, fs;

	eyex = 0.25f; eyey = 0.25f; eyez = 0.25f;

	vs = buildShader(GL_VERTEX_SHADER, "example5.vs");
	fs = buildShader(GL_FRAGMENT_SHADER, "example5.fs");
	program = buildProgram(vs, fs, 0);
	dumpProgram(program, "example 5");

	glUseProgram(program);

	
	bunny = readply("bunny.ply");

	glGenVertexArrays(1, &bunnyVAO);
	glBindVertexArray(bunnyVAO);

	vertices = new glm::vec3[bunny->nvertex];
	normals = new glm::vec3[bunny->nvertex];
	faceNorms = new std::vector<glm::vec3>[bunny->nvertex];
	for (int i = 0; i < bunny->nvertex; i++)
	{
		ply_vertex v = bunny->vertices[i];
		vertices[i] = glm::vec3(v.x, v.y, v.z);
	}
	triangles = bunny->nface;
	indices = new unsigned int[triangles * 3];
	// add indices of each face to the list of indices
	// compute face normals
	for (int i = 0; i < triangles; i++)
	{
		unsigned int ind1 = bunny->faces[i].vertices[0];
		unsigned int ind2 = bunny->faces[i].vertices[1];
		unsigned int ind3 = bunny->faces[i].vertices[2];

		indices[i * 3 + 0] = ind1;
		indices[i * 3 + 1] = ind2;
		indices[i * 3 + 2] = ind3;

		glm::vec3 face = compute_face_normal(bunny->vertices[ind1], bunny->vertices[ind2], bunny->vertices[ind3]);
		faceNorms[ind1].push_back(face);
		faceNorms[ind2].push_back(face);
		faceNorms[ind3].push_back(face);
	}

	//compute vertex normals
	for (int i = 0; i < bunny->nvertex; i++)
	{
		normals[i] = compute_vertex_normal(faceNorms[i]);
	}

	glGenBuffers(1, &vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, bunny->nvertex * 2 * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, bunny->nvertex * sizeof(glm::vec3), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, bunny->nvertex * sizeof(glm::vec3), bunny->nvertex * sizeof(glm::vec3), normals);

	glGenBuffers(1, &iBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles * 3 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	vPosition = glGetAttribLocation(program, "vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	vNormal = glGetAttribLocation(program, "vNormal");
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(bunny->nvertex * sizeof(glm::vec3)));
	glEnableVertexAttribArray(vNormal);

	projection = glm::perspective(45.0f, (float)800/(float)600, 0.1f, 100.0f);
	view = glm::lookAt(glm::vec3(eyex, eyey, eyez),
		glm::vec3(0.0f, 0.1f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	normal = glm::transpose(glm::inverse(glm::mat3(view)));
	viewPerspective = projection * view;

	delete[] faceNorms;
	delete[] normals;
	delete[] indices;
	delete[] vertices;
}

void display()
{
	glm::mat4 model;

	int vpLoc;
	int viewLoc;
	int modelLoc;
	int normalLoc;
	int colourLoc;

	GLint vPosition;

	model = glm::rotate(glm::mat4(1.0), angle, glm::vec3(0.0, 1.0, 0.0));

	glUseProgram(program);
	viewLoc = glGetUniformLocation(program, "view");
	glUniformMatrix4fv(viewLoc, 1, 0, glm::value_ptr(view));
	vpLoc = glGetUniformLocation(program, "viewPerspective");
	glUniformMatrix4fv(vpLoc, 1, 0, glm::value_ptr(viewPerspective));
	normalLoc = glGetUniformLocation(program, "normalMat");
	glUniformMatrix3fv(normalLoc, 1, 0, glm::value_ptr(normal));

	modelLoc = glGetUniformLocation(program, "model");
	colourLoc = glGetUniformLocation(program, "colour");
	vPosition = glGetAttribLocation(program, "vPosition");

	glUniform4f(colourLoc, 0.8, 0.2, 0.6, 1.0);

	glBindVertexArray(bunnyVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer);
	glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));
	glDrawElements(GL_TRIANGLES, triangles * 3, GL_UNSIGNED_INT, NULL);
}

int main()
{
	using namespace graphics;

	Window window("CSCI3090 - Assignment 1", 800, 600);
	glEnable(GL_DEPTH_TEST);

	init();

	glClearColor(0.2f, 0.1f, 0.3f, 1.0f);

	while (!window.Closed())
	{
		window.Clear();
		if (window.GetKey(GLFW_KEY_Q)) break;
		angle += 0.001;
		display();

		window.Update();
	}
	return 0;
}