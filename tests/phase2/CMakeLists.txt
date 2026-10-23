add_executable(nlm_phase2_test Phase2Tests.cpp)
target_include_directories(nlm_phase2_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_phase2_test PRIVATE nlm_core)
target_compile_features(nlm_phase2_test PRIVATE cxx_std_20)

enable_testing()
add_test(NAME nlm_phase2_test COMMAND nlm_phase2_test)