# NLM Neural Layer - Fundamental neural components

# Neuron
add_library(nlm_neuron STATIC
    src/brain/Neuron.cpp
)

target_include_directories(nlm_neuron PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/brain>
    $<INSTALL_INTERFACE:include/neural>
)

# Synapse
add_library(nlm_synapse STATIC
    src/brain/Synapse.cpp
)

target_include_directories(nlm_synapse PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/brain>
    $<INSTALL_INTERFACE:include/neural>
)

# Neural population
add_library(nlm_neural_population STATIC
    src/brain/NeuralPopulation.cpp
)

target_include_directories(nlm_neural_population PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/brain>
    $<INSTALL_INTERFACE:include/neural>
)

# Neural region
add_library(nlm_neural_region STATIC
    src/brain/NeuralRegion.cpp
)

target_include_directories(nlm_neural_region PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/brain>
    $<INSTALL_INTERFACE:include/neural>
)

# Brain
add_library(nlm_brain STATIC
    src/brain/Brain.cpp
)

target_include_directories(nlm_brain PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/brain>
    $<INSTALL_INTERFACE:include/neural>
)

# Neural dynamics (from dynamics directory)
add_library(nlm_neural_dynamics STATIC
    src/dynamics/NeuralDynamics.cpp
)

target_include_directories(nlm_neural_dynamics PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/dynamics>
    $<INSTALL_INTERFACE:include/neural>
)

# Neural layer interface library
add_library(nlm_neural INTERFACE)
target_link_libraries(nlm_neural INTERFACE
    nlm_neuron
    nlm_synapse
    nlm_neural_population
    nlm_neural_region
    nlm_brain
    nlm_neural_dynamics
)

# Neural layer installation
install(TARGETS nlm_neuron nlm_synapse nlm_neural_population nlm_neural_region nlm_brain nlm_neural_dynamics
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/brain
    DESTINATION include/neural/brain
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/dynamics
    DESTINATION include/neural/dynamics
    FILES_MATCHING PATTERN "*.hpp"
)

# Neural interface header
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/neural/include/nlm_neural.hpp
    DESTINATION include/neural
)