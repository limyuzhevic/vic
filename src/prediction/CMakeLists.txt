cmake_minimum_required(VERSION 3.16)

# Prediction module CMakeLists.txt
# Contains: PredictionSystem, NeuralPrediction

add_library(nlm_prediction STATIC
    prediction/PredictionSystem.cpp
    prediction/NeuralPrediction.cpp
)

target_include_directories(nlm_prediction PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_prediction PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

find_package(nlm_core REQUIRED)
target_link_libraries(nlm_prediction PUBLIC nlm_core)

install(TARGETS nlm_prediction
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY prediction/
    DESTINATION include/prediction
    FILES_MATCHING PATTERN "*.hpp"
)
