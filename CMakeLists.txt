cmake_minimum_required(VERSION 3.10)

# Set the project name
project(monai)

# Add the executable
add_executable(monai ${CMAKE_CURRENT_SOURCE_DIR}/src/ResourceMonitor/ResourceMonitor.cpp)
