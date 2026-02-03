#define _USE_MATH_DEFINES

//Подключение библиотек
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <random>
#include <chrono>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

#include "model.h"
#include "camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#pragma comment(lib, "opengl32.lib")

#define MAX_NUM_LIGHTS 10

GLfloat* calculateNPolygon(unsigned int);
GLuint genVbo(GLfloat*, unsigned int number);
GLuint genVbo(std::vector<float>*);
GLuint genVao(GLuint*, GLuint*);
GLuint genEb(GLuint*, GLsizeiptr);
GLuint genEb(std::vector<unsigned int>*);
GLuint genShaderProgram(GLuint*, GLuint*);

bool loadModel(const std::string&, std::vector<float>&, std::vector<float>&, std::vector<unsigned int>&);
void loadObj(const std::string&, GLuint* vao, GLuint* ebo, GLuint* indices_size);
void processNode(aiNode*, const aiScene*, std::vector<float>&, std::vector<float>&, std::vector<unsigned int>&);
void processMesh(aiMesh*, std::vector<float>&, std::vector<float>&, std::vector<unsigned int>&);

std::string LoadShader(const char*);
GLfloat* genArrRNG(int);
GLfloat* genFlatColorArray(int, glm::vec3);

struct texMaterial
{
	GLuint texture;
	glm::vec3 specular;
	float shininess;
};

struct Material
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

