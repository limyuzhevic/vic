# NLM Sensory Layer - Sensory input and perception

# Sensory input
add_library(nlm_sensory_input STATIC
    src/sensory/SensoryInput.cpp
)

target_include_directories(nlm_sensory_input PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/sensory>
    $<INSTALL_INTERFACE:include/sensory>
)

# Vision
add_library(nlm_vision STATIC
    src/sensory/Vision.cpp
)

target_include_directories(nlm_vision PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/sensory>
    $<INSTALL_INTERFACE:include/sensory>
)

# Audio
add_library(nlm_audio STATIC
    src/sensory/Audio.cpp
)

target_include_directories(nlm_audio PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/sensory>
    $<INSTALL_INTERFACE:include/sensory>
)

# Internal signals
add_library(nlm_internal_signals STATIC
    src/sensory/InternalSignals.cpp
)

target_include_directories(nlm_internal_signals PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/sensory>
    $<INSTALL_INTERFACE:include/sensory>
)

# Sensory layer interface library
add_library(nlm_sensory INTERFACE)
target_link_libraries(nlm_sensory INTERFACE
    nlm_sensory_input
    nlm_vision
    nlm_audio
    nlm_internal_signals
)

# Sensory layer installation
install(TARGETS nlm_sensory_input nlm_vision nlm_audio nlm_internal_signals
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/sensory
    DESTINATION include/sensory
    FILES_MATCHING PATTERN "*.hpp"
)

# Sensory interface header
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/sensory/include/nlm_sensory.hpp
    DESTINATION include/sensory
)