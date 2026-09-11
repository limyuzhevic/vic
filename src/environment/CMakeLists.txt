# Environment components

file(GLOB ENVIRONMENT_SOURCES
    "environment/Environment.cpp"
    "environment/Observation.cpp"
    "environment/Action.cpp"
)

add_library(nlm_environment STATIC ${ENVIRONMENT_SOURCES})

target_include_directories(nlm_environment PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nlm_environment PUBLIC nlm_core nlm_motor nlm_sensory)