struct DirLight
{
	glm::vec3 direction;
	// Параметры компонент света
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct PointLight
{
	glm::vec3 position;
	// Параметры затухания
	float constant;
	float linear;
	float quadratic;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct SpotLight
{
	glm::vec3 position;
	glm::vec3 direction;
	// Параметры угла
	float cutOff;
	float outerCutOff;

	// Параметры затухания
	float constant;
	float linear;
	float quadratic;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct LightsInfo
{
	DirLight dirLight;
	PointLight pointLights[MAX_NUM_LIGHTS];
	SpotLight spotLights[MAX_NUM_LIGHTS];
	int numPLights;
	int numSLights;
};

Material emerald
{
	glm::vec3 (0.0215, 0.1745, 0.0215),
	glm::vec3(0.07568, 0.61424, 0.07568),
	glm::vec3(0.633, 0.727811, 0.633),
	0.6
};


void paint(GLuint*, GLuint*, Material*, texMaterial*, GLsizei, GLuint*, glm::vec3, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void paintModel(Model*, GLuint*, glm::vec3, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void setMaterial(texMaterial*, GLuint*);
void setMaterial(Material*, GLuint*);

int WinWidth = 1080;
int WinHeight = 1080;

GLFWwindow* window;
Camera* camera;

void glfw_window_size_callback(GLFWwindow* window, int width, int height)
{
	WinWidth = width;
	WinHeight = height;
}
using namespace std;
int main()
{
	//Ининциализация и создание окна
	if (!glfwInit())
	{
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return -1;
	}

	window = glfwCreateWindow(WinWidth, WinHeight, "Triangle", NULL, NULL);
	glfwMakeContextCurrent(window);

	glewInit();

	Model fence("C:/Users/svekolny/source/repos/CGG_1/Kursach/fence.obj");
	Model pumpkin("C:/Users/svekolny/source/repos/CGG_1/Kursach/pumpkin.obj");
	Model tomato("C:/Users/svekolny/source/repos/CGG_1/Kursach/tomato_bush.obj");
	Model carrot("C:/Users/svekolny/source/repos/CGG_1/Kursach/carrot.obj");
	Model streetlight("C:/Users/svekolny/source/repos/CGG_1/Kursach/streetlight.obj");


	//сферо
	GLuint sphere_ebo;
	GLuint sphere_vao;
	GLuint sphere_indices_size;
	loadObj("C:\\Users\\svekolny\\OneDrive\\Документы\\sphere\\sphere.obj", &sphere_vao, &sphere_ebo, &sphere_indices_size);


	vector<float>* quad_out_vertices = new vector <float>
	{
		-1.0f, -0.5f, 0.0f,
		-0.8f, 0.5f, 0.0f,
		1.0f, 0.5f, 0.0f,
		0.8f, -0.5f, 0.0f
	};
	vector<unsigned int>* quad_out_indices = new vector<unsigned int>
	{
		0, 1, 2,
		0, 2, 3
	};
	vector<float>* quad_out_normals = new vector<float>
	{
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f
	};
	//Обычные текстурные координаты
	vector<glm::vec2> texCoords;
	texCoords.push_back(glm::vec2(0.0f, 0.0f));
	texCoords.push_back(glm::vec2(0.0f, 1.0f));
	texCoords.push_back(glm::vec2(1.0f, 1.0f));
	texCoords.push_back(glm::vec2(1.0f, 0.0f));

	//Текстурные координаты для последнего задания
	vector<glm::vec2> texCoordsSmol;
	texCoordsSmol.push_back(glm::vec2(0.0f, 0.0f));
	texCoordsSmol.push_back(glm::vec2(0.0f, 5.0f));
	texCoordsSmol.push_back(glm::vec2(5.0f, 5.0f));
	texCoordsSmol.push_back(glm::vec2(5.0f, 0.0f));

	//Генерация вершинных, цветовых и элементных буферов

	GLuint quad_vbov = genVbo(quad_out_vertices);
	GLuint quad_vbon = genVbo(quad_out_normals);

	GLuint quad_ebo = genEb(quad_out_indices);

	//Создание массивов вершин (цвет + координата)

	GLuint quad_vao = genVao(&quad_vbov, &quad_vbon);

	//Настройка текстур
	int width, height, nrChannels;
	unsigned char* data = stbi_load("MetalCorrodedHeavy001_COL_2K_METALNESS.jpg", &width, &height, &nrChannels, STBI_rgb);

	cout << width << " " << height << " " << nrChannels << endl;
	if (data == 0) cout << "Error on texture loading";


	//Текстура обычная
	GLuint texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint tex_vbo = 0;
	glGenBuffers(1, &tex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, tex_vbo);

	glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);
	glBindVertexArray(quad_vao);
	glBindBuffer(GL_ARRAY_BUFFER, tex_vbo);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);

	texMaterial texture
	{
		texID,
		glm::vec3(0.5f),
		0.5f
	};

	//Создаём вершинные шейдеры
	std::string tex_vertex_shader_path = LoadShader("C:\\Users\\svekolny\\source\\repos\\CGG_1\\Kursach\\tex_vertex_shader.vert");
	const char* tex_vertex_shader = tex_vertex_shader_path.c_str();

	GLuint texvs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(texvs, 1, &tex_vertex_shader, NULL);
	glCompileShader(texvs);

	std::string vertex_shader_path = LoadShader("C:\\Users\\svekolny\\source\\repos\\CGG_1\\Kursach\\vertex_shader.vert");
	const char* vertex_shader = vertex_shader_path.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);

	//Создаём фрагментный шейдер
	std::string tex_fragment_shader_path = LoadShader("C:\\Users\\svekolny\\source\\repos\\CGG_1\\Kursach\\tex_fragment_shader.frag");
	const char* tex_fragment_shader = tex_fragment_shader_path.c_str();

	GLuint texfs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(texfs, 1, &tex_fragment_shader, NULL);
	glCompileShader(texfs);

	std::string fragment_shader_path = LoadShader("C:\\Users\\svekolny\\source\\repos\\CGG_1\\Kursach\\fragment_shader.frag");
	const char* fragment_shader = fragment_shader_path.c_str();

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);

	//Смотрим, всё ли скомпилировалось
	GLint compilationLog;

	GLint success;

	glGetShaderiv(texvs, GL_COMPILE_STATUS, &success);
	if (!success) cout << "texvs compilation error";

	glGetShaderiv(texfs, GL_COMPILE_STATUS, &success);
	if (!success) cout << "texfs compilation error";

	glGetShaderiv(texvs, GL_INFO_LOG_LENGTH, &compilationLog);
	if (compilationLog > 0)
	{
		char* errorMessage = new char[compilationLog + 1];
		glGetShaderInfoLog(texvs, compilationLog, NULL, errorMessage);
		std::cout << errorMessage;
		delete errorMessage;
	}

	glGetShaderiv(texfs, GL_INFO_LOG_LENGTH, &compilationLog);
	if (compilationLog > 0)
	{
		char* errorMessage = new char[compilationLog + 1];
		glGetShaderInfoLog(texfs, compilationLog, NULL, errorMessage);
		std::cout << errorMessage;
		delete errorMessage;
	}

	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success) cout << "vs compilation error";

	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success) cout << "fs compilation error";

	glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &compilationLog);
	if (compilationLog > 0)
	{
		char* errorMessage = new char[compilationLog + 1];
		glGetShaderInfoLog(vs, compilationLog, NULL, errorMessage);
		std::cout << errorMessage;
		delete errorMessage;
	}

	glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &compilationLog);
	if (compilationLog > 0)
	{
		char* errorMessage = new char[compilationLog + 1];
		glGetShaderInfoLog(fs, compilationLog, NULL, errorMessage);
		std::cout << errorMessage;
		delete errorMessage;
	}

	//Собираем шейдеры в шейдерную программу
	GLuint tex_shader_program = genShaderProgram(&texvs, &texfs);
	GLuint shader_program = genShaderProgram(&vs, &fs);

	//Политика изменения размеров окна
	glfwSetWindowSizeCallback(window, glfw_window_size_callback);

	//Непосредственно отрисовка программы

	GLfloat lightIntensity = 1.0f;
	glm::vec4 dayColor (0.51f, 0.784f, 0.898f, 1.0f);
	glm::vec4 nightColor(0.019f, 0.055f, 0.22f, 1.0f);
	glm::vec4 clearColor( 0.51f, 0.784f, 0.898f, 1.0f );
	glm::vec4 nightDayDiff = dayColor - nightColor;
	glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
	

	GLfloat lastFrameTime = glfwGetTime();

	int currentKey = 0;

	GLfloat axisChange = 2.0f;
	GLfloat RotationChange = 180.0f;

	GLfloat sceneAxisX = 0.0f;
	GLfloat sceneAxisZ = 0.0f;

	GLfloat sceneRotation = 0.0f;

	GLfloat cameraApproach = 50.0f;

	GLfloat cameraChange = 50.0f;
	GLfloat cameraAxisX = 0.00f;
	GLfloat cameraAxisZ = 0.00f;

	GLfloat fenceHeight = 19.75f;
	GLfloat fenceLength = -1.0f;
	GLfloat fenceWidth = 1.0f;


	// Пример точки на сфере из obj файла:	v - 0.134320 10.000000 - 9.999100
	//Это значит, что радиус сферы равен примерно корню из 200 (по теореме Пифагора), что в свою очередь составляет примерно 14,14
	//Значит, требуемое изменение размера сферы для получения сферы с радиусом 5 равно отношению 5 к имеющемуся радиусу: 5 / 14,14 ~= 0,354
	//Получившееся значение и будем использовать в качестве требуемого изменения размера для соответствия сферы заданию

	GLfloat sphereSize = 1.0f;

	camera = new Camera();

	glfwSetWindowUserPointer(window, camera);

	glfwSetCursorPosCallback(window, camera->MouseCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	int mode = 0;
	bool isNight = false;

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //ВСЕГДА в самом начале

		//Операции для вычисления времени между кадрами
		GLfloat currentFrameTime = glfwGetTime();

		GLfloat delta = currentFrameTime - lastFrameTime;

		lastFrameTime = currentFrameTime;

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		switch (mode)
		{
		case 0: //Обычный вариант
		{
			//paint(&quad_vao, &quad_ebo, nullptr, &texture, quad_out_indices->size(), &tex_shader_program, glm::vec3(0.0f, 0.0f, 0.0f), 0, sceneAxisX, sceneAxisZ, sceneRotation, cameraApproach, cameraAxisX, cameraAxisZ);
			paint(&sphere_vao, &sphere_ebo, &emerald, nullptr, sphere_indices_size, &shader_program, glm::vec3(0.0f, 0.0f, 0.0f), sphereSize, sceneAxisX, sceneAxisZ, sceneRotation, lightIntensity);
			paintModel(&fence, &tex_shader_program, glm::vec3(fenceLength, fenceHeight, fenceWidth + 1.0f), sphereSize * 0.5, sceneAxisX, sceneAxisZ, sceneRotation, lightIntensity);
			paintModel(&fence, &tex_shader_program, glm::vec3(fenceLength + 1.0f, fenceHeight, fenceWidth +1.0f), sphereSize * 0.5, sceneAxisX, sceneAxisZ, sceneRotation, lightIntensity);
			paintModel(&fence, &tex_shader_program, glm::vec3(fenceLength + 2.0f, fenceHeight, fenceWidth + 1.0f), sphereSize * 0.5, sceneAxisX, sceneAxisZ, sceneRotation, lightIntensity);
			paintModel(&fence, &tex_shader_program, glm::vec3(fenceLength - 0.5f, fenceHeight, fenceWidth + 0.5f), sphereSize * 0.5, sceneAxisX, sceneAxisZ, sceneRotation + 90, lightIntensity);
			paintModel(&fence, &tex_shader_program, glm::vec3(fenceLength + 2.5f, fenceHeight, fenceWidth + 0.5f),  sphereSize * 0.5, sceneAxisX, sceneAxisZ, sceneRotation + 90, lightIntensity);
			paintModel(&fence, &tex_shader_program, glm::vec3(fenceLength + 2.5f, fenceHeight, fenceWidth + -0.5f), sphereSize * 0.5, sceneAxisX, sceneAxisZ, sceneRotation + 90, lightIntensity);
			paintModel(&fence, &tex_shader_program, glm::vec3(fenceLength + 2.5f, fenceHeight, fenceWidth + -1.5f), sphereSize * 0.5, sceneAxisX, sceneAxisZ, sceneRotation + 90, lightIntensity);
			paintModel(&fence, &tex_shader_program, glm::vec3(fenceLength + 2.5f, fenceHeight, fenceWidth + -2.5f), sphereSize * 0.5, sceneAxisX, sceneAxisZ, sceneRotation + 90, lightIntensity);
			paintModel(&fence, &tex_shader_program, glm::vec3(fenceLength + 2.0f, fenceHeight, fenceWidth - 3.0f), sphereSize * 0.5, sceneAxisX, sceneAxisZ, sceneRotation, lightIntensity);
			paintModel(&fence, &tex_shader_program, glm::vec3(fenceLength + 1.0f, fenceHeight, fenceWidth - 3.0f), sphereSize * 0.5, sceneAxisX, sceneAxisZ, sceneRotation, lightIntensity);
			paintModel(&fence, &tex_shader_program, glm::vec3(fenceLength, fenceHeight, fenceWidth - 3.0f), sphereSize * 0.5, sceneAxisX, sceneAxisZ, sceneRotation, lightIntensity);
			paintModel(&fence, &tex_shader_program, glm::vec3(fenceLength - 0.5f, fenceHeight, fenceWidth - 2.5f), sphereSize * 0.5, sceneAxisX, sceneAxisZ, sceneRotation + 90, lightIntensity);
			paintModel(&pumpkin, &tex_shader_program, glm::vec3(fenceLength + 1.5f, fenceHeight + 0.3f, fenceWidth - 2.0f), sphereSize * 0.5, sceneAxisX, sceneAxisZ, sceneRotation + 90, lightIntensity);
			paintModel(&tomato, &tex_shader_program, glm::vec3(fenceLength + 1.8f, fenceHeight + 0.55f, fenceWidth + 0.5f), sphereSize * 1, sceneAxisX, sceneAxisZ, sceneRotation + 90, lightIntensity);
			paintModel(&tomato, &tex_shader_program, glm::vec3(fenceLength + 1.2f, fenceHeight + 0.65f, fenceWidth - 0.1f), sphereSize * 1, sceneAxisX, sceneAxisZ, sceneRotation + 150, lightIntensity);
			paintModel(&tomato, &tex_shader_program, glm::vec3(fenceLength + 0.6f, fenceHeight + 0.6f, fenceWidth + 0.5f), sphereSize * 1, sceneAxisX, sceneAxisZ, sceneRotation - 30, lightIntensity);
			paintModel(&carrot, &tex_shader_program, glm::vec3(fenceLength + 0.3f, fenceHeight + 0.19f, fenceWidth - 2.0f), sphereSize * 0.001, sceneAxisX, sceneAxisZ, sceneRotation - 30, lightIntensity);
			paintModel(&carrot, &tex_shader_program, glm::vec3(fenceLength + 0.5f, fenceHeight + 0.15f, fenceWidth - 2.2f), sphereSize * 0.001, sceneAxisX, sceneAxisZ, sceneRotation, lightIntensity);
			paintModel(&carrot, &tex_shader_program, glm::vec3(fenceLength + 0.0f, fenceHeight + 0.13f, fenceWidth - 1.9f), sphereSize * 0.001, sceneAxisX, sceneAxisZ, sceneRotation + 60, lightIntensity);
			paintModel(&streetlight, &tex_shader_program, glm::vec3(fenceLength - 1.5f, fenceHeight - 0.25f, fenceWidth + 0.5f), sphereSize * 1.3, sceneAxisX, sceneAxisZ, sceneRotation + 150, lightIntensity);
			paintModel(&pumpkin, &tex_shader_program, glm::vec3(-3.2f, 21.75, 1.1f), sphereSize * 0.1, sceneAxisX, sceneAxisZ, sceneRotation + 150, lightIntensity);
			break;
		}
		}

		camera->ProcessKeyboard(window, delta);

		//Передвижение сцены в плоскости XZ
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_UP))
		{
			sceneAxisX -= axisChange * delta;
		}
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_DOWN))
		{
			sceneAxisX += axisChange * delta;
		}

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_RIGHT))
		{
			sceneAxisZ += axisChange * delta;
		}
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT))
		{
			sceneAxisZ -= axisChange * delta;
		}

		//Вращение сцены вокруг своего центра
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_T))
		{
			sceneRotation -= RotationChange * delta;
		}
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_G))
		{
			sceneRotation += RotationChange * delta;
		}

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_N))
		{
			if (!isNight)
			{
				isNight = true;
			}
			else
			{
				isNight = false;
			}
		}

		if (isNight)
		{
			if (clearColor[0] >= nightColor[0])
			{
				clearColor -= (nightDayDiff / 3.0f) * delta;
				glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
			}
			if (lightIntensity >= 0.01)
			{
				lightIntensity -= 0.33f * delta;
			}
		}
		else
		{
			if (clearColor[0] <= dayColor[0])
			{
				clearColor += (nightDayDiff / 3.0f) * delta;
				glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
			}
			if (lightIntensity <= 0.99)
			{
				lightIntensity += 0.33f * delta;
			}
		}

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(window, 1);
		}

		glfwPollEvents();
		glfwSwapBuffers(window);
		glViewport(0, 0, WinWidth, WinHeight);
	}
	glfwTerminate();
}



