CMAKE_MINIMUM_REQUIRED(VERSION 3.9)
PROJECT(myopengl)

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(GLUT REQUIRED)
FIND_PACKAGE(glfw3 REQUIRED)
FIND_PACKAGE(GLEW REQUIRED)
FIND_LIBRARY(freeimage_library freeimage)
FIND_LIBRARY(freeimage_debug_library freeimage)
INCLUDE_DIRECTORIES(glad/include/)
SET(sub_dirs hello-window)
#-lGLEW -lglfw3 -lGL -lX11 -lXxf86vm -lpthread -ldl -lXrandr -lXinerama -lXcursor
FOREACH(sub_dir ${sub_dirs})
	FILE(GLOB SOURCE ${CMAKE_CURRENT_LIST_DIR}/${sub_dir}/*.cpp)
	SET(glad_src ${CMAKE_CURRENT_LIST_DIR}/glad/src/glad.c)
	ADD_EXECUTABLE(${sub_dir} ${SOURCE} ${glad_src})

	TARGET_LINK_LIBRARIES(${sub_dir} PRIVATE OpenGL::GL GLUT::GLUT GLEW::GLEW glfw)
	IF(freeimage_library)
	  TARGET_LINK_LIBRARIES(${sub_dir} PRIVATE optimized ${freeimage_library})
	ENDIF()
	IF(freeimage_debug_library)
	  TARGET_LINK_LIBRARIES(${sub_dir} PRIVATE debug ${freeimage_debug_library})
	ENDIF()

ENDFOREACH()



