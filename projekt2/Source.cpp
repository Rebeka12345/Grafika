enum eVertexArrayObject {
    VAOVerticesData,
    VAOCount
};
enum eBufferObject {
    VBOVerticesData,
    BOCount
};
enum eProgram {
    QuadScreenProgram,
    ProgramCount
};
enum eTexture {
    NoTexture,
    TextureCount
};

#include <common.cpp>

GLchar windowTitle[] = "Drag-and-Drop";
GLfloat aspectRatio;
GLint dragged = -1;
GLfloat pointSize = 10.0f; 


static vector<vec2> verticesData = {
    vec2(-0.5f, -0.5f),
    vec2(-0.5f,  0.5f),
    vec2(0.5f,  0.5f),
    vec2(0.5f, -0.5f)
};

void initShaderProgram() {
    ShaderInfo shader_info[ProgramCount][3] = { {
        { GL_VERTEX_SHADER, "./vertexShader.glsl" },
        { GL_FRAGMENT_SHADER, "./fragmentShader.glsl" },
        { GL_NONE, nullptr } } };

    for (int programItem = 0; programItem < ProgramCount; programItem++) {
        program[programItem] = LoadShaders(shader_info[programItem]);
        locationMatModel = glGetUniformLocation(program[programItem], "matModel");
        locationMatView = glGetUniformLocation(program[programItem], "matView");
        locationMatProjection = glGetUniformLocation(program[programItem], "matProjection");
    }

    glBindVertexArray(VAO[VAOVerticesData]);
    glBindBuffer(GL_ARRAY_BUFFER, BO[VBOVerticesData]);
    glBufferData(GL_ARRAY_BUFFER, verticesData.size() * sizeof(vec2), verticesData.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), 0);

    glUseProgram(program[QuadScreenProgram]);

    matModel = mat4(1.0);
    matView = lookAt(vec3(0.0f, 0.0f, 9.0f),
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(locationMatModel, 1, GL_FALSE, value_ptr(matModel));
    glUniformMatrix4fv(locationMatView, 1, GL_FALSE, value_ptr(matView));
    glUniformMatrix4fv(locationMatProjection, 1, GL_FALSE, value_ptr(matProjection));
}

GLfloat distanceSquare(vec2 p1, vec2 p2) {
    vec2 delta = p1 - p2;
    return dot(delta, delta);
}


vec2 bezierPoint(float t, const vector<vec2>& points) {
    vector<vec2> temp = points;
    int n = temp.size();
    for (int k = 1; k < n; k++) {
        for (int i = 0; i < n - k; i++) {
            temp[i] = (1.0f - t) * temp[i] + t * temp[i + 1];
        }
    }
    return temp[0];
}

// Billentyű eseménykezelés
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    /** ESC billentyûre kilépés. */
    if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

// Aktív pont kiválasztása az egér pozíció alapján
GLint getActivePoint(vector<vec2> p, GLfloat sensitivity, vec2 mousePosition) {
    GLfloat sensitivitySquare = sensitivity * sensitivity;
    for (GLint i = 0; i < p.size(); i++)
        if (distanceSquare(p[i], mousePosition) < sensitivitySquare)
            return i;
    return -1;
}

// Kirajzolás
void display(GLFWwindow* window, double currentTime) {
    glClear(GL_COLOR_BUFFER_BIT);

    // Állítsuk be a programot
    glUseProgram(program[QuadScreenProgram]);
    glBindVertexArray(VAO[VAOVerticesData]);
    glBindBuffer(GL_ARRAY_BUFFER, BO[VBOVerticesData]);

    // Kontrollpontok kirajzolása
    glPointSize(7.0f); // pontok mérete 10 pixel
    glUniform3f(glGetUniformLocation(program[QuadScreenProgram], "uColor"), 0.85f, 0.2f, 0.5f); // ciklámen
    glBufferData(GL_ARRAY_BUFFER, verticesData.size() * sizeof(vec2), verticesData.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_POINTS, 0, verticesData.size());

    // Kontrollpoligon kirajzolása (nem záródik vissza)
    glUniform3f(glGetUniformLocation(program[QuadScreenProgram], "uColor"), 1.0f, 0.7f, 0.8f); // rózsaszín
    glDrawArrays(GL_LINE_STRIP, 0, verticesData.size());

    // Bézier-görbe számítása
    vector<vec2> curvePoints;
    for (float t = 0.0f; t <= 1.0f; t += 0.01f)
        curvePoints.push_back(bezierPoint(t, verticesData));

    // Bézier-görbe kirajzolása
    glUniform3f(glGetUniformLocation(program[QuadScreenProgram], "uColor"), 0.6f, 0.8f, 1.0f); // kék
    glBufferData(GL_ARRAY_BUFFER, curvePoints.size() * sizeof(vec2), curvePoints.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_LINE_STRIP, 0, curvePoints.size());

    // Kontrollpontok visszaállítása a bufferbe a további drag-and-drophoz
    glBufferData(GL_ARRAY_BUFFER, verticesData.size() * sizeof(vec2), verticesData.data(), GL_DYNAMIC_DRAW);
}

