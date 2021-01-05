cmake_minimum_required (VERSION 3.10)

include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost)

include_directories(${Boost_INCLUDE_DIRS})

add_executable (veeam_test "veeam_test.cpp" )

target_link_libraries(veeam_test ${Boost_LIBRARIES})


