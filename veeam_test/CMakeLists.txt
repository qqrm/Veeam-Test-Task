# CMakeList.txt : CMake project for Veeam Test Task, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (CMakeTarget "veeam_test.cpp" "veeam_test.h" "generator.h")

# TODO: Add tests and install targets if needed.
