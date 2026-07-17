#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <cmath>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <shader.h>

using std::cout, std::endl, std::string, std::vector;

//settings
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float storage = 0.0f;
float stopwatch = 0.0f;

unsigned int width = 800;
unsigned int height = 600;


constexpr float MIN_VALID_PRESS = 0.03f;
constexpr float TAP_THRESHOLD = 0.4f;
constexpr float OPERATOR_SELECT_WINDOW = 0.1f;

struct Timer {
	float clickTime = 0.0f;
	float mouseTime = 0.0f;
	float counter = 0.0f;
	float output = 0.0f;
	bool mouseLeftPressed = false;
};



string outputString = "";


float lastX = width / 2.0f;
float lastY = height / 2.0f;

bool clickMode = true;
unsigned int clicks = 0;
const char* operation = "";
const char* print = "";
string number = "";



//functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void vectorOut(std::vector<string> Vector);
double evaluateExpression(const std::vector<string>& tokens);
void finalizeExpression();
std::string formatResult(double value);

void uiDefine(ImGuiIO io);
void uiDraw();

float quadVertices[] = {
	// positions	// texCoords
	-1.0f,  0.4f,	0.0f, 1.0f,
	-1.0f, -1.0f,	0.0f, 0.0f,
	 1.0f, -1.0f,	1.0f, 0.0f,
	-1.0f,  0.4f,	0.0f, 1.0f,
	 1.0f, -1.0f,	1.0f, 0.0f,
	 1.0f,  0.4f,	1.0f, 1.0f
};

