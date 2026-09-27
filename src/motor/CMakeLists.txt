# NLM Motor Layer - Action and control

# Motor system
add_library(nlm_motor_system STATIC
    src/motor/MotorSystem.cpp
)

target_include_directories(nlm_motor_system PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/motor>
    $<INSTALL_INTERFACE:include/motor>
)

# Action
add_library(nlm_action STATIC
    src/motor/Action.cpp
)

target_include_directories(nlm_action PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/motor>
    $<INSTALL_INTERFACE:include/motor>
)

# Motor layer interface library
add_library(nlm_motor INTERFACE)
target_link_libraries(nlm_motor INTERFACE
    nlm_motor_system
    nlm_action
)

# Motor layer installation
install(TARGETS nlm_motor_system nlm_action
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/motor
    DESTINATION include/motor
    FILES_MATCHING PATTERN "*.hpp"
)

# Motor interface header
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/motor/include/nlm_motor.hpp
    DESTINATION include/motor
)