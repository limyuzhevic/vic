# NLM Prediction Layer - Prediction and inference

# Prediction system
add_library(nlm_prediction_system STATIC
    src/prediction/PredictionSystem.cpp
)

target_include_directories(nlm_prediction_system PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/prediction>
    $<INSTALL_INTERFACE:include/prediction>
)

# Neural prediction
add_library(nlm_neural_prediction STATIC
    src/prediction/NeuralPrediction.cpp
)

target_include_directories(nlm_neural_prediction PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/prediction>
    $<INSTALL_INTERFACE:include/prediction>
)

# Prediction layer interface library
add_library(nlm_prediction INTERFACE)
target_link_libraries(nlm_prediction INTERFACE
    nlm_prediction_system
    nlm_neural_prediction
)

# Prediction layer installation
install(TARGETS nlm_prediction_system nlm_neural_prediction
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/prediction
    DESTINATION include/prediction
    FILES_MATCHING PATTERN "*.hpp"
)

# Prediction interface header
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/prediction/include/nlm_prediction.hpp
    DESTINATION include/prediction
)