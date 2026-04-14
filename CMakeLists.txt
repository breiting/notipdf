cmake_minimum_required(VERSION 3.28)
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
set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
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
target_include_directories(glad PUBLIC extern/glad/include extern/stb)

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
# MuPDF (portable: Homebrew + Linux)
# ----------------------------------------------------------

# Try Homebrew first (macOS)
find_program(BREW_EXECUTABLE brew)

if(BREW_EXECUTABLE)
    execute_process(
        COMMAND ${BREW_EXECUTABLE} --prefix mupdf
        OUTPUT_VARIABLE MUPDF_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE MUPDF_PREFIX_RESULT
    )
endif()

# If brew worked, use it
if(BREW_EXECUTABLE AND MUPDF_PREFIX_RESULT EQUAL 0 AND NOT MUPDF_PREFIX STREQUAL "")
    message(STATUS "Using MuPDF from Homebrew: ${MUPDF_PREFIX}")

    set(MUPDF_HINTS
        ${MUPDF_PREFIX}/include
        ${MUPDF_PREFIX}/lib
    )
else()
    message(STATUS "Using system MuPDF (Linux or manual install)")

    set(MUPDF_HINTS
        /usr/include
        /usr/local/include
        /usr/lib
        /usr/local/lib
    )
endif()

# Include dir
find_path(MUPDF_INCLUDE_DIR
    NAMES mupdf/fitz.h
    HINTS ${MUPDF_HINTS}
    PATH_SUFFIXES include
    REQUIRED
)

# Main lib
find_library(MUPDF_LIBRARY
    NAMES mupdf
    HINTS ${MUPDF_HINTS}
    PATH_SUFFIXES lib
    REQUIRED
)

# Third-party lib (optional)
find_library(MUPDF_THIRD_LIBRARY
    NAMES mupdf-third
    HINTS ${MUPDF_HINTS}
    PATH_SUFFIXES lib
)

# Debug output
message(STATUS "MUPDF_INCLUDE_DIR: ${MUPDF_INCLUDE_DIR}")
message(STATUS "MUPDF_LIBRARY: ${MUPDF_LIBRARY}")
message(STATUS "MUPDF_THIRD_LIBRARY: ${MUPDF_THIRD_LIBRARY}")

##############################################
# Add additional requirements mainly for Linux
##############################################
if(UNIX AND NOT APPLE)
    find_package(Freetype REQUIRED)
    find_package(ZLIB REQUIRED)
    find_package(JPEG REQUIRED)
    find_library(OPENJPEG_LIBRARY NAMES openjp2 REQUIRED)
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(HARFBUZZ REQUIRED harfbuzz)
    find_library(JBIG2DEC_LIBRARY NAMES jbig2dec REQUIRED)
    find_library(GUMBO_LIBRARY NAMES gumbo REQUIRED)
    find_library(MUJS_LIBRARY NAMES mujs REQUIRED)

    add_library(mupdf_external INTERFACE)
    target_include_directories(mupdf_external
        INTERFACE
            ${MUPDF_INCLUDE_DIR}
            ${HARFBUZZ_INCLUDE_DIRS}
    )

    target_link_libraries(mupdf_external
        INTERFACE
            "-Wl,--start-group"
            ${MUPDF_LIBRARY}
            ${MUPDF_THIRD_LIBRARY}
            Freetype::Freetype
            ZLIB::ZLIB
            JPEG::JPEG
            ${OPENJPEG_LIBRARY}
            ${JBIG2DEC_LIBRARY}
            ${HARFBUZZ_LIBRARIES}
            ${GUMBO_LIBRARY}
            ${MUJS_LIBRARY}
            "-Wl,--end-group"
    )
else()
    add_library(mupdf_external INTERFACE)
    target_include_directories(mupdf_external
        INTERFACE
            ${MUPDF_INCLUDE_DIR}
    )
    target_link_libraries(mupdf_external
        INTERFACE
            ${MUPDF_LIBRARY}
            ${MUPDF_THIRD_LIBRARY}
    )
endif()

# ----------------------------------------------------------
# App
# ----------------------------------------------------------
add_executable(notipdf
    src/Application.cpp
    src/Camera2D.cpp
    src/LineRenderer.cpp
    src/PdfImageWriter.cpp
    src/PdfSelectionRenderer.cpp
    src/Logger.cpp
    src/PdfDocument.cpp
    src/PdfExporter.cpp
    src/ProcessRunner.cpp
    src/MetaWriter.cpp
    src/ImageOptimizer.cpp
    src/ConfigService.cpp
    src/LibraryCatalog.cpp
    src/PdfQuad.cpp
    src/PdfRenderer.cpp
    src/PdfTexture.cpp
    src/PdfViewerRenderer.cpp
    src/ViewerMapping.cpp
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
