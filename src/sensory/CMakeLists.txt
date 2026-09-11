# Sensory components

file(GLOB SENSORY_SOURCES
    "sensory/SensoryInput.cpp"
    "sensory/Vision.cpp"
    "sensory/Audio.cpp"
    "sensory/InternalSignals.cpp"
)

add_library(nlm_sensory STATIC ${SENSORY_SOURCES})

target_include_directories(nlm_sensory PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nlm_sensory PUBLIC nlm_core nlm_brain nlm_neuromodulation)