cmake_minimum_required(VERSION 3.16)

SET(CMAKE_CXX_STANDARD 20)

SET(ASIOCTSCONCUR_PROJECT_NAME ASIOCTSCONCUR)
SET(ASIOCTSCONCUR_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(ASIOCTSCONCUR_LIBRARY ${ASIOCTSCONCUR_PROJECT_NAME})
SET(ASIOCTSCONCUR_INCLUDE_DIR ${ASIOCTSCONCUR_SOURCE_DIR}/include)

PROJECT(${ASIOCTSCONCUR_PROJECT_NAME})

FIND_PACKAGE(Boost 1.72 COMPONENTS system)
FIND_PACKAGE(Threads)

ADD_LIBRARY(${ASIOCTSCONCUR_LIBRARY} INTERFACE)

TARGET_LINK_LIBRARIES(${ASIOCTSCONCUR_LIBRARY} INTERFACE ${Boost_LIBRARIES})
TARGET_INCLUDE_DIRECTORIES(${ASIOCTSCONCUR_LIBRARY} INTERFACE ${Boost_INCLUDE_DIR} ${ASIOCTSCONCUR_INCLUDE_DIR})

ADD_SUBDIRECTORY(test)