cmake_minimum_required(VERSION 3.16)

project(nlm_prediction VERSION 1.0.0 LANGUAGES CXX)

# Prediction library sources

set(PREDICTION_SOURCES
    PredictionSystem.cpp
    NeuralPrediction.cpp
)

# Add prediction library
add_library(nlm_prediction STATIC ${PREDICTION_SOURCES})

target_include_directories(nlm_prediction PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../prediction
)

target_link_libraries(nlm_prediction PUBLIC nlm_core nlm_brain nlm_memory)

# Install the prediction library
install(TARGETS nlm_prediction
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY ../prediction/ DESTINATION include/nlm/prediction
    FILES_MATCHING PATTERN "*.hpp"
)
