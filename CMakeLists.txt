cmake_minimum_required(VERSION 3.16)

project(WayChess VERSION 1.2.3)
set(CMAKE_AUTHOR "Troy Sharples")

set(LOG_LEVEL 4)
add_compile_options(-Wall -Wextra -Wpedantic)

add_subdirectory(src)
add_subdirectory(apps)

include(CTest)
add_subdirectory(tests)

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Troy Sharples")
include(CPack)