cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# Include modular CMake files
include(cmake/Phase6Core.cmake)
include(cmake/Phase6Libraries.cmake)
include(cmake/Phase6Executables.cmake)
include(cmake/Phase6Testing.cmake)
include(cmake/Phase6Install.cmake)
