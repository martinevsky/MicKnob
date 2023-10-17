cmake_minimum_required(VERSION 3.20)

project(MicKnob VERSION 1.0 LANGUAGES CXX)

file(GLOB SOURCES CONFIGURE_DEPENDS "*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})