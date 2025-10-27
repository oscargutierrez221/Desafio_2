cmake_minimum_required(VERSION 3.16)

project(Codigo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Codigo main.cpp
    clases.h clases.h
)

include(GNUInstallDirs)
install(TARGETS Codigo
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
