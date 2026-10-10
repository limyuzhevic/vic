cmake_minimum_required(VERSION 3.16)

# World module library
add_library(nlm_world
    SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/world>
    $<INSTALL_INTERFACE:include/world>
)

target_link_libraries(nlm_world PUBLIC nlm_core)

# World test target
if(NLM_BUILD_TESTS)
    add_executable(nlm_world_test
        ../tests/systems/test_world.cpp
    )
    
    target_link_libraries(nlm_world_test PRIVATE nlm_world)
    target_compile_features(nlm_world_test PRIVATE cxx_std_20)
    
    add_test(NAME nlm_world_test COMMAND nlm_world_test)
endif()