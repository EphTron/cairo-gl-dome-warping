PROJECT(cairo-gl-dome-warping)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_CXX_FLAGS "-W -Wall -Wno-unused-parameter -Werror -O3 -std=c++11")

FIND_PACKAGE(OpenGL REQUIRED)

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(SDL2 REQUIRED sdl2)

PKG_CHECK_MODULES(GTK2 REQUIRED gtk+-2.0)
include_directories(${GTK2_INCLUDE_DIRS})
link_directories(${GTK2_LIBRARY_DIRS})
add_definitions(${GTK2_CFLAGS_OTHER})


INCLUDE_DIRECTORIES(
	${OPENGL_INCLUDE_DIR}
	${SDL2_INCLUDE_DIRS}
)

LINK_DIRECTORIES(
	${OPENGL_LIBRARY_DIR}
	${SDL2_LIBRARY_DIRS}
)

# Here I add the cairo paths and libraries manually
set(CAIRO_INCLUDE_DIRS "/usr/local/include/cairo")
set(CAIRO_LIBRARY_DIRS "/usr/local/lib")
set(CAIRO_LIBRARIES cairo GL)
#set(CAIRO_LIBRARIES cairo gl)

# Need to add the path BEFORE because it will override the default from GTK
include_directories(BEFORE ${CAIRO_INCLUDE_DIRS})
link_directories(${CAIRO_LIBRARY_DIRS})

ADD_EXECUTABLE(cli-benchmark "src/cli-benchmark.cpp")
ADD_EXECUTABLE(sdl-example "src/sdl-example.cpp")

TARGET_LINK_LIBRARIES(cli-benchmark
	${OPENGL_LIBRARIES}
	${CAIRO_LIBRARY_DIRS}
	${GTK2_LIBRARIES}
	${SDL2_LIBRARIES}
)

TARGET_LINK_LIBRARIES(sdl-example
	${OPENGL_LIBRARIES}
    ${CAIRO_LIBRARY_DIRS}
    ${GTK2_LIBRARIES}
	${SDL2_LIBRARIES}
)