string LoadShader(const char* filename)
{
	string res;
	ifstream file(filename, ios::in);

	if (file.is_open())
	{
		stringstream sstr;
		sstr << file.rdbuf();
		res = sstr.str();
		file.close();
	}
	else cout << "error on opening path " << filename;

	return res;
}

GLfloat* calculateNPolygon(unsigned int vertexes)
{
	unsigned int dimensions = 3;

	GLfloat* VertexArray = new GLfloat[vertexes * dimensions];

	float Radius = 0.5f;

	for (int i = 0; i < vertexes; ++i)
	{
		float angle = 2.0f * M_PI * i / vertexes;
		GLfloat x = Radius * std::cos(angle);
		GLfloat y = Radius * std::sin(angle);
		VertexArray[i * dimensions] = x;
		VertexArray[i * dimensions + 1] = y;
		VertexArray[i * dimensions + 2] = 0.0f;
	}
	return VertexArray;
}

GLfloat* genArrRNG(int number)
{
	GLfloat* array = new GLfloat[number];

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 generator(seed);

	std::uniform_real_distribution<double> real_dist(0.0, 1.0);

	for (int i = 0; i < number; ++i)
	{
		array[i] = real_dist(generator);
	}
	return array;
}

GLuint genVbo(GLfloat* array, unsigned int number)
{
	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, number * sizeof(GLfloat), array, GL_STATIC_DRAW);
	return vbo;
}

