# Add the new demo system to the build
add_executable(nlm_main_demo
    src/main/NLMMainDemo.cpp
    src/main/NLMMainDemo.hpp
    src/demos/DemoFramework.cpp
    src/demos/DemoFramework.hpp
)

target_link_libraries(nlm_main_demo PUBLIC nlm_core)

# Add the enhanced main demo as an alternative to the original main
# It provides better modularity and command-line interface

# Set source directory for includes
target_include_directories(nlm_main_demo PRIVATE src)