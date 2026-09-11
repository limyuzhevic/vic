# Brain components

file(GLOB BRAIN_SOURCES
    "brain/Neuron.cpp"
    "brain/Synapse.cpp"
    "brain/NeuralPopulation.cpp"
    "brain/NeuralRegion.cpp"
    "brain/Brain.cpp"
)

add_library(nlm_brain STATIC ${BRAIN_SOURCES})

target_include_directories(nlm_brain PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nlm_brain PUBLIC nlm_core)