std::vector<std::string> sum;
Timer numberTimer;
Timer operatorTimer;
//****************************************************************************************************************************************
int main() {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(width, height, "HELLCULATOR", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//important -> load all the function pointers for openGL before calling any of its functions
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {

		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	//setting up imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	//Setup platform for imgui
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");


	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	//quad VAO, VBO
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);

	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	//VertexPos Attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//texture attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	Shader quadShader(R"(shaders\temp.vert)", R"(shaders\temp.frag)");



	//######################################################################################################
	//render loop
	glViewport(0, 0, width, height);
	while (!glfwWindowShouldClose(window))
	{
		float currentTime = (float)glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;

		stopwatch += deltaTime;

		//the one second repeater
		if (stopwatch > 1.0f) {
			stopwatch = 0.0f;

			//cycle digit while held (unchanged from original — this part wasn't the bug)
			if (numberTimer.mouseLeftPressed) {
				clicks = 0;

				storage = numberTimer.counter + 1.0f; //so the counter starts from 1

				if (storage >= 10.0f) {
					storage = 0.0f;
					numberTimer.clickTime = (float)glfwGetTime();
				}

				cout << storage << endl;
			}

			//choosing operator
			operatorTimer.counter = floor((float)glfwGetTime() - operatorTimer.clickTime);

			if (operatorTimer.counter > OPERATOR_SELECT_WINDOW && clicks != 0) {
				if (clicks == 1) {
					operation = "+";
				}
				else if (clicks == 2) {
					operation = "-";
				}
				else if (clicks == 3) {
					operation = "*";
				}
				else if (clicks == 4) {
					operation = "/";
				}
				else if (clicks == 5) {
					operation = "=";
				}
				else {
					// clicks somehow out of range (shouldn't happen, but guard anyway)
					clicks = 0;
					operation = "";
				}

				if (operation[0] != '\0' && clicks > 0 && (!number.empty() || !sum.empty())) {

					if (!number.empty()) {
						sum.push_back(number);
						number.clear();
					}

					sum.push_back(operation);
					vectorOut(sum);



					if (clicks == 5) {
						finalizeExpression();
					}
				}
				clicks = 0;
			}

		}

		if (numberTimer.mouseLeftPressed) {
			numberTimer.counter = floor((float)glfwGetTime() - numberTimer.clickTime);

		}

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		quadShader.use();
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//UI
		uiDefine(io);	//Design
		uiDraw();		//Render


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

}

void framebuffer_size_callback(GLFWwindow* window, int fwidth, int fheight) {

	glViewport(0, 0, fwidth, fheight);

	width = fwidth;
	height = fheight;

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
		if (lastY / height > 0.3f) {

			if (!outputString.empty() && sum.empty() && number.empty()) {
				outputString.clear();
			}


			if (numberTimer.mouseLeftPressed == false) {
				numberTimer.clickTime = (float)glfwGetTime();
			}
			numberTimer.mouseLeftPressed = true;
		}
	}

	if (button == GLFW_MOUSE_BUTTON_1 && action != GLFW_PRESS) {


		if (!numberTimer.mouseLeftPressed) {
			return;
		}

		float heldSeconds = (float)glfwGetTime() - numberTimer.clickTime;

		numberTimer.mouseLeftPressed = false;
		numberTimer.counter = 0.0f;

		// Debounce: throw out accidental micro-clicks (mouse bounce, etc.)
		if (heldSeconds < MIN_VALID_PRESS) {
			storage = 0.0f;
			return;
		}

		if (heldSeconds < TAP_THRESHOLD) {

			operatorTimer.clickTime = (float)glfwGetTime();
			clicks++;
			if (clicks > 5) clicks = 0; // safety wrap, shouldn't normally trigger
			if (clicks == 1) {
				print = "+";
				cout << "current operator: " << print << endl;
			}
			else if (clicks == 2) {
				print = "-";
				cout << "current operator: " << print << endl;
			}
			else if (clicks == 3) {
				print = "*";
				cout << "current operator: " << print << endl;
			}
			else if (clicks == 4) {
				print = "/";
				cout << "current operator: " << print << endl;
			}
			else if (clicks == 5) {
				print = "=";
				cout << "current operator: " << print << endl;
			}
			else if (clicks == 0) {
				print = "none";
				cout << "current operator: " << print << endl;
			}
		}
		else {
			// HOLD -> commit the current cycled digit to the number buffer.
			clickMode = false;
			int digit = (int)storage;
			cout << "value: " << digit << endl;
			number += std::to_string(digit);
		}

		storage = 0.0f;
	}
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; //reversed, y goes bottom to top
	lastX = xpos;
	lastY = ypos;

}

void vectorOut(std::vector<string> Vector) {
	for (unsigned int i = 0; i < Vector.size(); i++) {
		cout << Vector[i] << " ";
	}
	cout << endl;
}

//op = operator
//rhs = right hand side
double evaluateExpression(const std::vector<string>& tokens) {
	std::vector<string> clean;
	clean.reserve(tokens.size());
	for (const auto& t : tokens) {
		if (t != "=") clean.push_back(t);
	}

	if (clean.empty()) return 0.0;

	try {
		// Pass 1: fold * and / into single operands
		std::vector<double> nums;
		std::vector<char> ops;
		nums.push_back(std::stod(clean[0]));

		for (size_t i = 1; i + 1 < clean.size(); i += 2) {
			const string& op = clean[i];
			double rhs = std::stod(clean[i + 1]);

			if (op == "*") {
				nums.back() *= rhs;
			}
			else if (op == "/") {
				if (rhs == 0.0) {
					cout << "Error: division by zero" << endl;
					return nums.back();
				}
				nums.back() /= rhs;
			}
			else {
				ops.push_back(op[0]);
				nums.push_back(rhs);
			}
		}

		// Pass 2: left to right + and -
		double result = nums[0];
		for (size_t i = 0; i < ops.size(); i++) {
			if (ops[i] == '+') result += nums[i + 1];
			else if (ops[i] == '-') result -= nums[i + 1];
		}
		return result;
	}
	catch (const std::exception&) {
		cout << "Error: malformed expression" << endl;
		return 0.0;
	}
}

void finalizeExpression() {
	vectorOut(sum); 

	double result = evaluateExpression(sum);
	cout << "= " << result << endl;
	outputString = "= " + formatResult(result);

	sum.clear();
	number.clear();
}

void uiDefine(ImGuiIO io) {
	//remember this is being called each frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	{
		//Calculator display config
		ImGuiWindowFlags display_flags = (ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs);

		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2((float)width, (float)height * 0.3f));

		ImGui::Begin("OutputWindow", nullptr, display_flags);
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

			//making the equation string(the resultString might be excess then make sure to remove duplicates)
			std::string currentEquation = "";
			for (const auto& token : sum) {
				currentEquation += token + " ";
			}
			currentEquation += number; // the number being typed currently

			// Display current equation
			if (!currentEquation.empty()) {
				ImGui::SetWindowFontScale(3.0f); // Make text larger
				float textWidth = ImGui::CalcTextSize(currentEquation.c_str()).x;

				// Center horizontally, push down slightly
				ImGui::SetCursorPosX(((float)width - textWidth) * 0.5f);
				ImGui::SetCursorPosY((float)height * 0.3f * 0.25f);
				ImGui::Text("%s", currentEquation.c_str());
			}
			// Display calculated result if there is one
			else if (!outputString.empty()) {
				ImGui::SetWindowFontScale(3.0f);
				float textWidth = ImGui::CalcTextSize(outputString.c_str()).x;
				ImGui::SetCursorPosX(((float)width - textWidth) * 0.5f);
				ImGui::SetCursorPosY((float)height * 0.3f * 0.25f);
				ImGui::Text("%s", outputString.c_str());
			}
			else {
				// Idle state helper text inside the white box
				ImGui::SetWindowFontScale(3.0f);
				std::string welcome = "0";
				float textWidth = ImGui::CalcTextSize(welcome.c_str()).x;
				ImGui::SetCursorPosX(((float)width - textWidth) * 0.5f);
				ImGui::SetCursorPosY((float)height * 0.3f * 0.35f);
				ImGui::Text("%s", welcome.c_str());
			}

			ImGui::PopStyleColor();

		}
		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(0.0f, (float)height * 0.3f));
		ImGui::SetNextWindowSize(ImVec2((float)width, (float)height * 0.7f));

		ImGui::Begin("BottomZone", nullptr, display_flags);
		{
			// Use light text for the grey background
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.95f, 1.0f));
			ImGui::SetWindowFontScale(1.8f);

			std::string actionText = "";

			// Determine what active state to show
			if (numberTimer.mouseLeftPressed) {
				actionText = "Cycling Digit: " + std::to_string((int)storage);
			}
			else if (clicks > 0) {
				actionText = "Selecting Operator: " + std::string(print) + " (" + std::to_string(clicks) + " clicks)";
			}
			else {
				actionText = "[ HOLD ] to cycle numbers  |  [ CLICK ] to choose operator";
				ImGui::SetWindowFontScale(1.2f); // Shrink helper text slightly
			}

			// Center the action text
			float textWidth = ImGui::CalcTextSize(actionText.c_str()).x;
			ImGui::SetCursorPosX(((float)width - textWidth) * 0.5f);
			ImGui::SetCursorPosY((float)height * 0.7f * 0.4f); // Middle of the grey box
			ImGui::Text("%s", actionText.c_str());

			ImGui::PopStyleColor();
		}
		ImGui::End();
	}
};
	

std::string formatResult(double value) {
	// Print as integer if it's a whole number, otherwise trim trailing zeros
	if (value == std::floor(value)) {
		return std::to_string((long long)value);
	}
	std::string s = std::to_string(value);
	s.erase(s.find_last_not_of('0') + 1, std::string::npos);
	if (!s.empty() && s.back() == '.') s.pop_back();
	return s;
}



void uiDraw() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}