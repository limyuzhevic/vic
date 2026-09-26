cmake_minimum_required(VERSION 3.10)

# Set module name
add_library(utilities
    Utilities.hpp
    ValueScaling.cpp
    NeuronGroupDistribution.cpp
    ActivityCalculation.cpp
    NoveltySimilarity.cpp
    ConfigurationAccess.cpp
)

# Set target properties
target_include_directories(utilities
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(utilities
    PUBLIC
        cxx_std_11
)

# Set library properties
set_target_properties(utilities PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    EXPORT_NAME "nlm-utilities"
    VERSION "1.0.0"
    SOVERSION "1"
)

# Add usage examples
add_executable(utilities_example
    examples/ValueScalingExample.cpp
    examples/NeuronDistributionExample.cpp
    examples/ActivityCalculationExample.cpp
    examples/NoveltySimilarityExample.cpp
    examples/ConfigurationAccessExample.cpp
)

target_link_libraries(utilities_example utilities)
