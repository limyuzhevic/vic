# Add plasticity manager to the main core library

# Add PlasticityRuleManager sources
add_library(nlm_plasticity_manager STATIC
    src/plasticity/PlasticityRuleManager.cpp
)

target_include_directories(nlm_plasticity_manager PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_plasticity_manager PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Link plasticity manager to core library
target_link_libraries(nlm_plasticity_manager PUBLIC nlm_core)

# Add to main executable if needed
# target_link_libraries(nlm PUBLIC nlm_plasticity_manager)
