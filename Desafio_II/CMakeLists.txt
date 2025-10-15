cmake_minimum_required(VERSION 3.16)

project(Desafio_II LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Desafio_II main.cpp)

include(GNUInstallDirs)
install(TARGETS Desafio_II
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
