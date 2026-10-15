# Config library CMakeLists.txt

# NLM Configuration Library
add_library(nlm_config STATIC
    Config.cpp
    AdvancedConfig.cpp
)

target_include_directories(nlm_config PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nlm_config PUBLIC
    # Add any dependencies here
)

# Add configuration validation implementations
add_library(nlm_config_validation STATIC
    validators/ConfigValidator.cpp
)

target_include_directories(nlm_config_validation PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(nlm_config_validation PUBLIC nlm_config)

# Installation targets
install(TARGETS nlm_config nlm_config_validation
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES Config.hpp AdvancedConfig.hpp
    DESTINATION include/nlm/config
)