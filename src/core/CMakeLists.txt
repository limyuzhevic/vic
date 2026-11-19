# Command line utilities
add_library(nlm_commandline STATIC
    CommandLineParser.cpp
)

target_include_directories(nlm_commandline PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Configuration manager
add_library(nlm_configuration STATIC
    ConfigurationManager.cpp
)

target_include_directories(nlm_configuration PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nlm_configuration PUBLIC nlm_core)

# Utilities
add_library(nlm_utilities STATIC
    Utilities/StringUtils.cpp
    Utilities/MathUtils.cpp
    Utilities/PerformanceTimer.cpp
    Utilities/DebugUtils.cpp
    Utilities/NeuralUtils.cpp
    Utilities/FileUtils.cpp
    Utilities/CollectionUtils.cpp
)

target_include_directories(nlm_utilities PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)