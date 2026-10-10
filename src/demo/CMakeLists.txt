# Demo applications

# Enhanced demo executable
add_executable(nlm_enhanced_demo
    EnhancedDemo.cpp
)
target_link_libraries(nlm_enhanced_demo PUBLIC nlm_core nlm_agent nlm_world)