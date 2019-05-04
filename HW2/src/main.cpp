#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
const char *vertexShaderStr = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 newColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"	newColor = aColor;\n"
"}\n\0";
const char *fragmentShaderStr = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 newColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(newColor, 1.0);\n"
"}\n\0";

float vertices[] = {
	//first triangle
	-0.8f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,	//left down
	-0.2f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,	//right down
	-0.5f,  0.7f, 0.0f, 1.0f, 0.0f, 0.0f,	//up
	//second triangle
	0.2f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,		//left down
	0.8f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,		//right down
	0.5f,  0.7f, 0.0f, 0.0f, 1.0f, 0.0f,		//up
	//third triangle
	0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,		//left
	0.1f,  -0.2f, 0.0f, 0.0f, 1.0f, 0.0f,	//down
	//line
	-0.8f, -0.2f, 0.0f, 1.0f, 0.0f, 0.0f,	//left
	0.8f, -0.3f, 0.0f, 1.0f, 0.0f, 0.0f,	//right
	//point
	0.0f, -0.4f, 0.0f, 1.0f, 0.0f, 0.0f,
	-0.02f, -0.4f, 0.0f, 1.0f, 0.0f, 0.0f,
	-0.04f, -0.4f, 0.0f, 1.0f, 0.0f, 0.0f,
	0.02f, -0.4f, 0.0f, 1.0f, 0.0f, 0.0f,
	0.04f, -0.4f, 0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, -0.42f, 0.0f, 1.0f, 0.0f, 0.0f,
	-0.02f, -0.42f, 0.0f, 1.0f, 0.0f, 0.0f,
	-0.04f, -0.42f, 0.0f, 1.0f, 0.0f, 0.0f,
	0.02f, -0.42f, 0.0f, 1.0f, 0.0f, 0.0f,
	0.04f, -0.42f, 0.0f, 1.0f, 0.0f, 0.0f,
};

unsigned int indices[] = { 
	3, 4, 5, // 第一个三角形
	3, 6, 7  // 第二个三角形
};

const char* glsl_version = "#version 330";
bool show_demo_window = true;
bool show_window = true;

int main()
{
	//实例化GLFW窗口
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//创建窗口对象
	GLFWwindow* window = glfwCreateWindow(1000, 600, "LearnOpenGL", NULL, NULL);
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//颜色属性
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

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

	ImVec4 myColor;

	while (!glfwWindowShouldClose(window))
	{
		//处理输入
		processInput(window);

		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//ImGui::ShowDemoWindow(&show_demo_window);
		ImGui::Begin("Change the color of right triangle");
		ImGui::ColorEdit3("Color", (float*)&myColor);

		for (int i = 0; i < 5; i++) {
			vertices[18 + i * 6 + 3] = myColor.x;
			vertices[18 + i * 6 + 4] = myColor.y;
			vertices[18 + i * 6 + 5] = myColor.z;
		}

		ImGui::End();
		 
		//渲染背景颜色
		glClearColor(0.2f, 0.2f, 0.25f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//激活程序
		glUseProgram(shaderProgram);

		//绘制图形
		glBindVertexArray(VAO);
		//重新将顶点数据复制到缓存
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		//EBO
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_LINES, 8, 2);
		glDrawArrays(GL_POINTS, 10, 10);

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
	glDeleteBuffers(1, &EBO);
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