cmake_minimum_required(VERSION 3.10)
project(NLM_Configuration_System VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configuration system source files
set(CONFIG_SOURCES
    src/core/Config/Config.cpp
)

# New configuration system source files
set(NEW_CONFIG_SOURCES
    src/core/ConfigurationManager.cpp
    src/core/NestedConfiguration.cpp
    src/core/Schema/ConfigurationSchema.cpp
    src/core/Validation/ConfigurationValidator.cpp
    src/core/Templates/ConfigurationTemplates.cpp
    src/core/Dynamic/ConfigurationPerformanceAnalyzer.cpp
    src/plugins/Configuration/PluginConfiguration.cpp
    src/plugins/Configuration/PluginConfigurationValidator.cpp
    src/plugins/Configuration/PluginConfigurationTemplates.cpp
    src/plugins/Configuration/DynamicPluginConfiguration.cpp
    src/system/SystemConfiguration.cpp
    src/system/ComponentConfiguration.cpp
    src/system/EnvironmentConfiguration.cpp
    src/system/UserConfiguration.cpp
    src/core/ConfigurationInheritance.cpp
    src/core/ConfigurationBackup.cpp
    src/core/ConfigurationLogging.cpp
    src/core/ConfigurationMonitoring.cpp
    src/core/ConfigurationVersioning.cpp
)

# Configuration system headers
set(NEW_CONFIG_HEADERS
    src/core/ConfigurationManager.hpp
    src/core/NestedConfiguration.hpp
    src/core/Schema/ConfigurationSchema.hpp
    src/core/Validation/ConfigurationValidator.hpp
    src/core/Templates/ConfigurationTemplates.hpp
    src/core/Dynamic/ConfigurationPerformanceAnalyzer.hpp
    src/plugins/Configuration/PluginConfiguration.hpp
    src/plugins/Configuration/PluginConfigurationValidator.hpp
    src/plugins/Configuration/PluginConfigurationTemplates.hpp
    src/plugins/Configuration/DynamicPluginConfiguration.hpp
    src/system/SystemConfiguration.hpp
    src/system/ComponentConfiguration.hpp
    src/system/EnvironmentConfiguration.hpp
    src/system/UserConfiguration.hpp
    src/core/ConfigurationInheritance.hpp
    src/core/ConfigurationBackup.hpp
    src/core/ConfigurationLogging.hpp
    src/core/ConfigurationMonitoring.hpp
    src/core/ConfigurationVersioning.hpp
)

# Existing configuration system files
set(EXISTING_CONFIG_HEADERS
    src/core/Config/Config.hpp
    src/plugins/PluginConfig.hpp
    src/performance/ConfigurationPerformanceAnalyzer.hpp
)

# Create library
add_library(nlm_config ${CONFIG_SOURCES} ${NEW_CONFIG_SOURCES})

# Include directories
target_include_directories(nlm_config PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/Config
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/Schema
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/Validation
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/Templates
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/Dynamic
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/Configuration
    ${CMAKE_CURRENT_SOURCE_DIR}/src/system
    ${CMAKE_CURRENT_SOURCE_DIR}/src/performance
)

# Link dependencies
find_package(Threads REQUIRED)
target_link_libraries(nlm_config PUBLIC Threads::Threads)

# Configuration tests
add_executable(test_config_runner tests/test_main.cpp)
target_link_libraries(test_config_runner nlm_config)

# Installation
install(TARGETS nlm_config DESTINATION lib)
install(FILES ${NEW_CONFIG_HEADERS} DESTINATION include/nlm/core)
install(FILES ${EXISTING_CONFIG_HEADERS} DESTINATION include)
install(FILES tests/test_main.cpp DESTINATION tests)

# Add test target
enable_testing()
add_test(NAME configuration_tests COMMAND test_config_runner)
