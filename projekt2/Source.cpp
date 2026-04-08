#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>

using namespace std;
using namespace glm;

// Ablak
int windowWidth = 800;
int windowHeight = 800;
float aspectRatio = 1.0f;

// Kontrollpontok
vector<vec2> points;
int dragged = -1;

// Shader
GLuint program;
GLuint VBO, VAO;

// ===== SHADEREK =====
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
uniform vec3 color;
out vec4 FragColor;
void main() {
    FragColor = vec4(color, 1.0);
}
)";

// ===== SEGÉDFÜGGVÉNYEK =====

float dist2(vec2 a, vec2 b) {
    vec2 d = a - b;
    return dot(d, d);
}

// De Casteljau
vec2 bezier(float t, vector<vec2> p) {
    int n = p.size();
    for (int k = 1; k < n; k++)
        for (int i = 0; i < n - k; i++)
            p[i] = (1 - t) * p[i] + t * p[i + 1];
    return p[0];
}

// ===== INPUT =====

vec2 screenToWorld(double x, double y) {
    vec2 p;
    p.x = (x / windowWidth) * 2.0f - 1.0f;
    p.y = 1.0f - (y / windowHeight) * 2.0f;

    if (windowWidth < windowHeight)
        p.y /= aspectRatio;
    else
        p.x *= aspectRatio;

    return p;
}

int getPoint(vec2 m) {
    for (int i = 0; i < points.size(); i++)
        if (dist2(points[i], m) < 0.02f)
            return i;
    return -1;
}

void mouseButton(GLFWwindow* window, int button, int action, int mods) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    vec2 m = screenToWorld(x, y);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        dragged = getPoint(m);
        if (dragged == -1) {
            points.push_back(m);
            dragged = points.size() - 1;
        }
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        dragged = -1;

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        int idx = getPoint(m);
        if (idx >= 0)
            points.erase(points.begin() + idx);
    }
}

void cursorPos(GLFWwindow* window, double x, double y) {
    if (dragged >= 0) {
        points[dragged] = screenToWorld(x, y);
    }
}

// ===== RAJZOLÁS =====

void draw(vector<vec2>& data, GLenum mode, vec3 color) {
    glUniform3f(glGetUniformLocation(program, "color"), color.r, color.g, color.b);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(vec2), data.data(), GL_DYNAMIC_DRAW);

    glDrawArrays(mode, 0, data.size());
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Kontrollpontok
    glPointSize(7.0f); // ✔ megfelel
    draw(points, GL_POINTS, vec3(0.2f, 1.0f, 0.2f));

    // Kontrollpoligon
    draw(points, GL_LINE_STRIP, vec3(0.7f, 0.7f, 0.7f));

    // Bézier görbe
    if (points.size() >= 2) {
        vector<vec2> curve;

        int resolution = 300; // ✔ simább
        for (int i = 0; i <= resolution; i++) {
            float t = (float)i / resolution;
            curve.push_back(bezier(t, points));
        }

        glLineWidth(3.0f);
        draw(curve, GL_LINE_STRIP, vec3(1.0f, 0.2f, 0.2f));
    }
}

// ===== INIT =====

GLuint compileShader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);

    int success;
    glGetShaderiv(s, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(s, 512, NULL, log);
        cout << log << endl;
    }
    return s;
}

void init() {
    glewInit();

    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glUseProgram(program);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), 0);
    glEnableVertexAttribArray(0);

    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // ✔ szebb háttér
}

// ===== MAIN =====

int main() {
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Bezier", NULL, NULL);
    glfwMakeContextCurrent(window);

    init();

    glfwSetMouseButtonCallback(window, mouseButton);
    glfwSetCursorPosCallback(window, cursorPos);

    while (!glfwWindowShouldClose(window)) {
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        windowWidth = w;
        windowHeight = h;
        aspectRatio = (float)w / h;

        glViewport(0, 0, w, h);

        display();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}