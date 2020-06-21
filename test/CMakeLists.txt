find_library(GTEST_LIB gtest)
find_library(GTEST_MAIN_LIB gtest_main)
IF (NOT GTEST_LIB OR NOT GTEST_MAIN_LIB)
    ADD_SUBDIRECTORY(googletest)
ENDIF()

SET(ASIOCTSCONCURTESTS_SOURCE_FILES main.cpp)

ADD_EXECUTABLE(ASIOCTSCONCURTESTS
        main.cpp
        unit/condition_variable.cpp
        unit/semaphore.cpp
        unit/channel.cpp
        unit/barrier.cpp
        unit/latch.cpp
        unit/sync_group.cpp)

TARGET_LINK_LIBRARIES(ASIOCTSCONCURTESTS PRIVATE gtest gtest_main ${ASIOCTSCONCUR_LIBRARY})
TARGET_INCLUDE_DIRECTORIES(ASIOCTSCONCURTESTS PRIVATE ${ASIOCTSCONCUR_INCLUDE_DIR})