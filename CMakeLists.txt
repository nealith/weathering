CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(Weathering)

find_package( OpenCV REQUIRED )
find_package( NLopt REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS} ${NLopt_INCLUDE_DIRS}  ${Weathering_SOURCE_DIR}/includes ${Weathering_SOURCE_DIR}/lib/TextureQuilting/includes)
file(GLOB SOURCES
    ./src/*.cpp
)

add_compile_options("-g" "-O2")

#TextureQuilting

add_subdirectory(${Weathering_SOURCE_DIR}/lib/TextureQuilting)

ADD_EXECUTABLE(testWeathering testWeathering.cpp ${SOURCES} )
ADD_EXECUTABLE(grabcut grabcut.cpp )

target_link_libraries( testWeathering ${OpenCV_LIBS} ${NLOPT_LIBRARIES} TextureQuilting "-lm")
target_link_libraries( grabcut ${OpenCV_LIBS} )
