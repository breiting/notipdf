cmake_minimum_required(VERSION 3.31.1)
project(notipdf VERSION 0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/CompilerSettings.cmake)
include(FetchContent)

# ----------------------------------------------------------
# CLI11
# ----------------------------------------------------------
FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.4.2
)
FetchContent_MakeAvailable(CLI11)

# ----------------------------------------------------------
# GLM
# ----------------------------------------------------------
FetchContent_Declare(
		glm
		GIT_REPOSITORY https://github.com/g-truc/glm.git
		GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

# ----------------------------------------------------------
# GLFW
# ----------------------------------------------------------
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

# ----------------------------------------------------------
# GLAD
# ----------------------------------------------------------
add_library(glad STATIC extern/glad/src/gl.cpp)
target_include_directories(glad PUBLIC extern/glad/include)

# ----------------------------------------------------------
# Google Test
# ----------------------------------------------------------
FetchContent_Declare(
		googletest
		URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
		DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

# ----------------------------------------------------------
# ImGui
# ----------------------------------------------------------
FetchContent_Declare(
	imgui
	GIT_REPOSITORY https://github.com/ocornut/imgui.git
	GIT_TAG docking
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
	${imgui_SOURCE_DIR}/imgui.cpp
	${imgui_SOURCE_DIR}/imgui_draw.cpp
	${imgui_SOURCE_DIR}/imgui_tables.cpp
	${imgui_SOURCE_DIR}/imgui_widgets.cpp
	${imgui_SOURCE_DIR}/imgui_demo.cpp
)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})

# ----------------------------------------------------------
# nlohmann/json
# ----------------------------------------------------------
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# ----------------------------------------------------------
# Homebrew MuPDF
# ----------------------------------------------------------
find_program(BREW_EXECUTABLE brew)
if(NOT BREW_EXECUTABLE)
    message(FATAL_ERROR "Homebrew not found. Install Homebrew first.")
endif()

execute_process(
    COMMAND ${BREW_EXECUTABLE} --prefix mupdf
    OUTPUT_VARIABLE MUPDF_PREFIX
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE MUPDF_PREFIX_RESULT
)

if(NOT MUPDF_PREFIX_RESULT EQUAL 0 OR MUPDF_PREFIX STREQUAL "")
    message(FATAL_ERROR "MuPDF not found via Homebrew. Run: brew install mupdf")
endif()

message(STATUS "MuPDF prefix: ${MUPDF_PREFIX}")

find_path(MUPDF_INCLUDE_DIR
    NAMES mupdf/fitz.h
    HINTS ${MUPDF_PREFIX}/include
    REQUIRED
)

find_library(MUPDF_LIBRARY
    NAMES mupdf
    HINTS ${MUPDF_PREFIX}/lib
    REQUIRED
)

find_library(MUPDF_THIRD_LIBRARY
    NAMES mupdf-third
    HINTS ${MUPDF_PREFIX}/lib
    REQUIRED
)

add_library(mupdf_external INTERFACE)
target_include_directories(mupdf_external INTERFACE ${MUPDF_INCLUDE_DIR})
target_link_libraries(mupdf_external INTERFACE ${MUPDF_LIBRARY} ${MUPDF_THIRD_LIBRARY})

# ----------------------------------------------------------
# App
# ----------------------------------------------------------
add_executable(notipdf
    src/Application.cpp
    src/Camera2D.cpp
    src/Logger.cpp
    src/PdfDocument.cpp
    src/PdfQuad.cpp
    src/PdfRenderer.cpp
    src/PdfTexture.cpp
    src/PdfViewerRenderer.cpp
    src/ViewerPanel.cpp
    src/Window.cpp
    src/main.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(notipdf
    PRIVATE
        inc
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(notipdf
    PRIVATE
        CLI11::CLI11
        glfw
        glad
        glm::glm
        imgui
        nlohmann_json::nlohmann_json
        mupdf_external
)

if(APPLE)
    target_link_libraries(notipdf
        PRIVATE
            "-framework Cocoa"
            "-framework OpenGL"
            "-framework IOKit"
            "-framework CoreVideo"
    )
endif()
