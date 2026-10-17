# Additional Python bindings (improved)
add_library(nlm_enhanced_bindings STATIC
    ImprovedBindings.cpp
)
target_include_directories(nlm_enhanced_bindings PUBLIC src)
target_link_libraries(nlm_enhanced_bindings PUBLIC nlm_core)