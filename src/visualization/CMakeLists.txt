# CMake Configuration for NLM Visualization

# Visualization targets
add_library(nlm_visualization STATIC
    VisualizationInterface.cpp
    AdvancedVisualization.cpp
)

target_include_directories(nlm_visualization PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nlm_visualization PUBLIC
    nlm_core
    nlm_agent
)

# Visualization executable
add_executable(nlm_visualizer
    main_visualizer.cpp
)

target_link_libraries(nlm_visualizer PUBLIC
    nlm_visualization
)

# Installation targets
install(TARGETS nlm_visualizer
    RUNTIME DESTINATION bin
)

install(TARGETS nlm_visualization
    ARCHIVE DESTINATION lib
)
