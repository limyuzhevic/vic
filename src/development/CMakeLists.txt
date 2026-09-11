# Development components

file(GLOB DEVELOPMENT_SOURCES
    "development/DevelopmentSystem.cpp"
    "development/Synaptogenesis.cpp"
    "development/Pruning.cpp"
    "development/Maturation.cpp"
)

add_library(nlm_development STATIC ${DEVELOPMENT_SOURCES})

target_include_directories(nlm_development PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nlm_development PUBLIC nlm_core nlm_plasticity nlm_dynamics)