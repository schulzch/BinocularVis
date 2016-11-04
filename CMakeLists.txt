# Require CMake 3.3+ for sanity.
cmake_minimum_required(VERSION 3.3)

# Suppress in-source tree builds.
if(${CMAKE_BINARY_DIR} MATCHES "^${CMAKE_SOURCE_DIR}/src")
    message(FATAL_ERROR "Build directory must not be within source directory")
endif()

# Project name.
project(BinocularVis)

# Configure OpenCV.
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Configure OpenGL.
find_package(OpenGL REQUIRED)

# Configure Qt (note: set CMAKE_PREFIX_PATH, as the Qt documentation says).
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

# Enable C++11.
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
check_cxx_compiler_flag("-stdlib=libc++" COMPILER_SUPPORTS_STDLIB)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()
if(COMPILER_SUPPORTS_STDLIB)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

option(USE_OPENGL4 "Build with OpenGL 4.x support" ON)
if(USE_OPENGL4)
    message(STATUS "Building with OpenGL 4.x support")
    add_definitions(-DB_OPENGL4)
else()
    message(WARNING "Building with OpenGL 2.x support")
endif()

# Locate and group sources.
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "src/*.cpp" "src/*.h" "src/*.ui" "src/*.qrc" "src/*.rc")
file(GLOB_RECURSE GLSL_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "src/*.vert" "src/*.frag" "src/*.tesc " "src/*.tese"
    "src/*.geom" "src/*.comp")
foreach(FILE_NAME ${SOURCES} ${GLSL_SOURCES})
    get_filename_component(FILE_PATH ${FILE_NAME} PATH)
    string(REPLACE "/" "\\" GROUP_NAME ${FILE_PATH})
    string(REGEX REPLACE "^src" "Source Files" GROUP_NAME ${GROUP_NAME})
    source_group(${GROUP_NAME} FILES ${FILE_NAME})
endforeach()

# Setup target.
add_executable(binocularvis WIN32 ${SOURCES})
add_custom_target(binocularvis_glsl SOURCES ${GLSL_SOURCES})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(binocularvis ${OpenCV_LIBS})
qt5_use_modules(binocularvis Core Gui Widgets)
