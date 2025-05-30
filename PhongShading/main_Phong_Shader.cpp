#include <Windows.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/freeglut.h>

#define GLFW_INCLUDE_GLU
#define GLFW_DLL
#include <GLFW/glfw3.h>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <fstream>
#include <sstream>


using namespace glm;

GLuint load_shaders(const std::string& vertex_file_path, const std::string& fragment_file_path)
{
	GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

	// Get Vertex Shader Code
	std::string vertex_shader_code;
	std::ifstream vertex_shader_stream(vertex_file_path.c_str(), std::ios::in);
	if (vertex_shader_stream.is_open())
	{
		std::stringstream sstr;
		sstr << vertex_shader_stream.rdbuf();
		vertex_shader_code = sstr.str();
		vertex_shader_stream.close();
	}
	else
	{
		printf("Impossible to open %s.\n", vertex_file_path.c_str());
		getchar();
		return 0;
	}

	// Get Fragment Shader Code
	std::string fragment_shader_code;
	std::ifstream fragment_shader_stream(fragment_file_path.c_str(), std::ios::in);
	if (fragment_shader_stream.is_open())
	{
		std::stringstream sstr;
		sstr << fragment_shader_stream.rdbuf();
		fragment_shader_code = sstr.str();
		fragment_shader_stream.close();
	}
	else
	{
		printf("Impossible to open %s.\n", fragment_file_path.c_str());
		getchar();
		return 0;
	}

	GLint result = GL_FALSE;
	int info_log_length;

	// Compile Vertex Shader
	char const* vertex_source_pointer = vertex_shader_code.c_str();
	glShaderSource(vertex_shader_id, 1, &vertex_source_pointer, NULL);
	glCompileShader(vertex_shader_id);

	glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
	if (info_log_length > 0)
	{
		std::vector<char> vertex_shader_error_message(info_log_length + 1);
		glGetShaderInfoLog(vertex_shader_id, info_log_length, NULL, &vertex_shader_error_message[0]);
		printf("%s\n", &vertex_shader_error_message[0]);
	}

	// Compile Fragment Shader
	char const* fragment_source_pointer = fragment_shader_code.c_str();
	glShaderSource(fragment_shader_id, 1, &fragment_source_pointer, NULL);
	glCompileShader(fragment_shader_id);

	glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
	if (info_log_length > 0)
	{
		std::vector<char> fragment_shader_error_message(info_log_length + 1);
		glGetShaderInfoLog(fragment_shader_id, info_log_length, NULL, &fragment_shader_error_message[0]);
		printf("%s\n", &fragment_shader_error_message[0]);
	}

	// Link the Program
	GLuint program_id = glCreateProgram();
	glAttachShader(program_id, vertex_shader_id);
	glAttachShader(program_id, fragment_shader_id);
	glLinkProgram(program_id);

	glGetProgramiv(program_id, GL_LINK_STATUS, &result);
	glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
	if (info_log_length > 0)
	{
		std::vector<char> program_error_message(info_log_length + 1);
		glGetProgramInfoLog(program_id, info_log_length, NULL, &program_error_message[0]);
		printf("%s\n", &program_error_message[0]);
	}

	// Detach Shaders
	glDetachShader(program_id, vertex_shader_id);
	glDetachShader(program_id, fragment_shader_id);

	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);

	return program_id;
}

