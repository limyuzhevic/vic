cmake_minimum_required(VERSION 3.16)

# Sensory module library
add_library(nlm_sensory
    Vision.cpp
    Audio.cpp
    SensoryInput.cpp
    InternalSignals.cpp
)

target_include_directories(nlm_sensory PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/sensory>
    $<INSTALL_INTERFACE:include/sensory>
)

# Sensory test target
if(NLM_BUILD_TESTS)
    add_executable(nlm_sensory_test
        ../tests/systems/test_neuromodulation.cpp
    )
    
    target_link_libraries(nlm_sensory_test PRIVATE nlm_sensory)
    target_compile_features(nlm_sensory_test PRIVATE cxx_std_20)
    
    add_test(NAME nlm_sensory_test COMMAND nlm_sensory_test)
endif()