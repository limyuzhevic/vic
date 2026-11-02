# SynapseNetwork component CMakeLists.txt

# Add the SynapseNetwork library
add_library(synapse_network STATIC
    brain/SynapseNetwork.cpp
)

target_include_directories(synapse_network PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/brain
)

target_compile_options(synapse_network PRIVATE
    -Wall -Wextra -Wpedantic
)