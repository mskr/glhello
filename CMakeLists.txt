cmake_minimum_required(VERSION 3.0)
project(glhello)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(glfw)
add_subdirectory(glm)
include_directories(glew/include/ glew/lib/Release/Win32/ glfw/include glm/glm/ glm/glm/gtc/ glm/glm/gtx)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	${CMAKE_CURRENT_SOURCE_DIR}/glew/lib/Release/Win32/glew32s.lib
	glfw
	${GLFW_LIBRARIES}
)

add_definitions(
	-DGLEW_STATIC
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -pthread -std=c++11")

file(GLOB PROJECT_HEADERS *.h)
file(GLOB PROJECT_SOURCES *.cpp)

add_executable(glhello ${PROJECT_SOURCES} ${PROJECT_HEADERS})

target_link_libraries(glhello ${ALL_LIBS})