void update_mesh(const GLuint& vao, const std::vector<GLuint>& gl_buffers,
	const std::vector<vec3>& positions,
	const std::vector<vec3>& normals,
	const std::vector<unsigned int>& indices)
{
	glBindVertexArray(vao); // bind

	glBindBuffer(GL_ARRAY_BUFFER, gl_buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(positions[0]), &positions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, gl_buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(normals[0]), &normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_buffers[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0); // unbind
}

mat4 create_modeling_matrix(const float& sx, const float& sy, const float& sz, const float& tx, const float& ty, const float& tz) {
	mat4 ms = mat4(
		vec4(sx, 0, 0, 0),
		vec4(0, sy, 0, 0),
		vec4(0, 0, sz, 0),
		vec4(0, 0, 0, 1)
	);

	mat4 mt = mat4(
		vec4(1, 0, 0, 0),
		vec4(0, 1, 0, 0),
		vec4(0, 0, 1, 0),
		vec4(tx, ty, tz, 1)
	);

	return mt * ms;
}

mat4 create_camera_matrix(const vec3& u, const vec3& v, const vec3& w, const vec3& e) {
	mat4 mc = mat4(
		vec4(u.x, u.y, u.z, 0),
		vec4(v.x, v.y, v.z, 0),
		vec4(w.x, w.y, w.z, 0),
		vec4(e.x, e.y, e.z, 1)
	);

	return mc;
}

mat4 create_projection_matrix(const float& l, const float& r, const float& t, const float& b, const float& n, const float& f)
{
	mat4 mp = mat4(
		vec4(2 * n / (r - l), 0, 0, 0),
		vec4(0, 2 * n / (t - b), 0, 0),
		vec4((l + r) / (l - r), (b + t) / (b - t), (f + n) / (n - f), -1),
		vec4(0, 0, (-2 * f * n) / (f - n), 0)
	);

	return mp;
}

int gNumVertices;
int gNumTriangles;
std::vector<vec3> gVertices;
std::vector<vec3> gNormals;
std::vector<unsigned int> gIndexBuffer;

void create_scene()
{
	int width = 32;
	int height = 16;

	float theta, phi;
	int t;

	gNumVertices = (height - 2) * width + 2;
	gNumTriangles = (height - 2) * (width - 1) * 2;

	gVertices.resize(gNumVertices);
	gNormals.resize(gNumVertices, vec3(0.0f, 0.0f, 0.0f)); // √ ±‚»≠

	gIndexBuffer.resize(3 * gNumTriangles);

	t = 0;
	for (int j = 1; j < height - 1; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			theta = (float)j / (height - 1) * M_PI;
			phi = (float)i / (width - 1) * M_PI * 2;

			float x = sinf(theta) * cosf(phi);
			float y = cosf(theta);
			float z = -sinf(theta) * sinf(phi);

			gVertices[t] = vec3(x, y, z);
			t++;
		}
	}

	// Top pole
	gVertices[t++] = vec3(0.0f, 1.0f, 0.0f);
	// Bottom pole
	gVertices[t++] = vec3(0.0f, -1.0f, 0.0f);

	// Generate index buffer
	t = 0;
	for (int j = 0; j < height - 3; ++j)
	{
		for (int i = 0; i < width - 1; ++i)
		{
			int k0 = j * width + i;
			int k1 = (j + 1) * width + (i + 1);
			int k2 = j * width + (i + 1);

			gIndexBuffer[t++] = k0;
			gIndexBuffer[t++] = k1;
			gIndexBuffer[t++] = k2;

			k0 = j * width + i;
			k1 = (j + 1) * width + i;
			k2 = (j + 1) * width + (i + 1);

			gIndexBuffer[t++] = k0;
			gIndexBuffer[t++] = k1;
			gIndexBuffer[t++] = k2;
		}
	}

	int topIndex = (height - 2) * width;
	int bottomIndex = topIndex + 1;

	for (int i = 0; i < width - 1; ++i)
	{
		gIndexBuffer[t++] = topIndex;
		gIndexBuffer[t++] = i;
		gIndexBuffer[t++] = i + 1;

		gIndexBuffer[t++] = bottomIndex;
		gIndexBuffer[t++] = (height - 3) * width + (i + 1);
		gIndexBuffer[t++] = (height - 3) * width + i;
	}

	// Compute normals
	for (int i = 0; i < gNumTriangles; ++i)
	{
		int k0 = gIndexBuffer[3 * i + 0];
		int k1 = gIndexBuffer[3 * i + 1];
		int k2 = gIndexBuffer[3 * i + 2];

		vec3 v0 = gVertices[k0];
		vec3 v1 = gVertices[k1];
		vec3 v2 = gVertices[k2];

		// Face normal
		vec3 edge1 = v1 - v0;
		vec3 edge2 = v2 - v0;
		vec3 faceNormal = normalize(cross(edge1, edge2));

		// Accumulate to each vertex normal
		gNormals[k0] += faceNormal;
		gNormals[k1] += faceNormal;
		gNormals[k2] += faceNormal;
	}

	// Normalize all vertex normals
	for (int i = 0; i < gNumVertices; ++i)
	{
		gNormals[i] = normalize(gNormals[i]);
	}
}