GLuint genVbo(vector<float>* array)
{
	if (array == nullptr || array->empty()) {
		std::cerr << "Ошибка: Указатель на вектор пуст или NULL." << std::endl;
		return 0; // Возвращаем 0 в случае ошибки
	}

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, array->size() * sizeof(float), array->data(), GL_STATIC_DRAW);
	return vbo;
}

GLuint genVao(GLuint* vbov, GLuint* vbon)
{
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, *vbov);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, *vbon);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	return vao;
}

GLuint genEb(GLuint* indexes, GLsizeiptr size)
{
	GLuint eb = 0;
	glGenBuffers(1, &eb);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eb);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(GLuint), indexes, GL_STATIC_DRAW);
	return eb;
}

GLuint genEb(vector<unsigned int>* array)
{
	GLuint eb = 0;
	glGenBuffers(1, &eb);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eb);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, array->size() * sizeof(unsigned int), array->data(), GL_STATIC_DRAW);
	return eb;
}

GLuint genShaderProgram(GLuint* vs, GLuint* fs)
{
	GLuint shader_program = glCreateProgram();

	glAttachShader(shader_program, *vs);
	glAttachShader(shader_program, *fs);

	glBindAttribLocation(shader_program, 0, "vertex_position");
	glBindAttribLocation(shader_program, 1, "vertex_color");

	glLinkProgram(shader_program);

	GLint linkStatus;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &linkStatus);
	if (!linkStatus)
	{
		GLint logLength = 0;
		glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &logLength);

		if (logLength > 0)
		{
			// Выделить память для лога
			char* linkLog = new char[logLength + 1];
			glGetProgramInfoLog(shader_program, logLength, NULL, linkLog);

			// Вывести лог
			std::cout << "Ошибка линковки шейдерной программы:\n" << linkLog << endl;

			// Освободить память
			delete[] linkLog;
		}

		else
		{
			std::cout << "Ошибка линковки шейдерной программы, лог пуст" << endl;
		}
	}

	return shader_program;
}