// Ablakméret változás kezelése
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    windowWidth = glm::max(width, 1);
    windowHeight = glm::max(height, 1);

    aspectRatio = (float)windowWidth / (float)windowHeight;
    glViewport(0, 0, windowWidth, windowHeight);

    if (projectionType == Orthographic)
        if (windowWidth < windowHeight)
            matProjection = ortho(-worldSize, worldSize, -worldSize / aspectRatio, worldSize / aspectRatio, -100.0, 100.0);
        else
            matProjection = ortho(-worldSize * aspectRatio, worldSize * aspectRatio, -worldSize, worldSize, -100.0, 100.0);
    else
        matProjection = perspective(radians(45.0f), aspectRatio, 0.1f, 100.0f);

    glUniformMatrix4fv(locationMatProjection, 1, GL_FALSE, value_ptr(matProjection));
}

// Egér mozgatás
void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
    if (dragged >= 0) {
        dvec2 mousePosition;
        mousePosition.x = xPos * 2.0 / (GLdouble)windowWidth - 1.0;
        mousePosition.y = ((GLdouble)windowHeight - yPos) * 2.0 / (GLdouble)windowHeight - 1.0;

        if (windowWidth < windowHeight)
            mousePosition.y /= aspectRatio;
        else
            mousePosition.x *= aspectRatio;

        // Pont mozgatása
        verticesData[dragged] = mousePosition;

        glBindBuffer(GL_ARRAY_BUFFER, BO[VBOVerticesData]);
        glBufferData(GL_ARRAY_BUFFER, verticesData.size() * sizeof(vec2), verticesData.data(), GL_DYNAMIC_DRAW);
    }
}

// Egérgomb események
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    dvec2 mousePosition;

    // Egér pozíció lekérdezése
    glfwGetCursorPos(window, &mousePosition.x, &mousePosition.y);
    mousePosition.x = mousePosition.x * 2.0 / (GLdouble)windowWidth - 1.0;
    mousePosition.y = ((GLdouble)windowHeight - mousePosition.y) * 2.0 / (GLdouble)windowHeight - 1.0;

    // Aspect ratio korrekció
    if (windowWidth < windowHeight)
        mousePosition.y /= aspectRatio;
    else
        mousePosition.x *= aspectRatio;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // Megnézzük, hogy van-e már pont közel a kattintáshoz
        dragged = getActivePoint(verticesData, 0.1f, mousePosition);

        if (dragged == -1) {
            // Üres helyre kattintás -> új kontrollpont hozzáadása
            verticesData.push_back(mousePosition);
            dragged = verticesData.size() - 1; // rögtön mozgatható lesz

            // GPU buffer frissítése
            glBindBuffer(GL_ARRAY_BUFFER, BO[VBOVerticesData]);
            glBufferData(GL_ARRAY_BUFFER, verticesData.size() * sizeof(vec2), verticesData.data(), GL_DYNAMIC_DRAW);
        }
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        // Drag-and-drop leállítása
        dragged = -1;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        // Jobb gomb -> meglévő pont törlése
        GLint index = getActivePoint(verticesData, 0.1f, mousePosition);

        if (index >= 0 && verticesData.size() > 1) {
            verticesData.erase(verticesData.begin() + index);

            // GPU buffer frissítése
            glBindBuffer(GL_ARRAY_BUFFER, BO[VBOVerticesData]);
            glBufferData(GL_ARRAY_BUFFER, verticesData.size() * sizeof(vec2), verticesData.data(), GL_DYNAMIC_DRAW);
        }
    }
}

// Főprogram
int main(void) {
    init(3, 3, GLFW_OPENGL_COMPAT_PROFILE);
    initShaderProgram();
    framebufferSizeCallback(window, windowWidth, windowHeight);

    // Callback-ok beállítása
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    while (!glfwWindowShouldClose(window)) {
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanUpScene(EXIT_SUCCESS);
    return EXIT_SUCCESS;
}
//glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
