# Config validators CMakeLists.txt

# Create directory structure
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/validators)

# Validators library
target_include_directories(nlm_config_validation PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/validators>
    $<INSTALL_INTERFACE:include/nlm/config/validators>
)

# Add validator source files
add_library(nlm_basic_validator STATIC
    validators/BasicConfigValidator.cpp
)

target_include_directories(nlm_basic_validator PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/validators>
)

target_link_libraries(nlm_basic_validator PUBLIC nlm_config_validation)

# Installation targets for validators
install(TARGETS nlm_basic_validator
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY validators/ DESTINATION include/nlm/config/validators
    FILES_MATCHING PATTERN "*.hpp"
)