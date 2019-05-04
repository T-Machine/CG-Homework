#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "tool.h"
#include "Rasterization.h"
#define MAX_POINT 42000
#define POINT_SIZE 7
#define WIN_W 105
#define WIN_H 105
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
const char *vertexShaderStr = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
	"}\0";
const char *fragmentShaderStr = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\n\0";

const char* glsl_version = "#version 330";
int show_what = 0;
bool show_Rasterization = false;
int radius = 0;

//顶点信息
float vertices[MAX_POINT];
//绘制点的数量
int counter = 0;
//三角形顶点坐标输入
int vertex1[2];
int vertex2[2];
int vertex3[2];

int main()
{
	for (int i = 0; i < MAX_POINT; i++) {
		vertices[i] = 0.0f;
	}

	//实例化GLFW窗口
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//创建窗口对象
	GLFWwindow* window = glfwCreateWindow(WIN_W, WIN_H, "HW2", NULL, NULL);
	if (window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	//将窗口上下文设置为当前线程主上下文
	glfwMakeContextCurrent(window);
	//注册回调函数
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//初始化GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	//imgui
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	//创建VAO
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//创建VBO存储顶点数据
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//将顶点数据复制到缓存
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//位置属性
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//颜色属性
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);

	int success;
	char info[512];

	//顶点着色器
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderStr, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, info);
		cout << "vertexShader compilation failed\n" << info << endl;
	}

	//片段着色器
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderStr, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, info);
		cout << "fragmentShader compilation failed\n" << info << endl;
	}

	//链接着色器程序
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, info);
		cout << "shaderProgram compilation failed\n" << info << endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	while (!glfwWindowShouldClose(window))
	{
		counter = 0;

		//处理输入
		processInput(window);

		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//ImGui::ShowDemoWindow(&show_demo_window);
		ImGui::Begin("Draw triangle or circle");
		
		ImGui::RadioButton("Triangle", &show_what, 0);
		ImGui::RadioButton("Circle", &show_what, 1);

		if (show_what == 0) {
			ImGui::SliderInt2("vertex1", vertex1, -WIN_H + 5, WIN_H - 5);
			ImGui::SliderInt2("vertex2", vertex2, -WIN_H + 5, WIN_H - 5);
			ImGui::SliderInt2("vertex3", vertex3, -WIN_H + 5, WIN_H - 5);

			Bresenham_line(vertex1[0], vertex1[1], vertex2[0], vertex2[1], vertices, counter, WIN_W);
			Bresenham_line(vertex1[0], vertex1[1], vertex3[0], vertex3[1], vertices, counter, WIN_W);
			Bresenham_line(vertex2[0], vertex2[1], vertex3[0], vertex3[1], vertices, counter, WIN_W);

			ImGui::Checkbox("Triangle Rasterization", &show_Rasterization);

			if (show_Rasterization) {
				Triangle_Rasterization(vertex1[0], vertex1[1], vertex2[0], vertex2[1], vertex3[0], vertex3[1], vertices, counter, WIN_W);
			}
		}
		else if (show_what == 1) {
			ImGui::SliderInt("Radius", &radius, 0, WIN_H - 5);
			Bresenham_circle(radius, vertices, counter, WIN_H);
		}

		ImGui::End();
		 
		//渲染背景颜色
		glClearColor(0.2f, 0.2f, 0.25f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glPointSize(POINT_SIZE);

		//激活程序
		glUseProgram(shaderProgram);

		//绘制图形
		glBindVertexArray(VAO);
		//重新将顶点数据复制到缓存
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 
		glDrawArrays(GL_POINTS, 0, counter);

		//imgui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		//glfwPollEvents();
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	//回收资源
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

