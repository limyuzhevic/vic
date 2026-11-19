# Add the enhanced main demo target
# This provides better modularity and command-line interface compared to the original main.cpp

add_executable(nlm_main_demo
    src/main/NLMMainDemo.cpp
)

target_link_libraries(nlm_main_demo PUBLIC nlm_core)

# Add the new demo system as an alternative to the original main
# It provides better modularity, command-line interface, and easier extension

# The enhanced main demo can be built as:
#   ./nlm_main_demo --mode 0 --steps 1000
#   ./nlm_main_demo --mode 1 --config custom.cfg
#   ./nlm_main_demo --mode 3 --verbose

# For backward compatibility, the original nlm executable still uses the original main.cpp