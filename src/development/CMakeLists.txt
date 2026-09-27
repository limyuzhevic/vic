# NLM Development Layer - Neural development and maturation

# Development system
add_library(nlm_development_system STATIC
    src/development/DevelopmentSystem.cpp
)

target_include_directories(nlm_development_system PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/development>
    $<INSTALL_INTERFACE:include/development>
)

# Synaptogenesis
add_library(nlm_synaptogenesis STATIC
    src/development/Synaptogenesis.cpp
)

target_include_directories(nlm_synaptogenesis PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/development>
    $<INSTALL_INTERFACE:include/development>
)

# Pruning
add_library(nlm_pruning STATIC
    src/development/Pruning.cpp
)

target_include_directories(nlm_pruning PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/development>
    $<INSTALL_INTERFACE:include/development>
)

# Maturation
add_library(nlm_maturation STATIC
    src/development/Maturation.cpp
)

target_include_directories(nlm_maturation PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/development>
    $<INSTALL_INTERFACE:include/development>
)

# Development layer interface library
add_library(nlm_development INTERFACE)
target_link_libraries(nlm_development INTERFACE
    nlm_development_system
    nlm_synaptogenesis
    nlm_pruning
    nlm_maturation
)

# Development layer installation
install(TARGETS nlm_development_system nlm_synaptogenesis nlm_pruning nlm_maturation
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/development
    DESTINATION include/development
    FILES_MATCHING PATTERN "*.hpp"
)

# Development interface header
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/development/include/nlm_development.hpp
    DESTINATION include/development
)