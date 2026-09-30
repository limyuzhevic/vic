# NLM Brain Command-Line Tools
# CMake configuration for command-line inspection and control tools

# Add executables for each command-line tool

# nlm_info tool
add_executable(nlm_info nlm_info.cpp)
target_link_libraries(nlm_info nlm_core)
add_custom_target(nlm_info_deps DEPENDS nlm_core)

# nlm_save tool
add_executable(nlm_save nlm_save.cpp)
target_link_libraries(nlm_save nlm_core)
add_custom_target(nlm_save_deps DEPENDS nlm_core)

# nlm_load tool
add_executable(nlm_load nlm_load.cpp)
target_link_libraries(nlm_load nlm_core)
add_custom_target(nlm_load_deps DEPENDS nlm_core)

# nlm_status tool
add_executable(nlm_status nlm_status.cpp)
target_link_libraries(nlm_status nlm_core)
add_custom_target(nlm_status_deps DEPENDS nlm_core)

# nlm_metrics tool
add_executable(nlm_metrics nlm_metrics.cpp)
target_link_libraries(nlm_metrics nlm_core experiments)
add_custom_target(nlm_metrics_deps DEPENDS nlm_core experiments)

# Installation targets
install(TARGETS nlm_info nlm_save nlm_load nlm_status nlm_metrics
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Copy configuration file for reference
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../configs/default.cfg
    DESTINATION ${CMAKE_INSTALL_DATAROOT}/nlm
    RENAME default.cfg.example
)

# Add tools to the main build target
add_custom_target(nlm_tools ALL
    DEPENDS nlm_info nlm_save nlm_load nlm_status nlm_metrics
    COMMENT "Building NLM brain command-line tools"
)

# Add tools to the phase6 demo target dependencies
add_dependencies(nlm_phase6_demo nlm_tools)
