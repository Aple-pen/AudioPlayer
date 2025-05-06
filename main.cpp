#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define WIDTH 1920
#define HEIGHT 1080

GLFWwindow *window;

bool CreateGLWindow()
{
    // 윈도우 생성
    window = glfwCreateWindow(WIDTH, HEIGHT, "Dear ImGui + OpenGL 4.1", nullptr, nullptr);

    if (!window)
    {
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // VSync 활성화
    // ✅ GLAD 초기화
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        return false;
    }
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    
#ifdef __APPLE__
    ImGui_ImplOpenGL3_Init("#version 410");
#else
    ImGui_ImplOpenGL3_Init("#version 460");
#endif
    return true;
}

void glfw_error_callback(int error, const char *description)
{
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

bool IsInit()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return false;
#ifdef __APPLE__
    // 🍎 macOS: OpenGL 4.1 + Forward Compatible
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // macOS는 필수
#else
    // 🪟 Windows (또는 다른 플랫폼): 원하는 버전으로 설정 (예: 4.6)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Forward Compatibility는 Windows에선 보통 필요 없음
#endif
    return true;
}
bool InitImGui()
{
    if (!IsInit()) {
        return false;
    }
    // ImPlot::CreateContext();
    ImGui::CreateContext();
    // SetFont();

    // Dear ImGui 초기화
    IMGUI_CHECKVERSION();
    return true;
}

int main()
{
    if (!InitImGui() || !CreateGLWindow())
        return -1;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui 렌더링
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}