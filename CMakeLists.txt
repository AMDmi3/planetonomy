PROJECT(planetonomy)

# meta
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# flags
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -pedantic")

# depends
IF(NOT EXISTS ${PROJECT_SOURCE_DIR}/extlibs/libSDL2pp/CMakeLists.txt)
	MESSAGE(FATAL_ERROR "The source directory\n  ${PROJECT_SOURCE_DIR}/extlibs/libSDL2pp\ndoes not contain a CMakeLists.txt file.\nIt is likely that you forgot to run\n  git submodule init && git submodule update")
ENDIF(NOT EXISTS ${PROJECT_SOURCE_DIR}/extlibs/libSDL2pp/CMakeLists.txt)

SET(SDL2PP_WITH_IMAGE TRUE)
SET(SDL2PP_WITH_TTF FALSE)
ADD_SUBDIRECTORY(extlibs/libSDL2pp)

FIND_PACKAGE(PUGIXML REQUIRED)

# datadir
ADD_DEFINITIONS(-DDATADIR="${PROJECT_SOURCE_DIR}/data")

# sources
SET(PLANETONOMY_SOURCES
	src/Application.cc
	src/GameMap.cc
	src/GameScene.cc
	src/LowresPainter.cc
	src/Main.cc
	src/Scene.cc
	src/Sprites.cc
)

SET(PLANETONOMY_HEADERS
	src/Application.hh
	src/Constants.hh
	src/GameMap.hh
	src/GameScene.hh
	src/LowresPainter.hh
	src/Physics.hh
	src/Scene.hh
	src/Sprites.hh
)

# binary
INCLUDE_DIRECTORIES(SYSTEM ${SDL2PP_INCLUDE_DIRS} ${PUGIXML_INCLUDE_DIR})
ADD_EXECUTABLE(planetonomy ${PLANETONOMY_SOURCES} ${PLANETONOMY_HEADERS})
TARGET_LINK_LIBRARIES(planetonomy ${SDL2PP_LIBRARIES} ${PUGIXML_LIBRARY})
