# NLM Cognition Layer - Higher-level cognitive functions

# Neural planner
add_library(nlm_neural_planner STATIC
    src/cognition/NeuralPlanner.cpp
)

target_include_directories(nlm_neural_planner PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/cognition>
    $<INSTALL_INTERFACE:include/cognition>
)

# Concept formation
add_library(nlm_concept_formation STATIC
    src/cognition/ConceptFormation.cpp
)

target_include_directories(nlm_concept_formation PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/cognition>
    $<INSTALL_INTERFACE:include/cognition>
)

# Cognition layer interface library
add_library(nlm_cognition INTERFACE)
target_link_libraries(nlm_cognition INTERFACE
    nlm_neural_planner
    nlm_concept_formation
)

# Cognition layer installation
install(TARGETS nlm_neural_planner nlm_concept_formation
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/cognition
    DESTINATION include/cognition
    FILES_MATCHING PATTERN "*.hpp"
)

# Cognition interface header
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/cognition/include/nlm_cognition.hpp
    DESTINATION include/cognition
)