int WIDTH = 512;
int HEIGHT = 512;

void resize_callback(GLFWwindow*, int nw, int nh)
{
	WIDTH = nw;
	HEIGHT = nh;
	glViewport(0, 0, nw, nh);
}

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

int main(int argc, char* argv[])
{
	GLFWwindow* window;

	if (!glfwInit())
	{
		printf("Failed to init GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);	// 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// use OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// not use old OpenGL

	window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Viewer", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		printf("Failed to create window\n");
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, resize_callback);

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		printf("Failed to init GLEW\n");
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GLuint shader_program = load_shaders("Phong.vert", "Phong.frag");

	create_scene();

	vec3 u = vec3(1, 0, 0);
	vec3 v = vec3(0, 1, 0);
	vec3 w = vec3(0, 0, 1);
	vec3 e = vec3(0, 0, 0);
	vec3 light = vec3(-4, 4, -3);

	mat4 modeling_matrix = create_modeling_matrix(2, 2, 2, 0, 0, -7);
	mat4 camera_matrix = create_camera_matrix(u, v, w, e);
	mat4 projection_matrix = create_projection_matrix(-0.1, 0.1, 0.1, -0.1, 0.1, 1000);

	vec3 ka = vec3(0.0, 1.0, 0.0);
	vec3 kd = vec3(0.0, 0.5, 0.0);
	vec3 ks = vec3(0.5, 0.5, 0.5);
	float p = 32.0;
	float gamma = 2.2;

	float Ia = 0.2;
	float Id = 1.0;

	GLuint vao;
	glGenVertexArrays(1, &vao);

	const int num_buffers = 3;
	std::vector<GLuint> gl_buffers;
	gl_buffers.resize(num_buffers, 0);
	glGenBuffers(num_buffers, &gl_buffers[0]);
	update_mesh(vao, gl_buffers, gVertices, gNormals, gIndexBuffer);

	while (!glfwWindowShouldClose(window))
	{
		process_input(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader_program);

		glUniformMatrix4fv(glGetUniformLocation(shader_program, "modeling"), 1, GL_FALSE, &modeling_matrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "camera"), 1, GL_FALSE, &camera_matrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, &projection_matrix[0][0]);
		glUniform3fv(glGetUniformLocation(shader_program, "lightPos"), 1, &light[0]);
		glUniform3fv(glGetUniformLocation(shader_program, "viewPos"), 1, &e[0]);
		glUniform3fv(glGetUniformLocation(shader_program, "ka"), 1, &ka[0]);
		glUniform3fv(glGetUniformLocation(shader_program, "kd"), 1, &kd[0]);
		glUniform3fv(glGetUniformLocation(shader_program, "ks"), 1, &ks[0]);
		glUniform1f(glGetUniformLocation(shader_program, "p"), p);
		glUniform1f(glGetUniformLocation(shader_program, "gamma"), gamma);
		glUniform1f(glGetUniformLocation(shader_program, "Ia"), Ia);
		glUniform1f(glGetUniformLocation(shader_program, "Id"), Id);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, gIndexBuffer.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &vao);
	if (gl_buffers[0] != 0)
	{
		glDeleteBuffers(num_buffers, &gl_buffers[0]);
	}
	glDeleteProgram(shader_program);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}