cmake_minimum_required(VERSION 2.8)

project(CivDiplomatic)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

file(GLOB SRC "src/*.cpp")
add_executable(CivDiplomatic ${SRC})

find_package(ImageMagick COMPONENTS Magick++ REQUIRED)
include_directories(${ImageMagick_Magick++_INCLUDE_DIRS})
target_link_libraries(CivDiplomatic ${ImageMagick_LIBRARIES} ${ImageMagick_Magick++_LIBRARIES})
