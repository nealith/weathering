CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(TextureQuilting)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS}  ${TextureQuilting_SOURCE_DIR}/includes)
file(GLOB SOURCES
    ./includes/*.h
    ./src/*.cpp
)

ADD_EXECUTABLE(testGeneration testGeneration.cpp ${SOURCES})
ADD_EXECUTABLE(testTransfert testTransfert.cpp ${SOURCES})

target_link_libraries( testGeneration ${OpenCV_LIBS} )
target_link_libraries( testTransfert ${OpenCV_LIBS} )
