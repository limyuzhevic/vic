cmake_minimum_required(VERSION 3.16)

# Cognition system module (nlm_cognition)
get_filename_component(NLM_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
get_filename_component(NLM_ROOT_DIR ${NLM_SRC_DIR} DIRECTORY)

# Create cognition system libraries
add_library(nlm_cognition
    cognition/NeuralPlanner.cpp
    cognition/ConceptFormation.cpp
    cognition/AttentionalSelection.cpp
)

target_include_directories(nlm_cognition PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)

# Cognition-specific test target
if(NLM_BUILD_TESTS)
    add_executable(nlm_cognition_test
        cognition/tests/test_cognition.cpp
        cognition/tests/test_planner.cpp
        cognition/tests/test_concept_formation.cpp
    )
    
    target_link_libraries(nlm_cognition_test PRIVATE nlm_cognition)
    target_compile_features(nlm_cognition_test PRIVATE cxx_std_20)
    
    add_test(NAME nlm_cognition_test COMMAND nlm_cognition_test)
endif()