cmake_minimum_required(VERSION 3.15)
project(ModelExporter)

set(CMAKE_CXX_STANDARD  20)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
        libraries/assimp-5.0.1/include/assimp)

set(ZLIB_LIBRARIES zlibstatic)
set(ENABLE_BOOST_WORKAROUND ON)
set(BUILD_STATIC_LIB ON)
set(BUILD_ASSIMP_TOOLS  ON)
set(ASSIMP_BUILD_STATIC_LIB ON)

add_subdirectory(libraries/assimp-5.0.1)

add_executable(ModelExporter main.cpp)

target_link_libraries(ModelExporter assimp)