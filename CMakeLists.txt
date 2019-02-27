CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(TextureQuilting)

find_package( OpenCV REQUIRED )
find_package( NLopt REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS} ${NLopt_INCLUDE_DIRS}  ${TextureQuilting_SOURCE_DIR}/includes)
file(GLOB SOURCES
    ./includes/*.h
    ./src/*.cpp
)

add_compile_options("-g" "-O2")

ADD_EXECUTABLE(testGeneration testGeneration.cpp ${SOURCES})
ADD_EXECUTABLE(testTransfert testTransfert.cpp ${SOURCES})
ADD_EXECUTABLE(testWeathering testWeathering.cpp ${SOURCES} )
ADD_EXECUTABLE(grabcut grabcut.cpp )

target_link_libraries( testGeneration ${OpenCV_LIBS} ${NLOPT_LIBRARIES}  "-lm")
target_link_libraries( testTransfert ${OpenCV_LIBS}  ${NLOPT_LIBRARIES} "-lm")
target_link_libraries( testWeathering ${OpenCV_LIBS} ${NLOPT_LIBRARIES} "-lm")
target_link_libraries( grabcut ${OpenCV_LIBS} )
