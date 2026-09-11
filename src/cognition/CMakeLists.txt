# Cognition components

file(GLOB COGNITION_SOURCES
    "cognition/ConceptFormation.cpp"
    "cognition/NeuralPlanner.cpp"
)

add_library(nlm_cognition STATIC ${COGNITION_SOURCES})

target_include_directories(nlm_cognition PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nlm_cognition PUBLIC nlm_core nlm_brain nlm_neuromodulation nlm_memory)