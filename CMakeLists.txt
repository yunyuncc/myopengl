CMAKE_MINIMUM_REQUIRED(VERSION 3.9)
PROJECT(myopengl)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_COMPILER /usr/bin/g++-8)
SET(CMAKE_C_COMPILER /usr/bin/gcc-8)
INCLUDE(${CMAKE_CURRENT_LIST_DIR}/cmake/common.cmake)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(GLUT REQUIRED)
FIND_PACKAGE(glfw3 REQUIRED)
FIND_PACKAGE(GLEW REQUIRED)
FIND_PACKAGE(glm REQUIRED)
FIND_PACKAGE(ASSIMP REQUIRED)
FIND_PACKAGE(OpenCV 3.4 REQUIRED PATHS /opt)
FIND_LIBRARY(freeimage_library freeimage)
FIND_LIBRARY(freeimage_debug_library freeimage)
INCLUDE_DIRECTORIES(glad/include/)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/src/)

FILE(GLOB LIB_SRC ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
ADD_LIBRARY(myopengl SHARED ${LIB_SRC})
TARGET_LINK_LIBRARIES(myopengl PUBLIC OpenGL::GL GLUT::GLUT GLEW::GLEW glfw)
TARGET_LINK_LIBRARIES(myopengl PUBLIC ${OpenCV_LIBRARIES})
TARGET_LINK_LIBRARIES(myopengl PUBLIC optimized ${freeimage_library})
TARGET_LINK_LIBRARIES(myopengl PUBLIC stdc++fs)
TARGET_LINK_LIBRARIES(myopengl PUBLIC glm)
TARGET_LINK_LIBRARIES(myopengl PUBLIC ${ASSIMP_LIBRARIES})
add_subdirectory(test)
SET(sub_dirs 
	#01-hello-window 
	#02-hello-triangle 
	#03-shaders 
	#04-shaders 
	#05-shaders 
	#06-textures 
	#07-transformations 
	#08-coordinate-systems 
	#09-camera 
	#10-camera-class 
	#11-color 
	#12-basic-lighting 
	#13-materials
	#14-lighting-maps
	#15-light-casters 
	#16-multiple-lights
	#17-depth-testing
	#18-stencil-testing
	#19-blending
	#20-face-culling
	#21-framebuffer
	22-model
	23-cubemaps
	24-advanced_GLSL
	tmp
	)
FOREACH(sub_dir ${sub_dirs})
	SET(glad_src ${CMAKE_CURRENT_LIST_DIR}/glad/src/glad.c)
	FILE(GLOB SOURCE ${CMAKE_CURRENT_LIST_DIR}/examples/${sub_dir}/*.cpp)
	ADD_EXECUTABLE(${sub_dir} ${SOURCE} ${glad_src})

	TARGET_LINK_LIBRARIES(${sub_dir} PRIVATE myopengl)

ENDFOREACH()


