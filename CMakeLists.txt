cmake_minimum_required(VERSION 3.0.0)
project(learn-opengl-record VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(learn-opengl-record main.cpp)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
target_link_libraries(learn-opengl-record PRIVATE glm::glm)
target_link_libraries(learn-opengl-record PRIVATE glfw)
target_link_libraries(learn-opengl-record PRIVATE glad::glad)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
