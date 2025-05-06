cmake_minimum_required(VERSION 3.14)
project(AudioPlayer)

set(CMAKE_CXX_STANDARD 17)
# 플랫폼별 설정
if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "14.5")
#     set(CMAKE_OSX_SYSROOT "/Library/Developer/CommandLineTools/SDKs/MacOSX14.5.sdk" CACHE STRING "SDK Path for macOS 14.5" FORCE)
#     set(CMAKE_OSX_DEPLOYMENT_TARGET "14.5" CACHE STRING "Deployment target for macOS 14.5" FORCE)
elseif (WIN32)
    # Windows는 특별한 설정 없이 진행 가능
endif()

include(FetchContent)

# 🔹 GLFW 다운로드 및 추가 (OpenGL 4.1 설정)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.8
)
FetchContent_MakeAvailable(glfw)

# 🔹 GLAD 다운로드 및 추가 (OpenGL 4.1 제한)
FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
        GIT_TAG master
)
FetchContent_MakeAvailable(glad)

# glad CMake 옵션 설정 (OpenGL + loader + C)
set(GLAD_PROFILE "core")
set(GLAD_API "gl")
set(GLAD_GENERATOR "c")

if(APPLE)
    set(GLAD_VERSION "4.1")
else()
    set(GLAD_VERSION "4.6")
endif()

# 🔹 Dear ImGui 다운로드 및 추가
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG docking
)
FetchContent_MakeAvailable(imgui)

# FetchContent를 통한 ImPlot 자동 다운로드
include(FetchContent)
FetchContent_Declare(
        implot
        GIT_REPOSITORY https://github.com/epezent/implot.git
        GIT_TAG v0.16  # 최신 버전 태그 확인 후 설정 권장
)
FetchContent_MakeAvailable(implot)

# spdlog 외부에서 자동으로 받기
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.13.0  # 최신 안정 버전으로 설정 가능
)

# spdlog 다운로드 및 준비
FetchContent_MakeAvailable(spdlog)

# ImGui 및 ImPlot 소스 추가로
file(GLOB IMGUI_SOURCES
        ${imgui_SOURCE_DIR}/*.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        )

if (APPLE)
    # ✅ macOS 14.5 SDK를 강제 적용
    set(CMAKE_OSX_SYSROOT "/Library/Developer/CommandLineTools/SDKs/MacOSX14.5.sdk" CACHE STRING "SDK Path for macOS 14.5" FORCE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "14.5" CACHE STRING "Deployment target for macOS 14.5" FORCE)

    # OpenGL & 시스템 프레임워크 추가
    find_library(OpenGL_LIBRARY OpenGL)
    mark_as_advanced(OpenGL_LIBRARY)

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ObjC -framework Cocoa -framework IOKit -framework CoreFoundation -framework CoreVideo -framework ${OpenGL_LIBRARY}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif ()


# 🔹 ImGui 라이브러리 추가
add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        )
set_target_properties(imgui PROPERTIES CXX_STANDARD 17)

target_include_directories(imgui PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/Utils
        ${CMAKE_CURRENT_SOURCE_DIR}/Common
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${implot_SOURCE_DIR}
        )

target_link_libraries(imgui PUBLIC glfw glad ${OpenGL_LIBRARY})


# 🔹 실행 파일 생성
add_executable(AudioPlayer MACOSX_BUNDLE main.cpp
        ${implot_SOURCE_DIR}/implot.cpp
        ${implot_SOURCE_DIR}/implot_items.cpp # 이 파일 누락 시 링크 오류 발생
        Utils/Import.cpp Utils/Import.h Utils/Import.h)

target_include_directories(AudioPlayer PRIVATE third_party/minimp3)
target_link_libraries(AudioPlayer PRIVATE imgui glfw glad ${OpenGL_LIBRARY})

target_link_libraries(AudioPlayer PRIVATE spdlog::spdlog)
        
# # fonts 폴더를 빌드 디렉토리로 복사
# add_custom_command(TARGET AudioPlayer POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy_directory
#         ${CMAKE_SOURCE_DIR}/fonts $<TARGET_FILE_DIR:AudioPlayer>/fonts
#         )