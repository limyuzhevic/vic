# World system

add_subdirectory(brain)
add_subdirectory(dynamics)
add_subdirectory(plasticity)
add_subdirectory(development)
add_subdirectory(neuromodulation)
add_subdirectory(memory)
add_subdirectory(cognition)
add_subdirectory(sensory)
add_subdirectory(prediction)
add_subdirectory(motor)
add_subdirectory(environment)
add_subdirectory(experiments)
add_subdirectory(visualization)

add_library(nlm_world STATIC
    SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nlm_world PUBLIC nlm_core)