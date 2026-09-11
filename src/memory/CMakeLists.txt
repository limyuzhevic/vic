# Memory components

file(GLOB MEMORY_SOURCES
    "memory/WorkingMemory.cpp"
    "memory/EpisodicMemory.cpp"
    "memory/SemanticMemory.cpp"
    "memory/ProceduralMemory.cpp"
    "memory/AssociativeMemory.cpp"
    "memory/NeuralWorkingMemory.cpp"
    "memory/NeuralEpisodicMemory.cpp"
)

add_library(nlm_memory STATIC ${MEMORY_SOURCES})

target_include_directories(nlm_memory PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nlm_memory PUBLIC nlm_core nlm_plasticity nlm_neuromodulation)