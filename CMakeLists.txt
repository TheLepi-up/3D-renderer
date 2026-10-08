cmake_minimum_required(VERSION 3.5.0)
project(3D_maze VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories(include)
find_package(SFML 3 REQUIRED COMPONENTS System Window Graphics)

file(GLOB SOURCES "src/*.cpp")

add_compile_options(-Wall -Ofast -g -flto -ffast-math -ftree-vectorize -march=native)
# add_compile_options(-Wall -Og -g)
# add_compile_options(-fsanitize=address)
# add_link_options("-fsanitize=address")
add_compile_definitions(DCHECK)
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE SFML::System SFML::Window SFML::Graphics boost_thread)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)