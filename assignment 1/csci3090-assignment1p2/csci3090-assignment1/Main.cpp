#include "graphics/Window.h"
#include "Shaders.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

glm::mat4 blendMat;
GLuint vao0;
GLuint vao1;
GLuint vao2;
GLuint vao3;
GLuint vao4;
GLuint vao5;

GLuint program;
int numPoints = 0;
std::vector<glm::vec3> filePoints;

glm::mat4 projection;
float eyex, eyey, eyez;

glm::mat4 make_blend(GLclampf tension)
{
	glm::mat4 result = glm::mat4(0);

	GLfloat s = 0.5 * (1 - tension);

	result[0][0] = 0;
	result[0][1] = -s;
	result[0][2] = 2 * s;
	result[0][3] = -s;

	result[1][0] = 1;
	result[1][1] = 0;
	result[1][2] = s - 3;
	result[1][3] = 2 - s;

	result[2][0] = 0;
	result[2][1] = s;
	result[2][2] = 3 - 2 * s;
	result[2][3] = s - 2;

	result[3][0] = 0;
	result[3][1] = 0;
	result[3][2] = -s;
	result[3][3] = s;

	return result;
}

std::string read_file(const char *a_filepath)
{
	FILE *file = fopen(a_filepath, "rt");
	fseek(file, 0, SEEK_END);
	unsigned long length = ftell(file);
	char *data = new char[length + 1];
	memset(data, 0, length + 1);
	fseek(file, 0, SEEK_SET);
	fread(data, 1, length, file);
	fclose(file);

	std::string result(data);
	delete[] data;
	return result;
}

std::vector<glm::vec3> load_points(const char *filename)
{
	std::vector<glm::vec3> result;
	std::string file = read_file(filename);
	std::istringstream dataStream (file);
	int numPoints;
	dataStream >> numPoints;
	for (int i = 0; i < numPoints; i++)
	{
		float x, y, z;
		dataStream >> x >> y >> z;
		result.push_back(glm::vec3(x, y, z));
	}
	return result;
}

void genVao(GLclampf tension, GLuint &vao)
{
	std::vector<glm::vec3> points;

	blendMat = make_blend(tension);

	for(int i = 0; i < filePoints.size() - 3; i++)
	{	
		glm::vec3 p1 = filePoints[i + 0];
		glm::vec3 p2 = filePoints[i + 1];
		glm::vec3 p3 = filePoints[i + 2];
		glm::vec3 p4 = filePoints[i + 3];

		glm::vec4 px(p1.x, p2.x, p3.x, p4.x);
		glm::vec4 py(p1.y, p2.y, p3.y, p4.y);
		glm::vec4 pz(p1.z, p2.z, p3.z, p4.z);

		glm::vec4 uVec;

		for (float u = 0.0f; u <= 1.0f; u += 0.1)
		{
			uVec = glm::vec4(1.0f, u, u*u, u*u*u);
			glm::vec4 uB = uVec*blendMat;
			float xVal = glm::dot(uB, px);
			float yVal = glm::dot(uB, py);
			float zVal = glm::dot(uB, pz);
			points.push_back(glm::vec3(xVal, yVal, zVal));
		}
	}

	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	numPoints = points.size();
	GLuint vBuffer;
	glGenBuffers(1, &vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, numPoints * sizeof(glm::vec3), &points[0], GL_STATIC_DRAW);

	GLint position_attribute = glGetAttribLocation(program, "position");
	glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(position_attribute);
}

void init()
{
	eyex = 1.5f;
	eyey = 1.5f;
	eyez = 1.5f;

	filePoints = load_points("points.txt");

	int vs, fs;
	vs = buildShader(GL_VERTEX_SHADER, "example5.vs");
	fs = buildShader(GL_FRAGMENT_SHADER, "example5.fs");
	program = buildProgram(vs, fs, 0);
	dumpProgram(program, "example 5");

	glGenVertexArrays(1, &vao0);
	glBindVertexArray(vao0);

	numPoints = filePoints.size();
	GLuint vBuffer;
	glGenBuffers(1, &vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, numPoints * sizeof(glm::vec3), &filePoints[0], GL_STATIC_DRAW);

	GLint position_attribute = glGetAttribLocation(program, "position");
	glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(position_attribute);

	genVao(0.0f, vao1);
	genVao(0.25f, vao2);
	genVao(0.5f, vao3);
	genVao(0.75f, vao4);
	genVao(1.0f, vao5);
	
	projection = glm::perspective(45.0f, (float)800 / (float)600, 0.1f, 100.0f);
}

void display()
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 viewPerspective;

	int vpLoc;
	int modelLoc;
	int colourLoc;

	GLint vPosition;

	view = glm::lookAt(glm::vec3(eyex, eyey, eyez),
		glm::vec3(-1,-1, 0),
		glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat3 normal = glm::transpose(glm::inverse(glm::mat3(view)));
	viewPerspective = projection * view;

	model = glm::mat4(1.0);

	glUseProgram(program);
	vpLoc = glGetUniformLocation(program, "viewPerspective");
	glUniformMatrix4fv(vpLoc, 1, 0, glm::value_ptr(viewPerspective));

	modelLoc = glGetUniformLocation(program, "model");
	colourLoc = glGetUniformLocation(program, "colour");

	glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));

	glUniform4f(colourLoc, 1.0, 0.0, 0.0, 1.0);
	glBindVertexArray(vao1);
	glDrawArrays(GL_LINE_STRIP, 0, numPoints);

	glUniform4f(colourLoc, 0.0, 1.0, 0.0, 1.0);
	glBindVertexArray(vao2);
	glDrawArrays(GL_LINE_STRIP, 0, numPoints);

	glUniform4f(colourLoc, 0.0, 0.0, 1.0, 1.0);
	glBindVertexArray(vao3);
	glDrawArrays(GL_LINE_STRIP, 0, numPoints);

	glUniform4f(colourLoc, 1.0, 1.0, 0.0, 1.0);
	glBindVertexArray(vao4);
	glDrawArrays(GL_LINE_STRIP, 0, numPoints);

	glUniform4f(colourLoc, 0.0, 1.0, 1.0, 1.0);
	glBindVertexArray(vao5);
	glDrawArrays(GL_LINE_STRIP, 0, numPoints);

	glUniform4f(colourLoc, 1.0, 0.0, 1.0, 1.0);
	glBindVertexArray(vao0);
	glDrawArrays(GL_POINTS, 0, filePoints.size());
}

int main()
{
	using namespace graphics;

	Window window("CSCI3090 - Assignment 1", 800, 600);
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
	glEnable(GL_POINT_SMOOTH);

	glLineWidth(2.0f);
	glPointSize(5.0f);

	init();	

	while (!window.Closed())
	{
		window.Clear();
		if (window.GetKey(GLFW_KEY_Q)) break;

		display();

		window.Update();
	}
	return 0;
}