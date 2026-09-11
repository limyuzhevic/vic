# Neuromodulation components

file(GLOB NEUROMODULATION_SOURCES
    "neuromodulation/Neuromodulator.cpp"
    "neuromodulation/Reward.cpp"
    "neuromodulation/PredictionError.cpp"
    "neuromodulation/Novelty.cpp"
    "neuromodulation/Curiosity.cpp"
)

add_library(nlm_neuromodulation STATIC ${NEUROMODULATION_SOURCES})

target_include_directories(nlm_neuromodulation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nlm_neuromodulation PUBLIC nlm_core nlm_plasticity nlm_dynamics)