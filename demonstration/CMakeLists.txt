// demonstration/CMakeLists.txt
# CMake file for demonstration programs

add_executable(modern_cli_demo modern_cli_demo.cpp)
target_link_libraries(modern_cli_demo nlm_core nlohmann_json::nlohmann_json)

# Add dependencies
add_dependencies(modern_cli_demo nlm_core)