# Test Advanced Features

add_executable(test_advanced_features
    test_advanced_features/test_main.cpp
)

target_link_libraries(test_advanced_features PUBLIC nlm_core nlm_advanced_config)

enable_test_commands()
add_test(NAME test_advanced_features
    COMMAND test_advanced_features
)