# NLM Neuromodulation Layer - Reward and error signals

# Neuromodulator base
add_library(nlm_neuromodulator STATIC
    src/neuromodulation/Neuromodulator.cpp
)

target_include_directories(nlm_neuromodulator PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/neuromodulation>
    $<INSTALL_INTERFACE:include/neuromodulation>
)

# Reward
add_library(nlm_reward STATIC
    src/neuromodulation/Reward.cpp
)

target_include_directories(nlm_reward PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/neuromodulation>
    $<INSTALL_INTERFACE:include/neuromodulation>
)

# Prediction error
add_library(nlm_prediction_error STATIC
    src/neuromodulation/PredictionError.cpp
)

target_include_directories(nlm_prediction_error PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/neuromodulation>
    $<INSTALL_INTERFACE:include/neuromodulation>
)

# Novelty
add_library(nlm_novelty STATIC
    src/neuromodulation/Novelty.cpp
)

target_include_directories(nlm_novelty PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/neuromodulation>
    $<INSTALL_INTERFACE:include/neuromodulation>
)

# Curiosity
add_library(nlm_curiosity STATIC
    src/neuromodulation/Curiosity.cpp
)

target_include_directories(nlm_curiosity PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/neuromodulation>
    $<INSTALL_INTERFACE:include/neuromodulation>
)

# Neuromodulation layer interface library
add_library(nlm_neuromodulation INTERFACE)
target_link_libraries(nlm_neuromodulation INTERFACE
    nlm_neuromodulator
    nlm_reward
    nlm_prediction_error
    nlm_novelty
    nlm_curiosity
)

# Neuromodulation layer installation
install(TARGETS nlm_neuromodulator nlm_reward nlm_prediction_error nlm_novelty nlm_curiosity
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/neuromodulation
    DESTINATION include/neuromodulation
    FILES_MATCHING PATTERN "*.hpp"
)

# Neuromodulation interface header
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/neuromodulation/include/nlm_neuromodulation.hpp
    DESTINATION include/neuromodulation
)