# Motor components

file(GLOB MOTOR_SOURCES
    "motor/MotorSystem.cpp"
)

add_library(nlm_motor STATIC ${MOTOR_SOURCES})

target_include_directories(nlm_motor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nlm_motor PUBLIC nlm_core nlm_brain nlm_neuromodulation nlm_cognition)