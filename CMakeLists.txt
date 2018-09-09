cmake_minimum_required(VERSION 3.2)
project(tsab)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/)

file(GLOB_RECURSE SOURCE_FILES src/main.cpp src/tsab.cpp)
add_executable(tsab ${SOURCE_FILES})

include_directories(src/ libs/include/)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -std=c++14 -std=c++1z -std=c++17 -std=gnu++17")

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

if(SDL2_FOUND)
	include_directories(${SDL2_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIR})
	target_link_libraries(tsab ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARIES} ${SDL2_IMAGE_LIBRARY} ${SDL2_MIXER_LIBRARY})
endif()

link_directories(libs/)
target_link_libraries(tsab ${CMAKE_SOURCE_DIR}/libs/SDL_GPU/libSDL2_gpu.a)
target_link_libraries(tsab ${CMAKE_SOURCE_DIR}/libs/SDL_GPU/libSDL2_gpu.so)
target_link_libraries(tsab ${CMAKE_SOURCE_DIR}/libs/Lua/liblua.a)

#find_package(LuaJIT REQUIRED)

#if(LUAJIT_FOUND)
#	include_directories(${LUAJIT_INCLUDE_DIR})
#	target_link_libraries(tsab ${LUAJIT_LIBRARIES})
#endif()

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )
target_link_libraries(tsab ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )