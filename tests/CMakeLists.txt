cmake_minimum_required(VERSION 3.16)

# Test framework setup
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Brain system tests
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../src/brain/Neuron.hpp)
    add_executable(nlm_brain_test
        brain/tests/test_brain.cpp
        brain/tests/test_neuron.cpp
        brain/tests/test_synapse.cpp
        brain/tests/test_neural_population.cpp
    )
    
    target_link_libraries(nlm_brain_test PRIVATE nlm_neural)
    target_compile_features(nlm_brain_test PRIVATE cxx_std_20)
    add_test(NAME nlm_brain_test COMMAND nlm_brain_test)
endif()

# Memory system tests
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../src/memory/Memory.hpp)
    add_executable(nlm_memory_test
        memory/tests/test_memory.cpp
        memory/tests/test_working_memory.cpp
        memory/tests/test_episodic_memory.cpp
    )
    
    target_link_libraries(nlm_memory_test PRIVATE nlm_memory)
    target_compile_features(nlm_memory_test PRIVATE cxx_std_20)
    add_test(NAME nlm_memory_test COMMAND nlm_memory_test)
endif()

# Cognition system tests
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../src/cognition/NeuralPlanner.hpp)
    add_executable(nlm_cognition_test
        cognition/tests/test_cognition.cpp
        cognition/tests/test_planner.cpp
        cognition/tests/test_concept_formation.cpp
    )
    
    target_link_libraries(nlm_cognition_test PRIVATE nlm_cognition)
    target_compile_features(nlm_cognition_test PRIVATE cxx_std_20)
    add_test(NAME nlm_cognition_test COMMAND nlm_cognition_test)
endif()

# Plasticity system tests
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../src/plasticity/PlasticityRule.hpp)
    add_executable(nlm_plasticity_test
        plasticity/tests/test_plasticity.cpp
        plasticity/tests/test_stdp.cpp
        plasticity/tests/test_hebbian.cpp
    )
    
    target_link_libraries(nlm_plasticity_test PRIVATE nlm_plasticity)
    target_compile_features(nlm_plasticity_test PRIVATE cxx_std_20)
    add_test(NAME nlm_plasticity_test COMMAND nlm_plasticity_test)
endif()

# Phase-specific tests
enable_testing()

# Phase 2 tests
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/phase_tests/Phase2Test.cpp)
    add_executable(nlm_phase2_test
        phase_tests/Phase2Test.cpp
    )
    target_link_libraries(nlm_phase2_test PRIVATE nlm_systems nlm_core)
    target_compile_features(nlm_phase2_test PRIVATE cxx_std_20)
    add_test(NAME nlm_phase2_test COMMAND nlm_phase2_test)
endif()

# Phase 3 tests
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/phase_tests/Phase3Test.cpp)
    add_executable(nlm_phase3_test
        phase_tests/Phase3Test.cpp
    )
    target_link_libraries(nlm_phase3_test PRIVATE nlm_systems nlm_core)
    target_compile_features(nlm_phase3_test PRIVATE cxx_std_20)
    add_test(NAME nlm_phase3_test COMMAND nlm_phase3_test)
endif()

# Phase 4 tests
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/phase_tests/Phase4Test.cpp)
    add_executable(nlm_phase4_test
        phase_tests/Phase4Test.cpp
    )
    target_link_libraries(nlm_phase4_test PRIVATE nlm_systems nlm_core)
    target_compile_features(nlm_phase4_test PRIVATE cxx_std_20)
    add_test(NAME nlm_phase4_test COMMAND nlm_phase4_test)
endif()

# Phase 6 tests
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/phase_tests/Phase6Test.cpp)
    add_executable(nlm_phase6_test
        phase_tests/Phase6Test.cpp
    )
    target_link_libraries(nlm_phase6_test PRIVATE nlm_systems nlm_core)
    target_compile_features(nlm_phase6_test PRIVATE cxx_std_20)
    add_test(NAME nlm_phase6_test COMMAND nlm_phase6_test)
endif()

# Combined phase tests target
add_executable(nlm_phase_tests
    phase_tests/Phase2Test.cpp
    phase_tests/Phase3Test.cpp
    phase_tests/Phase4Test.cpp
    phase_tests/Phase6Test.cpp
)
target_link_libraries(nlm_phase_tests PRIVATE nlm_systems nlm_core)
target_compile_features(nlm_phase_tests PRIVATE cxx_std_20)
add_test(NAME nlm_phase_tests COMMAND nlm_phase_tests)