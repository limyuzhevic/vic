cmake_minimum_required(VERSION 3.16)

# Plasticity module library
add_library(nlm_plasticity
    Hebbian.cpp
    STDP.cpp
    StructuralPlasticity.cpp
    PlasticityRule.cpp
)

target_include_directories(nlm_plasticity PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/plasticity>
    $<INSTALL_INTERFACE:include/plasticity>
)

target_link_libraries(nlm_plasticity PUBLIC nlm_core_utils nlm_core_types nlm_neural)

# Plasticity test target
if(NLM_BUILD_TESTS)
    add_executable(nlm_plasticity_test
        ../tests/plasticity/test_plasticity.cpp
        ../tests/plasticity/test_stdp.cpp
        ../tests/plasticity/test_hebbian.cpp
    )
    
    target_link_libraries(nlm_plasticity_test PRIVATE nlm_plasticity)
    target_compile_features(nlm_plasticity_test PRIVATE cxx_std_20)
    
    add_test(NAME nlm_plasticity_test COMMAND nlm_plasticity_test)
endif()