//Отрисовка фигур по заданиям по вариантам
void paint(GLuint* vertex_array, GLuint* element_buffer, Material* material, texMaterial* texture, GLsizei size_of, GLuint* shader_program, glm::vec3 positionChange, GLfloat scale, GLfloat sceneAxisX, GLfloat sceneAxisZ, GLfloat sceneRotationAngle, GLfloat lightIntensity)
{
	glProvokingVertex(GL_LAST_VERTEX_CONVENTION);
	glm::mat4 transformMatrix = glm::mat4(1.0f);

	transformMatrix = glm::translate(transformMatrix, glm::vec3(sceneAxisX, 0.0f, sceneAxisZ));
	transformMatrix = glm::translate(transformMatrix, glm::vec3(positionChange[0], positionChange[1], positionChange[2]));

	transformMatrix = glm::rotate(transformMatrix, glm::radians(sceneRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));

	transformMatrix = glm::scale(transformMatrix, glm::vec3(scale, scale, scale));

	glUseProgram(*shader_program);

	camera->passCameraValue((GLfloat)WinWidth / (GLfloat)WinHeight, shader_program);

	//Передача матриц преобразований
	GLuint stm = glGetUniformLocation(*shader_program, "transform");
	glUniformMatrix4fv(stm, 1, GL_FALSE, &transformMatrix[0][0]);

	GLuint scp = glGetUniformLocation(*shader_program, "campos");
	glUniform3fv(scp, 1, glm::value_ptr(camera->Position));

	if (texture) setMaterial(texture, shader_program);
	if (material) setMaterial(material, shader_program);

	//Работа со светом
	LightsInfo lights;

	if (lightIntensity < 0.1) lightIntensity = 0.1;

	//Передача фонового освещения
	DirLight background
	{
		glm::vec3(1.0f, -1.0f, 0.0f), //dir

		glm::vec3(0.8f) * lightIntensity, //amb
		glm::vec3(0.8f) * lightIntensity, //dif
		glm::vec3(0.8f) * lightIntensity //shi
	};

	PointLight streetlight
	{
		glm::vec3 {-3.2f, 21.75, 1.1f},
		// Параметры затухания
		0.5f, //constant
		0.14f, //linear
		0.07f, //quadratic

		glm::vec3(1.0f) * (1 - lightIntensity), //ambient
		glm::vec3(1.0f) * (1 - lightIntensity), //diffuse
		glm::vec3(1.0f) * (1 - lightIntensity) //specular
	};

	lights.dirLight = background;

	GLuint bgdir = glGetUniformLocation(*shader_program, "lights.dirLight.direction");
	glUniform3fv(bgdir, 1, &lights.dirLight.direction[0]);

	GLuint bgamb = glGetUniformLocation(*shader_program, "lights.dirLight.ambient");
	glUniform3fv(bgamb, 1, &lights.dirLight.ambient[0]);

	GLuint bgdif = glGetUniformLocation(*shader_program, "lights.dirLight.diffuse");
	glUniform3fv(bgdif, 1, &lights.dirLight.diffuse[0]);

	GLuint bgspc = glGetUniformLocation(*shader_program, "lights.dirLight.specular");
	glUniform3fv(bgspc, 1, &lights.dirLight.specular[0]);

	//Число источников света
	GLuint lnp = glGetUniformLocation(*shader_program, "lights.numPLights");
	glUniform1i(lnp, 1);

	GLuint lns = glGetUniformLocation(*shader_program, "lights.numSLights");
	glUniform1i(lns, 0);

	GLuint ls2_pos = glGetUniformLocation(*shader_program, "lights.pointLights[0].position");
	glUniform3fv(ls2_pos, 1, &streetlight.position[0]);
	GLuint ls2_con = glGetUniformLocation(*shader_program, "lights.pointLights[0].constant");
	glUniform1f(ls2_con, streetlight.constant);
	GLuint ls2_ln = glGetUniformLocation(*shader_program, "lights.pointLights[0].linear");
	glUniform1f(ls2_ln, streetlight.linear);
	GLuint ls2_qd = glGetUniformLocation(*shader_program, "lights.pointLights[0].quadratic");
	glUniform1f(ls2_qd, streetlight.quadratic);
	GLuint ls2_amb = glGetUniformLocation(*shader_program, "lights.pointLights[0].ambient");
	glUniform3fv(ls2_amb, 1, &streetlight.ambient[0]);
	GLuint ls2_dif = glGetUniformLocation(*shader_program, "lights.pointLights[0].diffuse");
	glUniform3fv(ls2_dif, 1, &streetlight.diffuse[0]);
	GLuint ls2_spc = glGetUniformLocation(*shader_program, "lights.pointLights[0].specular");

	//if (stm == -1 || spm == -1 || svm == -1 || scp == -1 || bgdir == -1 || bgamb == -1 || bgdif == -1 || bgspc == -1) cout << "bruh";

	glBindVertexArray(*vertex_array);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *element_buffer); //Именно в таком порядке!! Иначе бан

	glLineWidth(1);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, size_of, GL_UNSIGNED_INT, 0);
}

