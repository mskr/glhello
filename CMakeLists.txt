cmake_minimum_required(VERSION 3.0)

project(glhello)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)

add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(Modules)

include_directories(
	glew/include/ 
	glew/lib/Release/Win32/ 
	glfw/include 
	glm/glm/ 
	glm/glm/gtc/ 
	glm/glm/gtx 
	Modules
)

link_directories(
	.
	Modules
)

set(LIBRARYS
	${OPENGL_LIBRARY}
	${CMAKE_CURRENT_SOURCE_DIR}/glew/lib/Release/Win32/glew32s.lib
	glfw
	${GLFW_LIBRARIES}
)

add_definitions(
	-DGLEW_STATIC
)

set(CMAKE_CXX_FLAGS
	"${CMAKE_CXX_FLAGS} -Wall -pedantic -pthread -std=c++11"
)

file(GLOB CORE_HEADERS *.h)
file(GLOB CORE_SOURCES *.cpp)
file(GLOB MODULE_HEADERS Modules/*.h)
file(GLOB MODULE_SOURCES Modules/*.cpp)

add_executable(${PROJECT_NAME} 
	${CORE_SOURCES} ${CORE_HEADERS}
	${MODULE_SOURCES} ${MODULE_HEADERS}
)

target_link_libraries(${PROJECT_NAME} ${LIBRARYS})