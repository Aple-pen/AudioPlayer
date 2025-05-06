#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Import.h"
#include "implot.h"

#define WIDTH 1920
#define HEIGHT 1080

GLFWwindow *window;


Import import;

double elapsedSeconds = 1.0;
std::chrono::steady_clock::time_point playbackStart;
bool isPlaying = false;

float x = elapsedSeconds;
float y_min = -1.5f;  // 또는 현재 y축 하한
float y_max = 3.5f;   // 또는 현재 y축 상한

float x_vals[2] = {x, x};
float y_vals[2] = {y_min, y_max};

bool CreateGLWindow()
{
    // 윈도우 생성
    window = glfwCreateWindow(WIDTH, HEIGHT, "AudioPlayer", nullptr, nullptr);

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
    ImPlot::CreateContext();
    // SetFont();

    // Dear ImGui 초기화
    IMGUI_CHECKVERSION();
    return true;
}
static int TimeFormatter(double value, char* buff, int size, void* user_data) {
    int total_ms = static_cast<int>(value * 1000.0);  // 전체 밀리초
    int hours = total_ms / (3600 * 1000);
    int minutes = (total_ms / (60 * 1000)) % 60;
    int seconds = (total_ms / 1000) % 60;
    int millis = total_ms % 1000;

    snprintf(buff, size, "%02d:%02d.%03d", minutes, seconds, millis);
    return 0;
}


void Render(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Waveform Viewer");

    if (ImPlot::BeginPlot("Stereo Overlay", "Time", "Amplitude", ImVec2(-1, 400),ImPlotFlags_NoLegend)) {
        ImPlot::SetupAxisFormat(ImAxis_X1, TimeFormatter);
        ImPlot::SetupAxisLimits(ImAxis_Y1, -1.5, 3.5, ImGuiCond_Always); // Y축 고정
        ImPlot::SetupAxisZoomConstraints(ImAxis_Y1, 5.0, 5.0); // 줌 제한 (Y축 고정)
        ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks);
        // 오른쪽 채널을 아래로
        ImPlot::PlotLine("Right", import.time.data(), import.right_wave.data(), import.right_wave.size());
        ImPlot::PlotLine("Left", import.time.data(), import.left_wave.data(), import.left_wave.size());

        ImPlot::PlotLine("Playhead", x_vals, y_vals, 2);

        ImPlot::EndPlot();
    }

    ImGui::End();

    if (ImGui::Button("▶ Play")) {
        playbackStart = std::chrono::steady_clock::now();
        isPlaying = true;
    }
    // ImGui 렌더링
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

//    ImPlot::PlotLine("L", import.left_wave.data(), import.left_wave.size());
    glfwSwapBuffers(window);

}
void refresh_callback(GLFWwindow *window) {
    Render();


}

int main()
{
    if (!InitImGui() || !CreateGLWindow())
        return -1;

    glfwSetWindowRefreshCallback(window, refresh_callback);


    if(import.GetMp3()){
        import.GetInfo();
    }

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        Render();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}