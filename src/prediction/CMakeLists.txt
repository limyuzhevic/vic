# Prediction components

file(GLOB PREDICTION_SOURCES
    "prediction/PredictionSystem.cpp"
    "prediction/NeuralPrediction.cpp"
)

add_library(nlm_prediction STATIC ${PREDICTION_SOURCES})

target_include_directories(nlm_prediction PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nlm_prediction PUBLIC nlm_core nlm_brain nlm_neuromodulation nlm_memory)