void paintModel(Model* model, GLuint* shader_program, glm::vec3 positionChange, GLfloat scale, GLfloat sceneAxisX, GLfloat sceneAxisZ, GLfloat sceneRotationAngle, GLfloat lightIntensity)
{
	glProvokingVertex(GL_LAST_VERTEX_CONVENTION);
	glm::mat4 transformMatrix = glm::mat4(1.0f);

	transformMatrix = glm::translate(transformMatrix, glm::vec3(sceneAxisX, 0.0f, sceneAxisZ));
	transformMatrix = glm::translate(transformMatrix, glm::vec3(positionChange[0], positionChange[1], positionChange[2]));

	transformMatrix = glm::rotate(transformMatrix, glm::radians(sceneRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));

	transformMatrix = glm::scale(transformMatrix, glm::vec3(scale, scale, scale));

	glUseProgram(*shader_program);

	camera->passCameraValue((GLfloat)WinWidth / (GLfloat)WinHeight, shader_program);

	//Передача матриц преобразований
	GLuint stm = glGetUniformLocation(*shader_program, "transform");
	glUniformMatrix4fv(stm, 1, GL_FALSE, &transformMatrix[0][0]);

	GLuint scp = glGetUniformLocation(*shader_program, "campos");
	glUniform3fv(scp, 1, glm::value_ptr(camera->Position));

	//Работа со светом
	LightsInfo lights;

	//Передача фонового освещения
	DirLight background
	{
		glm::vec3(1.0f, -1.0f, 0.0f), //dir

		glm::vec3(0.8f) * lightIntensity, //amb
		glm::vec3(0.8f) * lightIntensity, //dif
		glm::vec3(0.8f) * lightIntensity //shi
	};

	PointLight streetlight
	{
		glm::vec3 {-3.2f, 21.75, 1.1f},
		0.5f, //constant
		0.14f, //linear
		0.07f, //quadratic

		glm::vec3(0.3f) * (1 - lightIntensity), //ambient
		glm::vec3(0.3f) * (1 - lightIntensity), //diffuse
		glm::vec3(0.3f) * (1 - lightIntensity) //specular
	};


	lights.dirLight = background;

	GLuint bgdir = glGetUniformLocation(*shader_program, "lights.dirLight.direction");
	glUniform3fv(bgdir, 1, &lights.dirLight.direction[0]);

	GLuint bgamb = glGetUniformLocation(*shader_program, "lights.dirLight.ambient");
	glUniform3fv(bgamb, 1, &lights.dirLight.ambient[0]);

	GLuint bgdif = glGetUniformLocation(*shader_program, "lights.dirLight.diffuse");
	glUniform3fv(bgdif, 1, &lights.dirLight.diffuse[0]);

	GLuint bgspc = glGetUniformLocation(*shader_program, "lights.dirLight.specular");
	glUniform3fv(bgspc, 1, &lights.dirLight.specular[0]);


	//Число источников света
	GLuint lnp = glGetUniformLocation(*shader_program, "lights.numPLights");
	glUniform1i(lnp, 1);

	GLuint lns = glGetUniformLocation(*shader_program, "lights.numSLights");
	glUniform1i(lns, 0);

	GLuint ls2_pos = glGetUniformLocation(*shader_program, "lights.pointLights[0].position");
	glUniform3fv(ls2_pos, 1, &streetlight.position[0]);
	GLuint ls2_con = glGetUniformLocation(*shader_program, "lights.pointLights[0].constant");
	glUniform1f(ls2_con, streetlight.constant);
	GLuint ls2_ln = glGetUniformLocation(*shader_program, "lights.pointLights[0].linear");
	glUniform1f(ls2_ln, streetlight.linear);
	GLuint ls2_qd = glGetUniformLocation(*shader_program, "lights.pointLights[0].quadratic");
	glUniform1f(ls2_qd, streetlight.quadratic);
	GLuint ls2_amb = glGetUniformLocation(*shader_program, "lights.pointLights[0].ambient");
	glUniform3fv(ls2_amb, 1, &streetlight.ambient[0]);
	GLuint ls2_dif = glGetUniformLocation(*shader_program, "lights.pointLights[0].diffuse");
	glUniform3fv(ls2_dif, 1, &streetlight.diffuse[0]);
	GLuint ls2_spc = glGetUniformLocation(*shader_program, "lights.pointLights[0].specular");

	//if (stm == -1 || spm == -1 || svm == -1 || scp == -1 || bgdir == -1 || bgamb == -1 || bgdif == -1 || bgspc == -1) cout << "bruh";
	glUniform1i(lns, 0);

	model->Draw(static_cast<unsigned int>(*shader_program));
}

