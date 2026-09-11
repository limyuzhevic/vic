# Visualization components

file(GLOB VISUALIZATION_SOURCES
    "visualization/VisualizationInterface.cpp"
)

add_library(nlm_visualization STATIC ${VISUALIZATION_SOURCES})

target_include_directories(nlm_visualization PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nlm_visualization PUBLIC nlm_core nlm_environment)