void setMaterial(texMaterial* texture, GLuint* shader_program)
{
	//Передача материала
	GLuint mam = glGetUniformLocation(*shader_program, "material.texture");
	glUniform1f(mam, texture->texture);

	GLuint msp = glGetUniformLocation(*shader_program, "material.specular");
	glUniform3fv(msp, 1, &texture->specular[0]);

	GLuint msh = glGetUniformLocation(*shader_program, "material.shininess");
	glUniform1f(msh, texture->shininess);
}

void setMaterial(Material* material, GLuint* shader_program)
{
	GLuint mam = glGetUniformLocation(*shader_program, "material.ambient");
	glUniform3fv(mam, 1, &material->ambient[0]);

	GLuint mdi = glGetUniformLocation(*shader_program, "material.diffuse");
	glUniform3fv(mdi, 1, &material->diffuse[0]);

	GLuint msp = glGetUniformLocation(*shader_program, "material.specular");
	glUniform3fv(msp, 1, &material->specular[0]);

	GLuint msh = glGetUniformLocation(*shader_program, "material.shininess");
	glUniform1f(msh, material->shininess);
}

bool loadModel(const std::string& path, std::vector<float>& out_vertices, std::vector<float>& out_normals, std::vector<unsigned int>& out_indices)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	// Проверяем наличие ошибок при загрузке
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return false;
	}

	processNode(scene->mRootNode, scene, out_vertices, out_normals, out_indices);

	return true;
}

void loadObj(const std::string& path, GLuint* vao, GLuint* ebo, GLuint* indices_size)
{
	vector<float>* out_vertices = new vector<float>;
	vector<unsigned int>* out_indices = new vector<unsigned int>;
	vector<float>* out_normals = new vector<float>;
	loadModel(path, *out_vertices, *out_normals, *out_indices);

	GLuint vbov = genVbo(out_vertices);
	GLuint vbon = genVbo(out_normals);

	GLuint new_ebo = genEb(out_indices);
	GLuint new_vao = genVao(&vbov, &vbon);

	*vao = new_vao;
	*ebo = new_ebo;
	*indices_size = out_indices->size();
}

void processNode(aiNode* node, const aiScene* scene, std::vector<float>& out_vertices, std::vector<float>& out_normals, std::vector<unsigned int>& out_indices)
{
	// Обрабатываем все меши в текущем узле
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		// Узел содержит только индексы к реальным объектам мешей в сцене
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh, out_vertices, out_normals, out_indices);
	}

	// Рекурсивно обрабатываем дочерние узлы
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene, out_vertices, out_normals, out_indices);
	}
}

void processMesh(aiMesh* mesh, std::vector<float>& out_vertices, std::vector<float>& out_normals, std::vector<unsigned int>& out_indices)
{

	// Извлечение данных вершин (позиции и нормали)
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		// Позиции
		out_vertices.push_back(mesh->mVertices[i].x);
		out_vertices.push_back(mesh->mVertices[i].y);
		out_vertices.push_back(mesh->mVertices[i].z);

		//Нормали
		if (mesh->HasNormals()) {
			out_normals.push_back(mesh->mNormals[i].x);
			out_normals.push_back(mesh->mNormals[i].y);
			out_normals.push_back(mesh->mNormals[i].z);
		}
	}

	// Извлечение индексов (Assimp хранит их в "гранях" - aiFace)
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		// Поскольку мы использовали aiProcess_Triangulate, 
		// каждая грань будет состоять из 3 индексов
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			out_indices.push_back(face.mIndices[j]);
		}
	}
}

GLfloat* genFlatColorArray(int points, glm::vec3 color)
{
	GLfloat* array = new GLfloat[points];

	for (int i = 0; i < points; i += 3)
	{
		array[i] = color[0];
		array[i + 1] = color[1];
		array[i + 2] = color[2];
	}
	return array;
}

